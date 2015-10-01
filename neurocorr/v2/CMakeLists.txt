SET(c_srcs
types.hpp
types.cpp
train.hpp
train.cpp
records.hpp
records.cpp
cpwfn.hpp
cpwfn.cpp
cpwfn-build.cpp
cpwfn-product.cpp
cpwfn-integrate.cpp
cpwfn-eval.cpp
phifn.hpp
phifn.cpp
box.hpp
box.cpp
boxes.hpp
boxes.cpp
boxes-mixed.hpp
boxes-mixed.cpp
boxes-linear.hpp
boxes-linear.cpp
boxes-intg.hpp
boxes-intg.cpp
boxes-ops.hpp
boxes-ops.cpp
)

ADD_LIBRARY(ncv2 STATIC ${c_srcs})

IF(R_FOUND)

IF(WIN32)
SET(THE_DLL "neurocorr.dll")
ELSE()
SET(THE_DLL "neurocorr.so")
ENDIF()

MESSAGE( STATUS "Will Compile R Code..." )

SET(ncLib $<TARGET_FILE:ncv2>)

ADD_CUSTOM_TARGET(neurocorr_R ALL
COMMAND
${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/${THE_DLL} ${CMAKE_CURRENT_SOURCE_DIR}/neurocorr.cpp "${ncLib}" -L${SDK}/lib -lyocto
COMMENT "Building .Call module: ${THE_DLL}"
SOURCES neurocorr.cpp neurocorr.R ${c_srcs} tests/test-nc.R tests/gen_data_neur.R
DEPENDS
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
ENDIF()

ADD_SUBDIRECTORY(tests)

