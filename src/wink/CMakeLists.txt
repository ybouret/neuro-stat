ADD_LIBRARY(wink STATIC wink.hpp 
			wink-rand32.hpp
			wink-rand32.cpp
			wink-utils.hpp
			wink-utils.cpp
			wink-coincidence.cpp
			wink-lookup.hpp
			wink-lookup.cpp
			wink-iwindow.cpp
			wink-permutation.hpp
			wink-permutation.cpp
			wink-c-matrix.cpp
			wink-neuro-trials.cpp
			)

ADD_SUBDIRECTORY(tests)

