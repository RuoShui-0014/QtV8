#include "src/gui/QtV8Runner.h"

#include <ui_QtV8GUI.h>
#include "src/v8/V8Initializator.h"
#include "include/libplatform/libplatform.h"
#include "interface/Global.h"
#include "src/debug/debug-interface.h"
#include "src/gui/QtV8ConsoleDelegate.h"
#include "src/gui/QtV8GUI.h"

static int index = 1;

namespace rs {
ScriptTask::ScriptTask(v8::Isolate* isolate,
                       v8::Local<v8::Context> context,
                       QString& code)
    : m_isolate(isolate), m_context(isolate, context), m_code(code) {}

ScriptTask::~ScriptTask() = default;

void ScriptTask::Run() {
  v8::HandleScope handle_scope(m_isolate);
  v8::Local<v8::Context> context = m_context.Get(m_isolate);
  v8::Context::Scope context_scope(context);
  v8::TryCatch try_catch(m_isolate);

  v8::Local<v8::Script> script;
  v8::Local<v8::Value> result;
  QByteArray utf8Data = m_code.toUtf8();
  v8::Local<v8::String> js_code =
      v8::String::NewFromUtf8(m_isolate, utf8Data.constData()).ToLocalChecked();
  v8::ScriptOrigin scriptOrigin = v8::ScriptOrigin(
      v8::String::NewFromUtf8(
          m_isolate,
          (QString("QtV8_") + QString::number(index++) + ".js").toUtf8(),
          v8::NewStringType::kNormal)
          .ToLocalChecked());
  if (v8::Script::Compile(context, js_code, &scriptOrigin).ToLocal(&script)) {
    v8::MaybeLocal<v8::Value> maybe_result = script->Run(context);
    if (maybe_result.ToLocal(&result)) {
    }
  }
  if (try_catch.HasCaught()) {
    result = try_catch.Exception();
    try_catch.Reset();
  }
}

QtV8Runner::QtV8Runner(QtV8GUI* gui) : m_gui{gui} {
  connect(this, &QtV8Runner::SignalShow, m_gui, &QtV8GUI::SlotShow,
          Qt::QueuedConnection);
  connect(m_gui, &QtV8GUI::SignalRunCode, this, &QtV8Runner::SlotRunCode,
          Qt::QueuedConnection);
}

QtV8Runner::~QtV8Runner() {
  disconnect(this, &QtV8Runner::SignalShow, m_gui, &QtV8GUI::SlotShow);
  disconnect(m_gui, &QtV8GUI::SignalRunCode, this, &QtV8Runner::SlotRunCode);
}

void QtV8Runner::Init() {
  V8Initializer::SetV8Flag("--expose-gc");
  m_v8isolate = std::make_unique<V8Isolate>();

  v8::Isolate* isolate = m_v8isolate->GetIsolate();
  v8::Local<v8::Template> interface = binding::Global::GetInterface(isolate);
  binding::Global::InstallInterface(isolate, interface);
  m_v8isolate->SetDefaultContext(
      interface.As<v8::FunctionTemplate>()->InstanceTemplate());

  m_consoleDelegate = std::make_unique<QtV8ConsoleDelegate>(this);
  v8::debug::SetConsoleDelegate(m_v8isolate->GetIsolate(),
                                m_consoleDelegate.get());
}

void QtV8Runner::End() {
  v8::debug::SetConsoleDelegate(m_v8isolate->GetIsolate(), nullptr);
  m_consoleDelegate.reset();
  m_v8isolate.reset();
}

void QtV8Runner::RunLoop() {
  v8::Isolate* isolate = m_v8isolate->GetIsolate();
  v8::HandleScope handle_scope(isolate);
  v8::Context::Scope context_scope(
      m_v8isolate->GetDefaultV8Context()->GetContext());

  while (!m_quit) {
    QCoreApplication::processEvents();
    if (m_debug) {
      m_inspectorClient->dispatchProtocolMessage();
    }
    for (bool success = true; success;) {
      success = v8::platform::PumpMessageLoop(
          V8Initializer::GetInstance().GetPlatform(), isolate);
    }
    if (isolate->HasPendingBackgroundTasks()) {
      continue;
    }
    {
      std::unique_lock lock(m_mutex);
      m_runCondition.wait_for(lock, std::chrono::milliseconds(10));
    }
  }
}

void QtV8Runner::Quit() {
  m_quit = true;
  m_runCondition.notify_all();
}

void QtV8Runner::PostTask(std::unique_ptr<v8::Task> task) {
  std::shared_ptr<v8::TaskRunner> task_runner =
      V8Initializer::GetInstance().GetTaskRunner(m_v8isolate->GetIsolate());
  task_runner->PostTask(std::move(task));
  m_runCondition.notify_all();
}

void QtV8Runner::EnableDebug() {
  m_inspectorClient.reset(new V8InspectorClient(m_v8isolate->GetIsolate(),
                                                m_mutex, m_runCondition));
  m_inspectorClient->setDefaultContext(
      m_v8isolate->GetDefaultV8Context()->GetContext());
  m_debug = true;
  m_runCondition.notify_all();
  std::thread t([this]() {
    this->m_inspectorClient->connectSession();
    this->m_inspectorClient->createInspector();
    this->m_inspectorClient->RunLoop();
  });
  t.detach();
}

V8Isolate* QtV8Runner::GetV8Isolate() {
  return m_v8isolate.get();
}

void QtV8Runner::SlotRunCode(const QString& code) {
  auto task = std::make_unique<ScriptTask>(
      m_v8isolate->GetIsolate(),
      m_v8isolate->GetDefaultV8Context()->GetContext(),
      const_cast<QString&>(code));
  PostTask(std::move(task));
}

QtV8Runner::ScopeRunner::ScopeRunner(QtV8Runner* runner) : m_runner(runner) {
  m_runner->Init();
}

QtV8Runner::ScopeRunner::~ScopeRunner() {
  m_runner->End();
}

void QtV8Runner::ScopeRunner::Run() {
  m_runner->RunLoop();
}
}  // namespace rs
