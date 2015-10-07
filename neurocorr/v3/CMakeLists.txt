SET(srcs
types.hpp
types.cpp
train.hpp
train.cpp
train-find.cpp
records.hpp
records.cpp
cpw.hpp
cpw.cpp
cpw-from-spikes.cpp
cpw-product.cpp
cpw-eval.cpp
cpw-intg.cpp
phi.hpp
phi.cpp
box.hpp
box.cpp
boxes.hpp
boxes.cpp
vector-builder.hpp
vector-builder.cpp
matrix-builder.hpp
matrix-builder.cpp
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
${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/${THE_DLL} ${CMAKE_CURRENT_SOURCE_DIR}/neurocorr.cpp "$<TARGET_FILE:ncv3>" -L${SDK}/lib -ly-mk -lyocto
COMMENT "Building .Call module: ${THE_DLL}"
SOURCES neurocorr.cpp neurocorr.R ${srcs} 
gen_data_neur.R test-nc.R test-simu.R GeneYann.R SimuHawkesPatou.R
DEPENDS 
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
ENDIF()

