ADD_LIBRARY(wink STATIC wink.hpp 
			wink-utils.cpp
			wink-coincidence.cpp
			wink-lookup.cpp
			wink-iwindow.cpp
			wink-permutation.cpp
			)

ADD_SUBDIRECTORY(tests)

