#include "V8Context.h"

#include <src/debug/debug-interface.h>

#include "V8Isolate.h"
#include "src/interface/local_dom_window.h"

namespace rs {
V8Context::V8Context(V8Isolate* v8Isolate) : m_v8Isolate(v8Isolate) {
  m_context =
      v8::Global<v8::Context>(GetIsolate(), v8::Context::New(GetIsolate()));
}

V8Context::~V8Context() = default;

v8::Local<v8::Value> V8Context::RunCode(const char* code) {
  v8::Isolate* isolate = GetIsolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = m_context.Get(isolate);
  v8::Context::Scope context_scope(context);
  v8::TryCatch try_catch(isolate);

  v8::Local<v8::Script> script;
  v8::Local<v8::Value> result;
  v8::Local<v8::String> js_code =
      v8::String::NewFromUtf8(isolate, code).ToLocalChecked();
  if (v8::Script::Compile(context, js_code, nullptr).ToLocal(&script)) {
    v8::MaybeLocal<v8::Value> maybe_result = script->Run(context);
    if (maybe_result.ToLocal(&result)) {
      return result;
    }
  }
  if (try_catch.HasCaught()) {
    result = try_catch.Exception();
    try_catch.Reset();
  }
  return result;
}

v8::Isolate* V8Context::GetIsolate() const {
  return m_v8Isolate->GetIsolate();
}

v8::Local<v8::Context> V8Context::GetContext() const {
  return m_context.Get(GetIsolate());
}
void V8Context::SetContext(v8::Local<v8::ObjectTemplate> global_template) {
  m_context = v8::Global<v8::Context>(
      GetIsolate(),
      v8::Context::New(GetIsolate(), nullptr, global_template, {}));
  LocalDOMWindow* window = MakeCppGcObject<GC::kCurrent, LocalDOMWindow>();
  ScriptWrappable::Wrap(m_context.Get(m_v8Isolate->GetIsolate())->Global(),
                        window);
}

void V8Context::Release() const {
  GetContext().Clear();
}

}  // namespace rs
