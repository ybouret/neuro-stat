SET(PAR_SRCS	par/wink-mutex.hpp
				par/wink-mutex.cpp
				par/wink-thread.hpp
				par/wink-thread.cpp
				)
SOURCE_GROUP(par FILES ${PAR_SRCS})

ADD_LIBRARY(wink STATIC wink.hpp 
			wink-rand32.hpp
			wink-rand32.cpp
			wink-utils.hpp
			wink-utils.cpp
			wink-coincidence.hpp
			wink-coincidence.cpp
			wink-lookup.hpp
			wink-lookup.cpp
			wink-iwindow.hpp
			wink-iwindow.cpp
			wink-permutation.hpp
			wink-permutation.cpp
			wink-c-matrix.hpp
			wink-c-matrix.cpp
			wink-neuro-trials.hpp
			wink-neuro-trials.cpp
			wink.cpp
			wink-neuro-pair.hpp
			wink-neuro-pair.cpp
			wink-os.hpp
			wink-os.cpp
			${PAR_SRCS}
			)

ADD_SUBDIRECTORY(tests)

