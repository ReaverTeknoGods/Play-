cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../../deps/Dependencies/cmake-modules
	${CMAKE_MODULE_PATH}
)
include(Header)

project(SpuTest)

if (NOT TARGET PlayCore)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../../Source/
		${CMAKE_CURRENT_BINARY_DIR}/Source
	)
endif()

add_executable(SpuTest
	KeyOnOffTest.cpp
	Main.cpp
	MultiCoreIrqTest.cpp
	SetRepeatTest.cpp
	SetRepeatTest2.cpp
	SimpleIrqTest.cpp
	SweepTest.cpp
	Test.cpp

	MultiCoreIrqTest.h
	KeyOnOffTest.h
	SetRepeatTest.h
	SetRepeatTest2.h
	SimpleIrqTest.h
	SweepTest.h
	Test.h
)

target_link_libraries(SpuTest PlayCore)
add_test(NAME SpuTest
	COMMAND SpuTest
)