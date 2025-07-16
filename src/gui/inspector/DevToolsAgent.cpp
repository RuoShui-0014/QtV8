#include "DevToolsAgent.h"

#include "DevToolsSession.h"

namespace rs {

DevToolsAgent::DevToolsAgent(v8::Isolate* isolate) : m_isolate(isolate) {}

DevToolsAgent::~DevToolsAgent() = default;

void DevToolsAgent::OpenAgent() {
  m_channel = std::make_unique<DevToolsSession>();
  m_inspector = v8_inspector::V8Inspector::create(m_isolate, this);
}

void DevToolsAgent::CloseAgent() {
  m_channel.reset();
  m_inspector.reset();
}

void DevToolsAgent::ContextCreated(v8::Local<v8::Context> context) {
  static std::string name = "QtV8Inspector";
  m_inspector->contextCreated(
      {context,
       1,
       {reinterpret_cast<const uint8_t*>(name.data()), name.length()}});
}

void DevToolsAgent::ContextDestroyed(v8::Local<v8::Context> context) {
  m_inspector->contextDestroyed(context);
}

}  // namespace rs
