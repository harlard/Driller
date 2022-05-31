#/*
#-----------------------------------------------------------------------------
# Filename:    common.cmake
#-----------------------------------------------------------------------------
#
# This source file is based on
#   ___                 __    __ _ _    _
#  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
# //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
#/ \_// (_| | | |  __/  \  /\  /| |   <| |
#\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
#      |___/
#      Tutorial Framework
#      http://www.ogre3d.org/tikiwiki/
#
# Modified for CMU students.
#-----------------------------------------------------------------------------
#*/

cmake_minimum_required(VERSION 2.6)

set(APP "SimpleGame")

find_package(Threads)

set(HDRS
	./src/Game.h
	./src/Objects.h
	./src/Object/Tank.h
	./src/Object/Floor.h
	./src/Object/Rock.h
	./src/Object/CameraT.h
	./src/Object/DynamicObject.h
)

set(SRCS
	./src/Game.cpp
	./src/main.cpp
	./src/Object/Tank.cpp
	./src/Object/Floor.cpp
	./src/Object/Rock.cpp
	./src/Object/CameraT.cpp
	./src/Object/DynamicObject.cpp

)

include("tools/cmake/common.cmake")
