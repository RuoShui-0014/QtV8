#pragma once

#include <include/v8.h>

namespace rs {

/* init and end v8 engine */
class V8Initializer {
 public:
  static V8Initializer& GetInstance();
  ~V8Initializer();

  void Init();
  static void SetV8Flag(const char* flag);

  [[nodiscard]] bool Inited() const;

  [[nodiscard]] v8::Platform* GetPlatform() const;

  std::shared_ptr<v8::TaskRunner> GetTaskRunner(v8::Isolate* isolate);

 private:
  V8Initializer();

  bool m_inited{false};
  std::unique_ptr<v8::Platform> m_platform;
};

}  // namespace rs
