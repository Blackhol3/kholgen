cmake_minimum_required(VERSION 3.15)

project(KholGen VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_AUTORCC ON)
find_package(Qt6 REQUIRED COMPONENTS Concurrent Core Gui WebChannel WebSockets LinguistTools)
qt_standard_project_setup()

set(TS_FILES KholGen_fr_FR.ts)

add_executable(${PROJECT_NAME}
    main.cpp
    misc.cpp
    misc.h

    Colle.cpp
    Colle.h
    Communication.cpp
    Communication.h
    CsvExporter.cpp
    CsvExporter.h
    ExcelExporter.cpp
    ExcelExporter.h
    Exporter.cpp
    Exporter.h
    JsonImporter.cpp
    JsonImporter.h
    Slot.cpp
    Slot.h
    Solver.cpp
    Solver.h
    Subject.cpp
    Subject.h
    Teacher.cpp
    Teacher.h
    Timeslot.cpp
    Timeslot.h
    Trio.cpp
    Trio.h
    WebSocketTransport.cpp
    WebSocketTransport.h
    Week.cpp
    Week.h
    ${TS_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Concurrent Qt::Core Qt::Gui Qt::WebChannel Qt::WebSockets)
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

#[[
include(FetchContent)
set(BUILD_DEPS ON)
set(BUILD_SAMPLES OFF)
set(BUILD_EXAMPLES OFF)
FetchContent_Declare(
    or-tools
    GIT_REPOSITORY https://github.com/google/or-tools.git
    GIT_TAG        49b6301e1e1e231d654d79b6032e79809868a70e # v9.3
)
FetchContent_Declare(
    xlnt
    GIT_REPOSITORY https://github.com/tfussell/xlnt.git
    GIT_TAG        d88c901faa539f9272a81ba0bab72def70ca18d7 # master
)
FetchContent_MakeAvailable(or-tools xlnt)
target_link_libraries(${PROJECT_NAME} PRIVATE or-tools xlnt)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${ortools_SOURCE_DIR})
]]

find_library(ORTOOLS_PATH ortools HINTS C:/Qt/Libraries/or-tools/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${ORTOOLS_PATH})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE C:/Qt/Libraries/or-tools)

include(FetchContent)
FetchContent_Declare(
    xlnt
    GIT_REPOSITORY https://github.com/tfussell/xlnt.git
    GIT_TAG        d88c901faa539f9272a81ba0bab72def70ca18d7 # master
)
FetchContent_MakeAvailable(xlnt)
target_link_libraries(${PROJECT_NAME} PRIVATE xlnt)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    target_compile_options(${PROJECT_NAME} PRIVATE /experimental:external /external:anglebrackets /external:W0)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
