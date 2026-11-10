cmake_minimum_required(VERSION 3.28)
project(Agora VERSION 0.1.0 LANGUAGES CXX)

# Prefer C++26 when CMake knows the compiler's mode; otherwise require C++23.
# STANDARD_REQUIRED prevents CMake from silently falling back below this choice.
if("cxx_std_26" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(AGORA_CXX_STANDARD 26)
elseif("cxx_std_23" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    set(AGORA_CXX_STANDARD 23)
    message(STATUS "C++26 mode unavailable for this toolchain; using C++23")
else()
    message(FATAL_ERROR "Agora requires C++26 or the C++23 fallback. Update your compiler/CMake.")
endif()
message(STATUS "Agora language mode: C++${AGORA_CXX_STANDARD}")
set(CMAKE_CXX_STANDARD ${AGORA_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
    message(FATAL_ERROR "Agora modules require GCC 14 or newer. Use a current GCC/MSVC toolchain.")
endif()

add_library(agora_warnings INTERFACE)
target_compile_options(agora_warnings INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4;/permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall;-Wextra;-Wpedantic>)

add_library(agora_core STATIC)
add_library(agora::core ALIAS agora_core)
target_sources(agora_core PUBLIC FILE_SET CXX_MODULES FILES modules/agora/core.cppm)
target_compile_features(agora_core PUBLIC cxx_std_${AGORA_CXX_STANDARD})
target_compile_definitions(agora_core PUBLIC AGORA_CXX_STANDARD=${AGORA_CXX_STANDARD})
target_link_libraries(agora_core PRIVATE agora_warnings)

add_library(agora_exchange src/exchange/limit_order_book.cpp src/exchange/matching_engine.cpp)
add_library(agora::exchange ALIAS agora_exchange)
target_sources(agora_exchange PUBLIC FILE_SET CXX_MODULES FILES modules/agora/exchange.cppm)
target_link_libraries(agora_exchange PUBLIC agora::core PRIVATE agora_warnings)

add_library(agora_simulation src/simulation/event_scheduler.cpp)
add_library(agora::simulation ALIAS agora_simulation)
target_sources(agora_simulation PUBLIC FILE_SET CXX_MODULES FILES modules/agora/simulation.cppm)
target_link_libraries(agora_simulation PUBLIC agora::core PRIVATE agora_warnings)

add_library(agora_agents STATIC)
add_library(agora::agents ALIAS agora_agents)
target_sources(agora_agents PUBLIC FILE_SET CXX_MODULES FILES modules/agora/agents.cppm)
target_link_libraries(agora_agents PUBLIC agora::core PRIVATE agora_warnings)

add_library(agora_protocols src/protocols/codec.cpp)
add_library(agora::protocols ALIAS agora_protocols)
target_sources(agora_protocols PUBLIC FILE_SET CXX_MODULES FILES modules/agora/protocols.cppm)
target_link_libraries(agora_protocols PUBLIC agora::core PRIVATE agora_warnings)

add_executable(agora_sim apps/sim/main.cpp)
target_link_libraries(agora_sim PRIVATE agora::exchange agora::simulation agora::agents agora::protocols agora_warnings)

include(CTest)
if(BUILD_TESTING)
    add_executable(agora_core_tests tests/core_foundation.cpp)
    target_link_libraries(agora_core_tests PRIVATE agora::core agora_warnings)
    add_test(NAME core_foundation COMMAND agora_core_tests)
    add_test(NAME scaffold_smoke COMMAND agora_sim --version)
    set_tests_properties(scaffold_smoke PROPERTIES PASS_REGULAR_EXPRESSION "Agora 0.1.0")
    add_test(NAME foundation_demo COMMAND agora_sim --foundation)
    set_tests_properties(foundation_demo PROPERTIES PASS_REGULAR_EXPRESSION "6 obols = 1 drachma")
endif()
