########################################################################
##
##
##
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(neuro-stat-v3)

SET(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/..)

INCLUDE(../../share/rules.cmake)

ADD_SUBDIRECTORY(pyck)
ADD_SUBDIRECTORY(wink)
ADD_SUBDIRECTORY(wink_R)
ADD_SUBDIRECTORY(helpers)



