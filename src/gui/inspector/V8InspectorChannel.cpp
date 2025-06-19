#include "V8InspectorChannel.h"
#include "utils.h"

namespace rs {
V8InspectorChannel::V8InspectorChannel(v8::Isolate* isolate,
                                       websocket::stream<tcp::socket>* ws):
  m_isolate(isolate), m_ws(ws) {
}

V8InspectorChannel::~V8InspectorChannel() = default;

void V8InspectorChannel::sendResponse(int callId,
                                      std::unique_ptr<
                                        v8_inspector::StringBuffer> message) {
  const std::string response = convertToString(m_isolate, message->string());
  boost::beast::multi_buffer b;
  boost::beast::ostream(b) << response;
  m_ws->text(m_ws->got_text());
  m_ws->write(b.data());
}

void V8InspectorChannel::sendNotification(
    std::unique_ptr<v8_inspector::StringBuffer> message) {
  const std::string response = convertToString(m_isolate, message->string());
  boost::beast::multi_buffer b;
  boost::beast::ostream(b) << response;
  m_ws->text(m_ws->got_text());
  m_ws->write(b.data());
}

void V8InspectorChannel::flushProtocolNotifications() {
}
} // rs