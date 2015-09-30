

SET(c_srcs
record.hpp
record.cpp
neuron.hpp
neuron.cpp
trial.hpp
trial.cpp
neurodata.hpp
neurodata.cpp
cpw-function.hpp
cpw-function.cpp
experiment.hpp
experiment.cpp)

ADD_LIBRARY(neurocorr STATIC
${c_srcs})

IF(R_FOUND)

IF(WIN32)
SET(THE_DLL "neurocorr.dll")
ELSE()
SET(THE_DLL "neurocorr.so")
ENDIF()

MESSAGE( STATUS "Will Compile R Code..." )

SET(ncLib $<TARGET_FILE:neurocorr>)

ADD_CUSTOM_TARGET(rlib ALL
COMMAND
${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/${THE_DLL} ${CMAKE_CURRENT_SOURCE_DIR}/neurocorr.cpp "${ncLib}" -L${SDK}/lib -lyocto
COMMENT "Building .Call module: ${THE_DLL}"
SOURCES neurocorr.cpp neurocorr.R ${c_srcs} tests/test-nc.R tests/gen_data_neur.R
DEPENDS
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
ENDIF()

ADD_SUBDIRECTORY(tests)

