MAIN_PATH = $$PWD/main/

QT += widgets concurrent

CONFIG += c++17 qt

SOURCES += \
        $$MAIN_PATH/Colle.cpp \
        $$MAIN_PATH/ExcelExporter.cpp \
        $$MAIN_PATH/Group.cpp \
        $$MAIN_PATH/MainWindow.cpp \
        $$MAIN_PATH/Options.cpp \
        $$MAIN_PATH/OptionsTab.cpp \
        $$MAIN_PATH/OptionsVariations.cpp \
        $$MAIN_PATH/Slot.cpp \
        $$MAIN_PATH/Solver.cpp \
        $$MAIN_PATH/Subject.cpp \
        $$MAIN_PATH/Subjects.cpp \
        $$MAIN_PATH/SubjectsTab.cpp \
        $$MAIN_PATH/Teacher.cpp \
        $$MAIN_PATH/TeacherDialog.cpp \
        $$MAIN_PATH/Teachers.cpp \
        $$MAIN_PATH/TeachersTab.cpp \
        $$MAIN_PATH/Timeslot.cpp \
        $$MAIN_PATH/Week.cpp \

HEADERS += \
    $$MAIN_PATH/Colle.h \
    $$MAIN_PATH/ExcelExporter.h \
    $$MAIN_PATH/Group.h \
    $$MAIN_PATH/MainWindow.h \
    $$MAIN_PATH/Options.h \
    $$MAIN_PATH/OptionsTab.h \
    $$MAIN_PATH/OptionsVariations.h \
    $$MAIN_PATH/Slot.h \
    $$MAIN_PATH/Solver.h \
    $$MAIN_PATH/Subject.h \
    $$MAIN_PATH/Subjects.h \
    $$MAIN_PATH/SubjectsTab.h \
    $$MAIN_PATH/Teacher.h \
    $$MAIN_PATH/TeacherDialog.h \
    $$MAIN_PATH/Teachers.h \
    $$MAIN_PATH/TeachersTab.h \
    $$MAIN_PATH/Timeslot.h \
    $$MAIN_PATH/Week.h

FORMS += \
    $$MAIN_PATH/MainWindow.ui \
    $$MAIN_PATH/OptionsTab.ui \
    $$MAIN_PATH/SubjectsTab.ui \
    $$MAIN_PATH/TeacherDialog.ui \
    $$MAIN_PATH/TeachersTab.ui

INCLUDEPATH += \
    $$MAIN_PATH

ORTOOLS = $$PWD/../../../../../Qt/Libraries/or-tools

win32:CONFIG(release, debug|release): LIBS += -L$$ORTOOLS/lib/ -lortools
else:win32:CONFIG(debug, debug|release): LIBS += -L$$ORTOOLS/lib/ -lortoolsd

INCLUDEPATH += $$ORTOOLS/include
DEPENDPATH += $$ORTOOLS/include

XLNT = $$PWD/../../../../../Qt/Libraries/xlnt

win32:CONFIG(release, debug|release): LIBS += -L$$XLNT/lib/ -lxlnt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$XLNT/lib/ -lxlntd

INCLUDEPATH += $$XLNT/include
DEPENDPATH += $$XLNT/include
