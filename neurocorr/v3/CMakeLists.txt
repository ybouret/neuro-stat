SET(srcs
types.hpp
types.cpp
train.hpp
train.cpp
experiment.hpp
experiment.cpp
records.hpp
records.cpp
cpw.hpp
cpw.cpp
)

ADD_LIBRARY(ncv3 STATIC ${srcs})

ADD_SUBDIRECTORY(tests)

IF(R_FOUND)

IF(WIN32)
SET(THE_DLL "neurocorr.dll")
ELSE()
SET(THE_DLL "neurocorr.so")
ENDIF()

MESSAGE( STATUS "Will Compile R Code..." )


ADD_CUSTOM_TARGET(neurocorr_Rv3 ALL
COMMAND
${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/${THE_DLL} ${CMAKE_CURRENT_SOURCE_DIR}/neurocorr.cpp "$<TARGET_FILE:ncv3>" -L${SDK}/lib -lyocto
COMMENT "Building .Call module: ${THE_DLL}"
SOURCES neurocorr.cpp neurocorr.R ${srcs} gen_data_neur.R test-nc.R
DEPENDS 
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
ENDIF()

