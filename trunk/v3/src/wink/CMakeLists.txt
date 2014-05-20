INCLUDE(../../../share/rules.cmake )
ADD_LIBRARY(wink STATIC types.hpp
	record.hpp		record.cpp 
	neuron.hpp		neuron.cpp
	lookup.hpp      	lookup.cpp
	iwindow.hpp		iwindow.cpp
	coincidences.hpp	coincidences.cpp
	drawing.hpp		drawing.cpp
	neurons.hpp		neurons.cpp
)

ADD_SUBDIRECTORY(tests)


