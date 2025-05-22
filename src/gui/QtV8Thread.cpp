#include "src/gui/QtV8Thread.h"
#include "src/gui/QtV8GUI.h"

namespace rs {
QtV8Thread::QtV8Thread(QtV8GUI* gui) : QThread(gui), m_gui(gui), m_runner(gui) {
  this->setObjectName("V8EngineThread");

  m_runner.moveToThread(this);
}

QtV8Thread::~QtV8Thread() {
  m_runner.moveToThread(QThread::currentThread());
}

void QtV8Thread::run() {
  QtV8Runner::ScopeRunner runner(&m_runner);
  runner.Run();
}

void QtV8Thread::QuitRunner() {
  m_runner.Quit();
}

void QtV8Thread::EnableDebug() {
  m_runner.EnableDebug();
}
} // namespace rs