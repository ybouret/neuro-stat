

SET(c_srcs
record.hpp
record.cpp
neuron.hpp
neuron.cpp
trial.hpp
trial.cpp
neurodata.hpp
neurodata.cpp
step-function.hpp
step-function.cpp
experiment.hpp
experiment.cpp)

ADD_LIBRARY(neurocorr STATIC
${c_srcs})

ADD_EXECUTABLE(test-nc
	test-main.cpp
	test-experiment.cpp
	test-records.cpp
    	test-neurodata.cpp
)

TARGET_LINK_LIBRARIES(test-nc neurocorr)
TARGET_LINK_YOCTO(test-nc)

ENABLE_TESTING()

ADD_TEST(Records   test-nc records)
ADD_TEST(NeuroData test-nc neurodata)



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
SOURCES neurocorr.cpp neurocorr.R ${c_srcs} test-nc.R gen_data_neur.R
DEPENDS
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

ADD_TEST(NAME test_nc_R WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMAND ${R_EXE} --no-save --slave -f test-nc.R)
ENDIF()


ADD_CUSTOM_TARGET(check-nc COMMAND ${CMAKE_CTEST_COMMAND} -V DEPENDS test-nc )
