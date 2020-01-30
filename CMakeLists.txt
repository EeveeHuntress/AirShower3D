cmake_minimum_required(VERSION 2.8.6)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0017 NEW)
cmake_policy(SET CMP0020 NEW)

project("GraPra 2019 - AirShower3D" C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

#if(CMAKE_COMPILER_IS_GNUCXX)
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Wextra")
#endif()

# Required libraries
find_package(Qt5OpenGL 5.4.0 REQUIRED)

# process .ui files
############################
# List your ui files here  #
# (if you need any)        #
############################
qt5_wrap_ui(ui_mainwindow.h gui/mainwindow.ui)

#process resource files
############################
# List your qrc files here #
# (if you need any)        #
############################
qt5_add_resources (ResourceSources shader.qrc images.qrc data.qrc)

# Optional libraries
find_package(GTA QUIET)

# The utility library
add_subdirectory(glbase)

# The program
################################
# List your project files here #
################################
add_executable(airshower3d
    main.cpp
    ${ResourceSources}
    image/image.cpp
    gui/mainwindow.ui
    gui/mainwindow.cpp
    gui/glwidget.cpp
    gui/glwidget.hpp
    gui/config.cpp

    objects/drawable.cpp
    objects/skybox.cpp
    objects/detector.cpp
    objects/airshower.cpp
    objects/ground.cpp
    objects/crown.cpp
    objects/sfcenter.cpp
    objects/solarpanel.cpp
    #    objects/mountain.cpp


    shader/detector.vs.glsl
    shader/detector.fs.glsl
    shader/skybox.vs.glsl
    shader/skybox.fs.glsl
    shader/ground.fs.glsl
    shader/ground.vs.glsl
    shader/airshower.fs.glsl
    shader/airshower.vs.glsl
    shader/solarpanel.fs.glsl
    shader/solarpanel.vs.glsl

)

include_directories(${CMAKE_SOURCE_DIR}/glbase ${OPENGL_INCLUDE_DIR})

####################################
# Build with correct opengl lib    #
# (this could fail on your system) #
####################################
if(WIN32 OR CYGWIN)
        target_link_libraries(airshower3d opengl32 libglbase Qt5::OpenGL ${OPENGL_gl_LIBRARY})
else()
        target_link_libraries(airshower3d GL libglbase Qt5::OpenGL ${OPENGL_gl_LIBRARY})
endif()

if(GTA_FOUND)
        add_definitions(-DHAVE_GTA)
        include_directories(${GTA_INCLUDE_DIR})
        target_link_libraries(airshower3d ${GTA_LIBRARIES})
endif()

# copy images
add_custom_command(TARGET airshower3d POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/images $<TARGET_FILE_DIR:airshower3d>/images)

install(TARGETS airshower3d RUNTIME DESTINATION bin)
