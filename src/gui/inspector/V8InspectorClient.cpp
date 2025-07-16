#include "V8InspectorClient.h"

#include "V8InspectorChannel.h"
#include "include/libplatform/libplatform.h"
#include "src/v8/V8Initializator.h"
#include "utils.h"

namespace rs {
V8InspectorClient::V8InspectorClient(v8::Isolate* isolate,
                                     std::mutex& mutex,
                                     std::condition_variable& run_condition)
    : m_isolate(isolate),
      m_mutex(mutex),
      m_runCondition(run_condition),
      m_ioc{1} {}

V8InspectorClient::~V8InspectorClient() {
  contextDestroyed();
  // m_ws->close(boost::beast::websocket::close_code::normal);
  try {
    m_ws.reset();
  } catch (std::exception const& e) {
  }
}

void V8InspectorClient::connectSession() {
  auto const address = net::ip::make_address("127.0.0.1");
  tcp::acceptor acceptor{m_ioc, {address, static_cast<unsigned short>(10000)}};
  tcp::socket socket{m_ioc};
  acceptor.accept(socket);
  m_ws.reset(new websocket::stream<tcp::socket>(std::move(socket)));
  m_ws->accept();
}

void V8InspectorClient::createInspector() {
  m_channel.reset(new V8InspectorChannel(m_isolate, m_ws.get()));
  m_inspector = v8_inspector::V8Inspector::create(m_isolate, this);
  m_session = m_inspector->connect(
      1, m_channel.get(), v8_inspector::StringView(),
      v8_inspector::V8Inspector::ClientTrustLevel::kFullyTrusted);
  contextCreated();
}

void V8InspectorClient::RunLoop() {
  while (true) {
    try {
      beast::flat_buffer buffer{};
      m_ws->read(buffer);
      {
        std::unique_lock lock(m_mgsMutex);
        m_messageQueue.emplace(boost::beast::buffers_to_string(buffer.data()));
      }
      m_runCondition.notify_one();
    } catch (beast::system_error const& se) {
      if (se.code() != websocket::error::closed) {
        std::cerr << "Error: " << se.code().message() << std::endl;
      }
      if (se.code() == websocket::error::closed) {
        std::cout << "WebSocket 正常关闭" << std::endl;
        return;
      }
    } catch (std::exception const& e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }
}

void V8InspectorClient::setDefaultContext(v8::Local<v8::Context> context) {
  m_context.Reset(m_isolate, context);
}

void V8InspectorClient::contextCreated() {
  v8_inspector::StringView contextName = convertToStringView("inspector");
  m_inspector->contextCreated(
      v8_inspector::V8ContextInfo(m_context.Get(m_isolate), 1, contextName));
}

void V8InspectorClient::contextDestroyed() {
  m_inspector->contextDestroyed(m_context.Get(m_isolate));
}

bool V8InspectorClient::dispatchProtocolMessage() {
  std::unique_lock lock(m_mgsMutex);
  if (!m_messageQueue.empty()) {
    std::string message = m_messageQueue.front();
    m_messageQueue.pop();
    v8_inspector::StringView protocolMessage = convertToStringView(message);
    if (protocolMessage.length() > 0) {
      m_session->dispatchProtocolMessage(protocolMessage);
    }
  }
  return !m_messageQueue.empty();
}

void V8InspectorClient::runMessageLoopOnPause(int contextGroupId) {
  if (m_runNestedLoop) {
    return;
  }
  m_paused = true;
  m_runNestedLoop = true;
  v8::Platform* platform = V8Initializer::GetInstance().GetPlatform();
  while (m_paused) {
    while (dispatchProtocolMessage() ||
           v8::platform::PumpMessageLoop(platform, m_isolate)) {
    }
    {
      std::unique_lock lock(m_mutex);
      m_runCondition.wait_for(lock, std::chrono::milliseconds(10),
                              [this]() { return !m_messageQueue.empty(); });
    }
  }
  m_paused = false;
  m_runNestedLoop = false;
}

void V8InspectorClient::quitMessageLoopOnPause() {
  m_paused = false;
}

void V8InspectorClient::schedulePauseOnNextStatement(
    const v8_inspector::StringView& reason) {
  m_session->schedulePauseOnNextStatement(reason, reason);
}

void V8InspectorClient::waitFrontendMessageOnPause() {
  m_paused = true;
}
}  // namespace rs
