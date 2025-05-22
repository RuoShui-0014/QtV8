#pragma once

#include <v8-inspector.h>
#include "DevToolsSession.h"

namespace rs {

class DevToolsAgent : public v8_inspector::V8InspectorClient {
public:
  explicit DevToolsAgent(v8::Isolate* isolate);
  ~DevToolsAgent() override;

  void OpenAgent();
  void CloseAgent();

  void ContextCreated(v8::Local<v8::Context> context);
  void ContextDestroyed(v8::Local<v8::Context> context);

private:
  v8::Isolate* m_isolate;
  std::unique_ptr<v8_inspector::V8Inspector> m_inspector;
  std::unique_ptr<DevToolsSession> m_channel;
};

} // rs
