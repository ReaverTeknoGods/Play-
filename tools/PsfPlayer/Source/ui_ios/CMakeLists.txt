cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../deps/Dependencies/cmake-modules
	${CMAKE_MODULE_PATH}
)
include(Header)

project(PsfPlayer)
set(PSF_PROJECT_NAME "PsfPlayer")
set(PSF_PROJECT_VERSION "1.05")

if(NOT TARGET PsfCore)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../
		${CMAKE_CURRENT_BINARY_DIR}/PsfCore
	)
endif()
list(APPEND PSF_UI_IOS_PROJECT_LIBS PsfCore)

if(NOT TARGET sh_openal)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../SH_OpenAL
		${CMAKE_CURRENT_BINARY_DIR}/SH_OpenAL
)
endif()
list(APPEND PSF_UI_IOS_PROJECT_LIBS sh_openal)

find_library(UIKIT_LIBRARY NAMES UIKit)
find_library(AVFOUNDATION_LIBRARY NAMES AVFoundation)
find_library(COREGRAPHICS_LIBRARY NAMES CoreGraphics)
find_library(MEDIAPLAYER_LIBRARY NAMES MediaPlayer)

set (OSX_SOURCES
	AppDelegate.mm
	FileInfoViewController.mm
	main.mm
	MainTabBarController.mm
	NSStringUtils.mm
	PlaylistSelectViewController.m
	PlaylistViewController.mm
)

set (OSX_HEADERS
	AppDelegate.h
	FileInfoViewController.h
	MainTabBarController.h
	NSStringUtils.h
	PlaylistSelectViewController.h
	PlaylistViewController.h
)

set (OSX_RES
	${CMAKE_CURRENT_SOURCE_DIR}/Icon.png
	${CMAKE_CURRENT_SOURCE_DIR}/Images.xcassets
	${CMAKE_CURRENT_SOURCE_DIR}/PlaylistBarIcon.png
	${CMAKE_CURRENT_SOURCE_DIR}/PlaylistBarIcon@2x.png
	${CMAKE_CURRENT_SOURCE_DIR}/Storyboard.storyboard
	${CMAKE_CURRENT_SOURCE_DIR}/TrackInfoBarIcon.png
	${CMAKE_CURRENT_SOURCE_DIR}/TrackInfoBarIcon@2x.png
)

# Add our Executable
add_executable(PsfPlayer MACOSX_BUNDLE ${OSX_SOURCES} ${OSX_HEADERS} ${OSX_RES})
set_target_properties(PsfPlayer PROPERTIES COMPILE_FLAGS "-x objective-c++")

target_link_libraries(PsfPlayer ${UIKIT_LIBRARY})
target_link_libraries(PsfPlayer ${AVFOUNDATION_LIBRARY})
target_link_libraries(PsfPlayer ${COREGRAPHICS_LIBRARY})
target_link_libraries(PsfPlayer ${MEDIAPLAYER_LIBRARY})
target_link_libraries(PsfPlayer ${PSF_UI_IOS_PROJECT_LIBS} "-ObjC")

# AOT stuff
if(USE_AOT_CACHE)
	target_link_libraries(PsfPlayer "${CMAKE_CURRENT_SOURCE_DIR}/../../build_aot/output/macho/$(CURRENT_ARCH)/PsfBlocks.o")
	set_target_properties(PsfPlayer PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

target_include_directories(PsfPlayer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set a custom plist file for the app bundle
# NOTE: for these values to be used Info.plist has to be edited
string(TIMESTAMP DATE "%Y-%m-%d")
set_target_properties(
	PsfPlayer
	PROPERTIES
		MACOSX_BUNDLE_INFO_STRING "${PSF_PROJECT_NAME}"
		MACOSX_BUNDLE_GUI_IDENTIFIER "com.virtualapplications.PsfPlayer"
		MACOSX_BUNDLE_LONG_VERSION_STRING "${PSF_PROJECT_NAME} Version ${PSF_PROJECT_VERSION}"
		MACOSX_BUNDLE_BUNDLE_NAME ${PSF_PROJECT_NAME}
		MACOSX_BUNDLE_SHORT_VERSION_STRING "${PSF_PROJECT_VERSION}"
		MACOSX_BUNDLE_BUNDLE_VERSION ${DATE}
		MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2006-2017, Jean-Philip Desjardins. All Rights Reserved."
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
		XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
		XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME "LaunchImage"
		XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO"
		RESOURCE "${OSX_RES}"
)

