ADD_LIBRARY(ncv4 STATIC
types.hpp
types.cpp
train.hpp
train.cpp
locate.hpp
locate.cpp
records.hpp
records.cpp
cpw.hpp
cpw.cpp
cpw-build.cpp
cpw-moments.cpp
cpw-max-abs-on.cpp
cpw-intg.cpp
cpw-product.cpp
phi.hpp
phi.cpp
box.hpp
box.cpp
boxes.hpp
boxes.cpp
vector-builder.hpp
vector-builder.cpp
symindex.hpp
matrix-builder.hpp
matrix-builder.cpp
minimiser.hpp
minimiser.cpp
)

ADD_SUBDIRECTORY(tests)

IF(R_FOUND)

IF(WIN32)
SET(THE_DLL "neurocorr.dll")
ELSE()
SET(THE_DLL "neurocorr.so")
ENDIF()

MESSAGE( STATUS "Will Compile R Code..." )

ADD_CUSTOM_TARGET(neurocorr_Rv4 ALL
COMMAND
${R_EXE} CMD SHLIB -c --preclean -o ${CMAKE_CURRENT_SOURCE_DIR}/${THE_DLL} ${CMAKE_CURRENT_SOURCE_DIR}/neurocorr.cpp "$<TARGET_FILE:ncv4>" -L${SDK}/lib -ly-mk -lyocto
COMMENT "Building .Call module: ${THE_DLL}"
SOURCES neurocorr.cpp neurocorr.R  test.R test-phi.R test-min.R
DEPENDS ncv4
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
ENDIF()


