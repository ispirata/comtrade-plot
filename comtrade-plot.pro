TEMPLATE = app
TARGET = comtrade-plot

CONFIG += c++11

QT += widgets printsupport

INCLUDEPATH += vendor

# Input
SOURCES += src/main.cpp \
           src/ComtradeAnalogChannel.cpp \
           src/ComtradeChannel.cpp \
           src/ComtradeDigitalChannel.cpp \
           src/ComtradeRecord.cpp \
           src/PlotWindow.cpp \
           vendor/qcustomplot.cpp

HEADERS += src/ComtradeAnalogChannel.h \
           src/ComtradeChannel.h \
           src/ComtradeDigitalChannel.h \
           src/ComtradeRecord.h \
           src/PlotWindow.h \
           vendor/qcustomplot.h

FORMS += \
    ui/plotwindow.ui

isEmpty(PREFIX) {
    PREFIX = /usr/local
}

unix:target.path = $$PREFIX/bin
unix:INSTALLS += target
