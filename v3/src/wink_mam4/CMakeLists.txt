SET(R_FOUND OFF)

FIND_PROGRAM(R_EXE "R")

IF( "${R_EXE}" STREQUAL "R_EXE-NOTFOUND")
	MESSAGE( STATUS "R is not available")
ELSE()
	MESSAGE( STATUS "R is ${R_EXE}")
	SET(R_FOUND ON)
ENDIF()


IF(R_FOUND)
IF(WIN32)
SET(WINK_DLL "wink.dll")
SET(WINK_XTS_DLL "wink_xts.dll")
ELSE()
SET(WINK_DLL "wink.so")
SET(WINK_XTS_DLL "wink_xts.so")
ENDIF()

GET_TARGET_PROPERTY(winkLib wink LOCATION )
MESSAGE( STATUS "winkLib=${winkLib}" )

GET_TARGET_PROPERTY(pyckLib pyck LOCATION )
MESSAGE( STATUS "pyckLib=${pyckLib}" )

# generic wink functions
ADD_CUSTOM_TARGET(wink_R ALL
	COMMAND ${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/${WINK_DLL} ${CMAKE_CURRENT_SOURCE_DIR}/wink.cpp "${winkLib}" "${pyckLib}"
	COMMENT "Building .Call module"
	DEPENDS wink pyck
	SOURCES wink.cpp wink.R)

IF(EXISTS ${YOCTO_RULES})
# eXact Trial Shuffling processing
FILE( WRITE ${CMAKE_CURRENT_SOURCE_DIR}/Makevars "PKG_CPPFLAGS=-I${YROOT}/include" )
ADD_CUSTOM_TARGET(wink_XTS ALL
    COMMAND ${CMAKE}
	COMMAND ${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/${WINK_XTS_DLL} ${CMAKE_CURRENT_SOURCE_DIR}/wink_xts.cpp -L${YROOT}/lib -ly-mpa -lyocto
	COMMENT "Building eXact Trial Shuffling .Call module"
	SOURCES wink_xts.cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
ENDIF(EXISTS ${YOCTO_RULES})

ENABLE_TESTING()
ADD_TEST(NAME true_coinc WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMAND bash test_true_coinc.sh)
ADD_TEST(NAME test_perm  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMAND bash test_perm.sh)
ADD_TEST(NAME test_boot  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMAND bash test_boot.sh)

ENDIF()


