cmake_minimum_required(VERSION 3.21)

project(KholGen VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Concurrent Core WebChannel WebSockets LinguistTools)
qt_standard_project_setup(I18N_SOURCE_LANGUAGE fr)
qt_add_translations(${PROJECT_NAME} NO_GENERATE_PLURALS_TS_FILE TS_FILE_DIR translations)

add_executable(${PROJECT_NAME}
    main.cpp
    misc.cpp
    misc.h

    Colle.cpp
    Colle.h
    Communication.cpp
    Communication.h
    Group.cpp
    Group.h
    Slot.cpp
    Slot.h
    Solver.cpp
    Solver.h
    State.cpp
    State.h
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

    Objective/EvenDistributionBetweenTeachersObjective.cpp
    Objective/EvenDistributionBetweenTeachersObjective.h
    Objective/MinimalNumberOfSlotsObjective.cpp
    Objective/MinimalNumberOfSlotsObjective.h
    Objective/NoConsecutiveCollesObjective.cpp
    Objective/NoConsecutiveCollesObjective.h
    Objective/Objective.cpp
    Objective/Objective.h
    Objective/ObjectiveComputation.cpp
    Objective/ObjectiveComputation.h
    Objective/OnlyOneCollePerDayObjective.cpp
    Objective/OnlyOneCollePerDayObjective.h
    Objective/SameSlotOnlyOnceInCycleObjective.cpp
    Objective/SameSlotOnlyOnceInCycleObjective.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Concurrent Qt::Core Qt::WebChannel Qt::WebSockets)

include(FetchContent)
set(ABSL_PROPAGATE_CXX_STD ON)
set(BUILD_DEPS ON)
set(BUILD_SAMPLES OFF)
set(BUILD_EXAMPLES OFF)
FetchContent_Declare(
    or-tools
    GIT_REPOSITORY https://github.com/google/or-tools.git
    GIT_TAG        ed8db9097cdbaa06b40384d3a86164744a606043 # v9.7
)
FetchContent_MakeAvailable(or-tools)
target_link_libraries(${PROJECT_NAME} PRIVATE ortools::ortools)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    target_compile_options(${PROJECT_NAME} PRIVATE /experimental:external /external:anglebrackets /external:W0)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

set(PACKAGE_DIR "${CMAKE_BINARY_DIR}/package/")
file(READ "${or-tools_SOURCE_DIR}/LICENSE" OR-TOOLS_LICENSE)
file(WRITE "${PACKAGE_DIR}/3rdpartylicenses.txt" "Package: or-tools\n${OR-TOOLS_LICENSE}")

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E create_hardlink "${CMAKE_BINARY_DIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}" "${PACKAGE_DIR}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> ${PACKAGE_DIR}
    COMMAND_EXPAND_LISTS
)
