//
// Created by Administrator on 24-10-12.
//
#pragma once

#include <QThread>

#include "src/gui/QtV8Runner.h"

namespace rs {
class QtV8GUI;

class QtV8Thread : public QThread {
  Q_OBJECT

public:
  explicit QtV8Thread(QtV8GUI* gui = nullptr);
  ~QtV8Thread() override;

  void run() override;

  void QuitRunner();

  void EnableDebug();

  QtV8Runner* GetRunner() { return &m_runner; }

private:
  QtV8GUI* m_gui{};
  QtV8Runner m_runner;
};
} // namespace rs