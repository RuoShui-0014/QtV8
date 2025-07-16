#pragma once

#include <v8-platform.h>

#include <QObject>
#include <atomic>
#include <condition_variable>
#include "inspector/V8InspectorClient.h"

namespace rs {

class V8Isolate;
class QtV8GUI;
class QtV8ConsoleDelegate;
class V8InspectorClient;

class ScriptTask : public v8::Task {
 public:
  ScriptTask(v8::Isolate* isolate,
             v8::Local<v8::Context> context,
             QString& code);
  ~ScriptTask() override;

  void Run() override;

 private:
  v8::Isolate* m_isolate;
  v8::Global<v8::Context> m_context;
  QString m_code;
};

class QtV8Runner : public QObject {
  Q_OBJECT
 public:
  explicit QtV8Runner(QtV8GUI* gui);
  ~QtV8Runner() override;

  [[nodiscard]] V8Isolate* GetV8Isolate();

  void Init();
  void RunLoop();
  void End();
  void Quit();

  void PostTask(std::unique_ptr<v8::Task> task);
  void EnableDebug();

 signals:
  void SignalShow(const QString& log);

 public slots:
  void SlotRunCode(const QString& code);

 public:
  class ScopeRunner {
   public:
    explicit ScopeRunner(QtV8Runner* runner);
    ~ScopeRunner();

    void Run();

   private:
    QtV8Runner* m_runner;
  };

 private:
  QtV8GUI* m_gui{};
  std::unique_ptr<V8Isolate> m_v8isolate;
  std::unique_ptr<QtV8ConsoleDelegate> m_consoleDelegate;
  std::unique_ptr<V8InspectorClient> m_inspectorClient;

  std::atomic<bool> m_quit{false};
  std::atomic<bool> m_debug{false};
  std::mutex m_mutex;
  std::condition_variable m_runCondition;
};
}  // namespace rs
