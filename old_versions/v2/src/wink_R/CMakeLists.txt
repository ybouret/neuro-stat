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
ELSE()
SET(WINK_DLL "wink.so")
ENDIF()

GET_TARGET_PROPERTY(winkLib wink LOCATION )
MESSAGE( STATUS "winkLib=${winkLib}" )

GET_TARGET_PROPERTY(pyckLib pyck LOCATION )
MESSAGE( STATUS "pyckLib=${pyckLib}" )
	
ADD_CUSTOM_TARGET(wink_R ALL
	COMMAND ${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/${WINK_DLL} ${CMAKE_CURRENT_SOURCE_DIR}/wink.cpp "${winkLib}" "${pyckLib}"
	COMMENT "Building .Call module"
	DEPENDS wink pyck
	SOURCES wink.cpp)
	
ENDIF()

