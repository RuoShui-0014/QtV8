//
// Created by Administrator on 24-10-12.
//
#pragma once

#include <QWidget>

namespace rs {
QT_BEGIN_NAMESPACE

namespace Ui {
class QtV8GUI;
}

QT_END_NAMESPACE

class QtV8Thread;

class QtV8GUI : public QWidget {
  Q_OBJECT

public:
  explicit QtV8GUI(QWidget* parent = nullptr);
  ~QtV8GUI() override;

  Ui::QtV8GUI* GetUI();
signals:
  void SignalRunCode(const QString& code);

public slots:
  void SlotRunCode();
  void SlotClean();
  void SlotDebug();
  void SlotShow(const QString& log);

private:
  Ui::QtV8GUI* ui{nullptr};
  std::unique_ptr<QtV8Thread> m_thread;
};
} // namespace rs