ADD_LIBRARY(wink STATIC
	record.hpp		record.cpp 
	one-neuron.hpp		one-neuron.cpp
	lookup.hpp      	lookup.cpp
	iwindow.hpp		iwindow.cpp
	coincidences.hpp	coincidences.cpp
	drawing.hpp		drawing.cpp
)

ADD_SUBDIRECTORY(tests)


