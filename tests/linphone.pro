QT += core gui quick widgets

TARGET = linphone
TEMPLATE = app

SOURCES += \
  src/app.cpp \
  src/main.cpp \
  src/views/main_window.cpp

HEADERS += \
  src/app.hpp \
  src/views/main_window.hpp

TRANSLATIONS = \
  languages/en.ts \
  languages/fr.ts

lupdate_only{
  SOURCES = \
    ui/*.qml
}

RESOURCES += \
  resources.qrc