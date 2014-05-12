IF(WIN32)
        SET(YROOT E:/program/yocto4/sdk)
ELSE(WIN32)
        SET(YROOT $ENV{HOME}/program/yocto4/sdk)
ENDIF()

SET(YOCTO_RULES ${YROOT}/share/yocto/rules.cmake)

IF(EXISTS ${YOCTO_RULES})
INCLUDE(${YOCTO_RULES})
INCLUDE_DIRECTORIES(${YROOT}/include)
LINK_DIRECTORIES(${YROOT}/lib)

ADD_EXECUTABLE(proba-cm proba-cm.cpp)
TARGET_LINK_YOCTO(proba-cm mk mpa)


ENDIF()


