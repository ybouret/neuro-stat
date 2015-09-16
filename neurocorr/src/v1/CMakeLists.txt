

SET(c_srcs
record.hpp
record.cpp
neuron.hpp
neuron.cpp
trial.hpp
trial.cpp
neurodata.hpp
neurodata.cpp
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


ADD_CUSTOM_TARGET(check-nc COMMAND ${CMAKE_CTEST_COMMAND} -V DEPENDS test-nc )

IF(R_FOUND)

IF(WIN32)
SET(THE_DLL "neurocorr.dll")
ELSE()
SET(THE_DLL "neurocorr.so")
ENDIF()

MESSAGE( STATUS "Will Compile R Code..." )

ADD_CUSTOM_TARGET(rlib ALL
COMMAND
${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/${THE_DLL} ${CMAKE_CURRENT_SOURCE_DIR}/neurocorr.cpp -L${SDK}/lib -lyocto
COMMENT "Building .Call module: ${THE_DLL}"
SOURCES neurocorr.cpp neurocorr.R ${c_srcs}
DEPENDS
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

ENDIF()
