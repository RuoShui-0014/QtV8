//
// Created by ruoshui on 25-7-8.
//

#pragma once

#include "../module/script_wrappable.h"
#include "../module/wrapper_type_info.h"

namespace rs {

class LocalDOMWindow : public ScriptWrappable {
 public:
  LocalDOMWindow();
  ~LocalDOMWindow() override;

  LocalDOMWindow* window() { return this; }

  void Trace(cppgc::Visitor* visitor) const override;
};

class V8Window {
 public:
  static constexpr const WrapperTypeInfo* GetWrapperTypeInfo() {
    return &wrapper_type_info_;
  }

  static void InstallInterfaceTemplate(
      v8::Isolate* isolate,
      v8::Local<v8::Template> interface_template);

 private:
  friend LocalDOMWindow;
  static const WrapperTypeInfo wrapper_type_info_;
};

}  // namespace svm
