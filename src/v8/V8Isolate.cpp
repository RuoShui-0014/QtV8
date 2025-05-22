//
// Created by Administrator on 25-1-30.
//
#include "V8Isolate.h"

#include "V8Context.h"
#include "include/libplatform/libplatform.h"
#include "include/v8-cppgc.h"

namespace rs {
V8Initializer& V8Initializer::GetInstance() {
  static V8Initializer v8Init{};
  return v8Init;
}

V8Initializer::V8Initializer() {
  Init();
  cppgc::InitializeProcess(m_platform->GetPageAllocator());
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

V8Isolate::V8Isolate() {
  /* 初始化v8引擎 */
  if (!V8Initializer::GetInstance().Inited()) {
    V8Initializer::GetInstance().Init();
  }

  InitCppgc();

  m_isolate = v8::Isolate::Allocate();
  m_create_params.cpp_heap = m_cpp_heap.get();
  m_create_params.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate::Initialize(m_isolate, m_create_params);

  m_v8isolate_scope = std::make_unique<Scope>(m_isolate);
  m_default_context = std::make_unique<V8Context>(this);
}

V8Isolate::~V8Isolate() {
  m_default_context.reset();
  m_v8isolate_scope.reset();
  m_isolate->Dispose();
  delete m_create_params.array_buffer_allocator;
}

void V8Isolate::InitCppgc() {
  m_cpp_heap = v8::CppHeap::Create(V8Initializer::GetInstance().GetPlatform(),
                                   v8::CppHeapCreateParams{{}});
}

v8::Isolate* V8Isolate::GetIsolate() {
  return m_isolate;
}
void V8Isolate::SetDefaultContext(
    v8::Local<v8::ObjectTemplate> global_template) {
  m_default_context->SetContext(global_template);
}

std::shared_ptr<V8Context> V8Isolate::GetDefaultV8Context() {
  return m_default_context;
}

v8::Local<v8::Value> V8Isolate::RunCode(const char* code) const {
  return {};
}
}  // namespace rs
