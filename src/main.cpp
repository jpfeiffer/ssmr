#include "MainWindow.h"

#include <QApplication>

#include <QUrl>
#include <QMessageBox>

int main(int argc, char *argv[])
{
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
  QApplication a(argc, argv);

  QCoreApplication::setOrganizationName("Ssmr");
  QCoreApplication::setApplicationName("ssmr");

  //for debug builds we want easy application closing
  #ifndef QT_DEBUG
  //we want to allow the application run with a tray icon active
  a.setQuitOnLastWindowClosed(false);
  #endif

  Ssmr::MainWindow mw;
  mw.show();

  return a.exec();
}
