#pragma once

#include <include/v8-inspector.h>
#include <queue>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace rs {

class V8InspectorChannel;

class V8InspectorClient : public v8_inspector::V8InspectorClient {
public:
  explicit V8InspectorClient(v8::Isolate* isolate, std::mutex& mutex,
                             std::condition_variable& run_condition);
  ~V8InspectorClient() override;

  void connectSession();
  void createInspector();

  void RunLoop();

  void setDefaultContext(v8::Local<v8::Context> context);
  void contextCreated();
  void contextDestroyed();

  void runMessageLoopOnPause(int contextGroupId) override;
  void quitMessageLoopOnPause() override;

  bool dispatchProtocolMessage();
  void schedulePauseOnNextStatement(const v8_inspector::StringView& reason);
  void waitFrontendMessageOnPause();

private:
  v8::Isolate* m_isolate;
  v8::Global<v8::Context> m_context;
  std::mutex& m_mutex;
  std::condition_variable& m_runCondition;
  net::io_context m_ioc;
  std::queue<std::string> m_messageQueue;
  std::mutex m_mgsMutex;
  std::unique_ptr<v8_inspector::V8Inspector> m_inspector;
  std::unique_ptr<v8_inspector::V8InspectorSession> m_session;
  std::unique_ptr<websocket::stream<tcp::socket>> m_ws;
  std::unique_ptr<V8InspectorChannel> m_channel;
  bool m_paused = false;
  bool m_runNestedLoop = false;
};
} // rs