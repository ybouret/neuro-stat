######################################
##
## Finding R
##
#####################################
SET(R_FOUND OFF)

FIND_PROGRAM(R_EXE "R")

IF("${R_EXE}" STREQUAL "R_EXE-NOTFOUND")
	# no R was installed on this system
        MESSAGE( STATUS "R is not available")
ELSE("${R_EXE}" STREQUAL "R_EXE-NOTFOUND")
	# ok, we found R
        MESSAGE( STATUS "R is ${R_EXE}")
        SET(R_FOUND ON)
ENDIF("${R_EXE}" STREQUAL "R_EXE-NOTFOUND")

