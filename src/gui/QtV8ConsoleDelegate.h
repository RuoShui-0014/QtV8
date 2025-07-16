//
// Created by Administrator on 24-10-13.
//
#pragma once

#include "src/debug/interface-types.h"
#include "src/gui/QtV8Runner.h"
#include "src/v8/V8Isolate.h"

namespace rs {

class QtV8ConsoleDelegate : public v8::debug::ConsoleDelegate {
public:
  explicit QtV8ConsoleDelegate(QtV8Runner* runner) : m_runner(runner) {
  }

  void Assert(const v8::debug::ConsoleCallArguments& args,
              const v8::debug::ConsoleContext&) override {
  }

  void Log(const v8::debug::ConsoleCallArguments& args,
           const v8::debug::ConsoleContext&) override {
    QString log;
    bool isFirst{true};
    v8::Isolate* isolate = m_runner->GetV8Isolate()->GetIsolate();
    for (int i = 0; i < args.Length(); i++) {
      v8::HandleScope handle_scope(isolate);

      v8::Local<v8::Value> arg = args[i];
      v8::Local<v8::String> str_obj;

      if (arg->IsSymbol())
        arg = v8::Local<v8::Symbol>::Cast(arg)->Description(isolate);
      if (!arg->ToString(isolate->GetCurrentContext()).ToLocal(&str_obj))
        return;

      v8::String::Utf8Value str(isolate, str_obj);
      if (isFirst) {
        log = log + *str;
        isFirst = false;
      } else {
        log = log + " " + *str;
      }
    }
    emit m_runner->SignalShow(log);
  }

  void Error(const v8::debug::ConsoleCallArguments& args,
             const v8::debug::ConsoleContext&) override {
  }

  void Warn(const v8::debug::ConsoleCallArguments& args,
            const v8::debug::ConsoleContext&) override {
  }

  void Info(const v8::debug::ConsoleCallArguments& args,
            const v8::debug::ConsoleContext&) override {
  }

  void Debug(const v8::debug::ConsoleCallArguments& args,
             const v8::debug::ConsoleContext&) override {
  }

  void Profile(const v8::debug::ConsoleCallArguments& args,
               const v8::debug::ConsoleContext& context) override {
  }

  void ProfileEnd(const v8::debug::ConsoleCallArguments& args,
                  const v8::debug::ConsoleContext& context) override {
  }

  void Time(const v8::debug::ConsoleCallArguments& args,
            const v8::debug::ConsoleContext&) override {
  }

  void TimeLog(const v8::debug::ConsoleCallArguments& args,
               const v8::debug::ConsoleContext&) override {
  }

  void TimeEnd(const v8::debug::ConsoleCallArguments& args,
               const v8::debug::ConsoleContext&) override {
  }

  void TimeStamp(const v8::debug::ConsoleCallArguments& args,
                 const v8::debug::ConsoleContext&) override {
  }

  void Trace(const v8::debug::ConsoleCallArguments& args,
             const v8::debug::ConsoleContext&) override {
  }

private:
  QtV8Runner* m_runner{nullptr};
};
} // namespace rs