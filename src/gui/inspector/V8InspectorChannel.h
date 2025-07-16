#pragma once

#include <include/v8-inspector.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "V8InspectorChannel.h"

namespace beast = boost::beast;          // from <boost/beast.hpp>
namespace http = beast::http;            // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;  // from <boost/beast/websocket.hpp>
namespace net = boost::asio;             // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;        // from <boost/asio/ip/tcp.hpp>

namespace rs {

class V8InspectorChannel : public v8_inspector::V8Inspector::Channel {
 public:
  V8InspectorChannel(v8::Isolate* isolate, websocket::stream<tcp::socket>* ws);
  ~V8InspectorChannel() override;

  void sendResponse(
      int callId,
      std::unique_ptr<v8_inspector::StringBuffer> message) override;
  void sendNotification(
      std::unique_ptr<v8_inspector::StringBuffer> message) override;
  void flushProtocolNotifications() override;

 private:
  v8::Isolate* m_isolate;
  websocket::stream<tcp::socket>* m_ws;
};

}  // namespace rs
