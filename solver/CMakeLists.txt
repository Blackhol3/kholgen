cmake_minimum_required(VERSION 3.28)

project(KholGen VERSION 2.0.0.0 LANGUAGES CXX)
set(PROJECT_HUMAN_NAME KhôlGen)
set(PROJECT_LIB_NAME "${PROJECT_NAME}Lib")
set(PROJECT_TESTS_NAME "${PROJECT_NAME}Tests")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Concurrent Core HttpServer WebChannel WebSockets LinguistTools)
qt_standard_project_setup(I18N_SOURCE_LANGUAGE fr)
qt_add_translations(${PROJECT_NAME} NO_GENERATE_PLURALS_TS_FILE TS_FILE_DIR translations)

add_library(${PROJECT_LIB_NAME}
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

configure_file(resources.rc ./)
add_executable(${PROJECT_NAME}
	main.cpp
	${CMAKE_BINARY_DIR}/resources.rc
)

add_executable(${PROJECT_TESTS_NAME}
)

target_link_libraries(${PROJECT_LIB_NAME} PUBLIC Qt::Concurrent Qt::Core Qt::HttpServer Qt::WebChannel Qt::WebSockets)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIB_NAME})
target_link_libraries(${PROJECT_TESTS_NAME} PRIVATE ${PROJECT_LIB_NAME})

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
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        4e8d92bf02f7d1c8006a0e7a5ecabd8e62d98502 # v3.6.0
)
FetchContent_MakeAvailable(or-tools catch2)
target_link_libraries(${PROJECT_LIB_NAME} PRIVATE ortools::ortools)
target_link_libraries(${PROJECT_TESTS_NAME} PRIVATE Catch2::Catch2WithMain)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE "PROJECT_NAME=\"${PROJECT_NAME}\""
	PRIVATE "PROJECT_HUMAN_NAME=\"${PROJECT_HUMAN_NAME}\""
	PRIVATE "PROJECT_VERSION=\"${PROJECT_VERSION}\""
)

if(MSVC)
	target_compile_options(${PROJECT_LIB_NAME} INTERFACE /W4 /WX)
	target_compile_options(${PROJECT_LIB_NAME} INTERFACE /external:anglebrackets /external:W0)
else()
	target_compile_options(${PROJECT_LIB_NAME} INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

include(CMakeInstall.txt)
