ADD_LIBRARY(wink STATIC
	record.hpp	record.cpp 
	experiment.hpp	experiment.cpp
	lookup.hpp      lookup.cpp
	iwindow.hpp	iwindow.cpp
)

ADD_SUBDIRECTORY(tests)


