#***********************************************************************************************************************
# Set this configuration option to minimize the application into the systray automatically
#***********************************************************************************************************************
CONFIG *= ENABLE_SYSTRAY_MODULE

QT += core gui multimedia svg serialport charts

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ssmr
APPLICATION_NAME = Simple smart meter reader

VERSION_MAJOR_NUMBER = 0
VERSION_MINOR_NUMBER = 1
VERSION_PATCHLEVEL_NUMBER = 0
PRODUCT_NAME = Simple smart meter reader
AUTHOR_NAME = Johannes Pfeiffer
PROJECT_ROOT = $$PWD

TEMPLATE = app

DESTDIR = $${PROJECT_ROOT}/bin

#***********************************************************************************************************************
# Manually specify that we have our own manifest file
#***********************************************************************************************************************
windows {
	CONFIG -= embed_manifest_exe
}

DEFINES *= \
	DEFAULT_COM_PORT=\\\"COM2\\\" \
	DEFAULT_TIMEOUT_SEC=10

#DEFINES += SHOW_HTTPS_OPTION

INCLUDEPATH *= \
	$${PROJECT_ROOT}/src \
	$${PROJECT_ROOT}/dependencies/include

VPATH = $${INCLUDEPATH}

# We need to tell the compiler where the common source code can be found
LIBS *= \
	-L$${DESTDIR} \
	-lsml \
	-lqtcsv \
	-luuid

SOURCES += \
	main.cpp \
	src/Connection.cpp \
	src/ConnectionDialog.cpp \
	src/ConnectionSerializer.cpp \
	src/ConnectionWindow.cpp \
	src/HelpFunctions.cpp \
	src/ObisValueDiagramWidget.cpp \
	src/ObisValueLogWidget.cpp \
	src/ObisValueMappingWidget.cpp \
	src/ObisValueWidget.cpp \
	src/TrayElementController.cpp \
	src/MainWindow.cpp

HEADERS += \
	src/Connection.h \
	src/ConnectionDialog.h \
	src/ConnectionSerializer.h \
	src/ConnectionWindow.h \
	src/HelpFunctions.h \
	src/ObisValueDiagramWidget.h \
	src/ObisValueLogWidget.h \
	src/ObisValueMappingWidget.h \
	src/ObisValueWidget.h \
	src/TrayElementController.h \
	src/MainWindow.h \
	src/TypeDefinitions.h

FORMS += \
	src/ConnectionDialog.ui \
	src/ConnectionWindow.ui \
	src/MainWindow.ui \
	src/ObisValueDiagramWidget.ui \
	src/ObisValueLogWidget.ui \
	src/ObisValueMappingWidget.ui \
	src/ObisValueWidget.ui

DISTFILES +=

RESOURCES += \
	ssmr.qrc
