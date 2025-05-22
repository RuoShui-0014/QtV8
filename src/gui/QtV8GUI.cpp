//
// Created by Administrator on 24-10-12.
//
#include "src/gui/QtV8GUI.h"

#include "src/gui/QtV8Thread.h"
#include "ui_QtV8GUI.h"
#include <thread>

namespace rs {
QtV8GUI::QtV8GUI(QWidget* parent) : QWidget(parent), ui(new Ui::QtV8GUI) {
  ui->setupUi(this);

  connect(ui->toolButton_run, &QToolButton::clicked, this,
          &QtV8GUI::SlotRunCode);
  connect(ui->toolButton_clean, &QToolButton::clicked, this,
          &QtV8GUI::SlotClean);
  connect(ui->toolButton_debug, &QToolButton::clicked, this,
          &QtV8GUI::SlotDebug);

  m_thread = std::make_unique<QtV8Thread>(this);
  m_thread->start();
}

QtV8GUI::~QtV8GUI() {
  m_thread->QuitRunner();
  m_thread->wait();

  disconnect(ui->toolButton_run, &QToolButton::clicked, this,
             &QtV8GUI::SlotRunCode);
  disconnect(ui->toolButton_clean, &QToolButton::clicked, this,
             &QtV8GUI::SlotClean);

  delete ui;
}

Ui::QtV8GUI * QtV8GUI::GetUI() {
  return ui;
}

void QtV8GUI::SlotRunCode() {
  QString code = ui->textEdit_code->toPlainText();
  emit SignalRunCode(code);
}

void QtV8GUI::SlotClean() {
  ui->textBrowser_log->clear();
}

void QtV8GUI::SlotDebug() {
  this->m_thread->EnableDebug();
}

void QtV8GUI::SlotShow(const QString& log) {
  ui->textBrowser_log->append(">" + log);
}
} // namespace rs