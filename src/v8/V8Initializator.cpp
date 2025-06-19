#include "V8Initializator.h"

#include <include/libplatform/libplatform.h>
#include <include/v8-cppgc.h>

namespace rs {

V8Initializer& V8Initializer::GetInstance() {
  static V8Initializer v8Init{};
  return v8Init;
}

V8Initializer::V8Initializer() {
  m_platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(m_platform.get());
  v8::V8::Initialize();
  cppgc::InitializeProcess(m_platform->GetPageAllocator());

  m_inited = true;
}

V8Initializer::~V8Initializer() {
  cppgc::ShutdownProcess();
  v8::V8::Dispose();
  v8::V8::DisposePlatform();
}

void V8Initializer::Init() {
  m_platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(m_platform.get());
  v8::V8::Initialize();
  cppgc::InitializeProcess(m_platform->GetPageAllocator());

  m_inited = true;
}

void V8Initializer::SetV8Flag(const char* flag) {
  v8::V8::SetFlagsFromString(flag);
}

bool V8Initializer::Inited() const {
  return m_inited;
}

v8::Platform* V8Initializer::GetPlatform() const {
  return m_platform.get();
}

std::shared_ptr<v8::TaskRunner> V8Initializer::GetTaskRunner(
    v8::Isolate* isolate) {
  return m_platform->GetForegroundTaskRunner(isolate);
}

}  // namespace rs
