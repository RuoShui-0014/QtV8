#pragma once

#include <include/v8-inspector.h>

namespace rs {
class DevToolsSession : public v8_inspector::V8Inspector::Channel {
 public:
  DevToolsSession();
  ~DevToolsSession() override;

  void ConnectToV8(v8_inspector::V8Inspector* inspector, int context_group_id);

  void DispatchProtocolCommand(const std::string& command);

  //
  void sendResponse(
      int callId,
      std::unique_ptr<v8_inspector::StringBuffer> message) override;
  void sendNotification(
      std::unique_ptr<v8_inspector::StringBuffer> message) override;
  void flushProtocolNotifications() override;

 private:
  std::unique_ptr<v8_inspector::V8InspectorSession> m_session;
};
}  // namespace rs
