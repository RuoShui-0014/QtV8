//
// Created by Administrator on 24-10-12.
//
#pragma once

#include <QThread>

namespace rs {

class QtV8GUI;
class QtV8Runner;

class QtV8Thread : public QThread {
  Q_OBJECT

public:
  explicit QtV8Thread(QtV8GUI* gui = nullptr);
  ~QtV8Thread() override;

  void run() override;

  void QuitRunner();

  void EnableDebug();

  QtV8Runner* GetRunner();

private:
  QtV8GUI* m_gui{};
  std::unique_ptr<QtV8Runner> m_runner;
};
} // namespace rs