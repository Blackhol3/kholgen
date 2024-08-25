cmake_minimum_required(VERSION 3.28)

project(KholGen VERSION 2.0.0.0 LANGUAGES CXX)
set(PROJECT_HUMAN_NAME KhôlGen)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Concurrent Core HttpServer WebChannel WebSockets LinguistTools)
qt_standard_project_setup(I18N_SOURCE_LANGUAGE fr)
qt_add_translations(${PROJECT_NAME} NO_GENERATE_PLURALS_TS_FILE TS_FILE_DIR translations)

configure_file(resources.rc ./)
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

	${CMAKE_BINARY_DIR}/resources.rc
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Concurrent Qt::Core Qt::HttpServer Qt::WebChannel Qt::WebSockets)

include(FetchContent)
set(ABSL_PROPAGATE_CXX_STD ON)
set(BUILD_DEPS ON)
set(BUILD_SAMPLES OFF)
set(BUILD_EXAMPLES OFF)
FetchContent_Declare(
    or-tools
    GIT_REPOSITORY https://github.com/google/or-tools.git
    GIT_TAG        ed8db9097cdbaa06b40384d3a86164744a606043 # v9.7
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(or-tools)
target_link_libraries(${PROJECT_NAME} PRIVATE ortools::ortools)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE "PROJECT_NAME=\"${PROJECT_NAME}\""
	PRIVATE "PROJECT_HUMAN_NAME=\"${PROJECT_HUMAN_NAME}\""
	PRIVATE "PROJECT_VERSION=\"${PROJECT_VERSION}\""
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    target_compile_options(${PROJECT_NAME} PRIVATE /experimental:external /external:anglebrackets /external:W0)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

include(CMakeInstall.txt)
