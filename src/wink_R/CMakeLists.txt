
SET(R_FOUND OFF)

FIND_PROGRAM(R_EXE "R")

IF( "${R_EXE}" STREQUAL "R_EXE-NOTFOUND")
	MESSAGE( STATUS "R is not available")
ELSE()
	MESSAGE( STATUS "R is ${R_EXE}")
	SET(R_FOUND ON)
ENDIF()


IF(R_FOUND)
GET_TARGET_PROPERTY(winkLib wink LOCATION )
MESSAGE( STATUS "winkLib=${winkLib}" )

ADD_CUSTOM_TARGET(wink_ser ALL
	COMMAND ${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/wink_ser.so ${CMAKE_CURRENT_SOURCE_DIR}/wink_ser.cpp "${winkLib}"
	COMMENT "Building .Call serial interface"
	DEPENDS wink
	SOURCES wink_ser.cpp)
	
ADD_CUSTOM_TARGET(wink_par ALL
	COMMAND ${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/wink_par.so ${CMAKE_CURRENT_SOURCE_DIR}/wink_par.cpp "${winkLib}"
	COMMENT "Building .Call parallel interface"
	DEPENDS wink
	SOURCES wink_par.cpp)
	
ENDIF()

