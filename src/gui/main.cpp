#include <QApplication>
#include "src/gui/QtV8GUI.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    rs::QtV8GUI widget;
    widget.show();

    return QApplication::exec();
}
