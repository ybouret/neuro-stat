########################################################################
##
##
##
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(neuro-stat-v3)

SET(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/..)

IF(WIN32)
        SET(YROOT E:/program/yocto4/sdk)
ELSE(WIN32)
        SET(YROOT $ENV{HOME}/program/yocto4/sdk)
ENDIF(WIN32)

SET(YOCTO_RULES ${YROOT}/share/yocto/rules.cmake)

ADD_SUBDIRECTORY(pyck)
ADD_SUBDIRECTORY(wink)
ADD_SUBDIRECTORY(wink_R)

ADD_SUBDIRECTORY(helpers)



