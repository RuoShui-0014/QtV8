//
// Created by Administrator on 25-1-30.
//
#pragma once

#include <v8.h>

namespace rs {
class V8Isolate;

class V8Context {
public:
  explicit V8Context(V8Isolate* m_v8Isolate);
  ~V8Context();

  [[nodiscard]] v8::Isolate* GetIsolate() const;
  [[nodiscard]] v8::Local<v8::Context> GetContext() const;
  void SetContext(v8::Local<v8::ObjectTemplate> global_template);

  [[nodiscard]] char* QtRunCode(const char* code);
  [[nodiscard]] v8::Local<v8::Value> RunCode(const char* code);

  void Release() const;

private:
  V8Isolate* m_v8Isolate;
  v8::Global<v8::Context> m_context;
};
} // namespace rs
