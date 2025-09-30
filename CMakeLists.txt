cmake_minimum_required(VERSION 3.16)
project(TextEditor C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES source/platform.cpp source/editor.cpp source/editor_view.cpp)

add_executable(TextEditor ${SOURCES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TextEditor)

if(WIN32)
    target_link_libraries(TextEditor PRIVATE
            user32
            gdi32
            kernel32
    )

    if(MSVC)
        set_target_properties(TextEditor PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
    else()
        set_target_properties(TextEditor PROPERTIES LINK_FLAGS "-Wl,-subsystem,windows")
    endif()
endif()
