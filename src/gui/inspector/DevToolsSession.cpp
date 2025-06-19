#include "DevToolsSession.h"

namespace rs {

DevToolsSession::DevToolsSession() = default;

DevToolsSession::~DevToolsSession() = default;

void DevToolsSession::ConnectToV8(v8_inspector::V8Inspector* inspector,
                                  int context_group_id) {
  m_session = inspector->connect(
      context_group_id, this, {},
      v8_inspector::V8Inspector::ClientTrustLevel::kFullyTrusted,
      v8_inspector::V8Inspector::SessionPauseState::kNotWaitingForDebugger);
}

void DevToolsSession::DispatchProtocolCommand(const std::string& command) {
  m_session->dispatchProtocolMessage(v8_inspector::StringView(
      reinterpret_cast<const uint8_t*>(command.data()), command.size()));
}

void DevToolsSession::sendResponse(
    int callId,
    std::unique_ptr<v8_inspector::StringBuffer> message) {}

void DevToolsSession::sendNotification(
    std::unique_ptr<v8_inspector::StringBuffer> message) {}

void DevToolsSession::flushProtocolNotifications() {}

}  // namespace rs
