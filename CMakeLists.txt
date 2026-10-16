cmake_minimum_required(VERSION 3.25...4.2)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0141 NEW)

project(racers2 LANGUAGES CXX)

if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "Use the 32-bit MSVC 6.0 SP5 environment. On Linux, use tools/build.py.")
endif()
if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "12.0.8804")
  message(FATAL_ERROR "LR2 requires cl 12.00.8804 (SP5), with C2.DLL 12.00.8966. See docs/compiler.md.")
endif()

include(cmake/reccmp.cmake)
file(STRINGS "${PROJECT_SOURCE_DIR}/sources.txt" LR2_SOURCES)
add_executable(legoracers2 WIN32 ${LR2_SOURCES})
set_target_properties(legoracers2 PROPERTIES
  OUTPUT_NAME "LEGO Racers 2"
  MSVC_RUNTIME_LIBRARY "MultiThreaded"
  MSVC_DEBUG_INFORMATION_FORMAT "Embedded"
)
target_include_directories(legoracers2 PRIVATE "LEGO Racers 2/include" util)
target_compile_definitions(legoracers2 PRIVATE ENABLE_DECOMP_ASSERTS)

# Keep these settings in sync with tools/build.py. Optimization settings are a
# starting point, to be refined by LR2 comparisons rather than copied from LR1.
set(CMAKE_CXX_FLAGS "/W3 /GX /DWIN32 /D_WINDOWS")
set(CMAKE_CXX_FLAGS_DEBUG "/Od /D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob1 /DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Ob1 /DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /DNDEBUG")
# Retain recovered functions whose original callers have not been recovered yet.
set(CMAKE_EXE_LINKER_FLAGS "/machine:I386 /incremental:no /opt:noref,icf /pdbtype:sept /debug")
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
  set(CMAKE_EXE_LINKER_FLAGS_${config} "/incremental:no")
endforeach()
target_link_options(legoracers2 PRIVATE "/MAP:$<TARGET_FILE_DIR:legoracers2>/legoracers2.map")
target_link_libraries(legoracers2 PRIVATE user32)

reccmp_add_target(legoracers2 ID LEGORACERS2)
reccmp_configure()
