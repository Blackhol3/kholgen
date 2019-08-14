COMMON_PATH = $$PWD/common/

QT += widgets concurrent

CONFIG += c++17 qt

SOURCES += \
        $$COMMON_PATH/Colle.cpp \
        $$COMMON_PATH/ComputationTab.cpp \
        $$COMMON_PATH/ExcelExporter.cpp \
        $$COMMON_PATH/Group.cpp \
        $$COMMON_PATH/Groups.cpp \
        $$COMMON_PATH/GroupsTab.cpp \
        $$COMMON_PATH/MainWindow.cpp \
        $$COMMON_PATH/misc.cpp \
        $$COMMON_PATH/Option.cpp \
        $$COMMON_PATH/Options.cpp \
        $$COMMON_PATH/OptionsTab.cpp \
        $$COMMON_PATH/OptionsVariations.cpp \
        $$COMMON_PATH/Slot.cpp \
        $$COMMON_PATH/Solver.cpp \
        $$COMMON_PATH/Subject.cpp \
        $$COMMON_PATH/Subjects.cpp \
        $$COMMON_PATH/SubjectsTab.cpp \
        $$COMMON_PATH/Tab.cpp \
        $$COMMON_PATH/Teacher.cpp \
        $$COMMON_PATH/TeacherDialog.cpp \
        $$COMMON_PATH/Teachers.cpp \
        $$COMMON_PATH/TeachersTab.cpp \
        $$COMMON_PATH/Timeslot.cpp \
        $$COMMON_PATH/UndoCommand.cpp \
        $$COMMON_PATH/Week.cpp

HEADERS += \
    $$COMMON_PATH/Colle.h \
    $$COMMON_PATH/ComputationTab.h \
    $$COMMON_PATH/ExcelExporter.h \
    $$COMMON_PATH/Group.h \
    $$COMMON_PATH/Groups.h \
    $$COMMON_PATH/GroupsTab.h \
    $$COMMON_PATH/MainWindow.h \
    $$COMMON_PATH/misc.h \
    $$COMMON_PATH/Option.h \
    $$COMMON_PATH/Options.h \
    $$COMMON_PATH/OptionsTab.h \
    $$COMMON_PATH/OptionsVariations.h \
    $$COMMON_PATH/Slot.h \
    $$COMMON_PATH/Solver.h \
    $$COMMON_PATH/Subject.h \
    $$COMMON_PATH/Subjects.h \
    $$COMMON_PATH/SubjectsTab.h \
    $$COMMON_PATH/Tab.h \
    $$COMMON_PATH/Teacher.h \
    $$COMMON_PATH/TeacherDialog.h \
    $$COMMON_PATH/Teachers.h \
    $$COMMON_PATH/TeachersTab.h \
    $$COMMON_PATH/Timeslot.h \
    $$COMMON_PATH/UndoCommand.h \
    $$COMMON_PATH/Week.h

FORMS += \
    $$COMMON_PATH/ComputationTab.ui \
    $$COMMON_PATH/GroupsTab.ui \
    $$COMMON_PATH/MainWindow.ui \
    $$COMMON_PATH/OptionsTab.ui \
    $$COMMON_PATH/SubjectsTab.ui \
    $$COMMON_PATH/TeacherDialog.ui \
    $$COMMON_PATH/TeachersTab.ui

INCLUDEPATH += \
    $$COMMON_PATH

ORTOOLS = C:/Qt/Libraries/or-tools

win32:CONFIG(release, debug|release): LIBS += -L$$ORTOOLS/lib/ -lortools
else:win32:CONFIG(debug, debug|release): LIBS += -L$$ORTOOLS/lib/ -lortoolsd

INCLUDEPATH += $$ORTOOLS/include
DEPENDPATH += $$ORTOOLS/include

XLNT = C:/Qt/Libraries/xlnt

win32:CONFIG(release, debug|release): LIBS += -L$$XLNT/lib/ -lxlnt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$XLNT/lib/ -lxlntd

INCLUDEPATH += $$XLNT/include
DEPENDPATH += $$XLNT/include
