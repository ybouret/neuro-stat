// \file
/**
 \brief detecting O/S
 */
#ifndef WINK_OS_INCLUDED
#define WINK_OS_INCLUDED 1

#if defined(_WIN32)
#	define WINK_PLATFORM "Windows"
#	define WINK_WIN		1
#	define _WIN32_WINNT    0x0501
#endif

#if defined(__MACH__) && ( defined(__APPLE__) || defined(__APPLE_CC__) )
#	define WINK_PLATFORM	"Darwin"
#	define WINK_BSD 		1
#	define WINK_APPLE      1
#endif

#if defined(__linux__)
#	define WINK_PLATFORM	"Linux"
#	define WINK_BSD 		 1
#	define WINK_LINUX       1
#endif

#if defined(__FreeBSD__)
#	define WINK_PLATFORM	"FreeBSD"
#	define WINK_BSD 		 1
#	define WINK_FREEBSD     1
#endif

#if defined(__OpenBSD__)
#	define WINK_PLATFORM	"OpenBSD"
#	define WINK_BSD 		 1
#	define WINK_OPENBSD     1
#endif

#if defined(__sun__)
#	define WINK_PLATFORM	"SunOS"
#	define WINK_BSD 		1
#	define WINK_SUNOS      1
#endif

#if defined(__GNUC__) || defined(__DMC__)

/*--------------------------------------------------------------------*/
/*   ENTER: GNU systems                                               */
/*--------------------------------------------------------------------*/
#	include	<stdint.h>
#	define	WINK_HAS_STDINT 1
#	define	WINK_U64(X) X##ULL
#	define	WINK_I64(X) X##LL
/*--------------------------------------------------------------------*/
/*   LEAVE: GNU systems                                               */
/*--------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------*/
/*   ENTER: other systems                                             */
/*--------------------------------------------------------------------*/
#if 	defined(_MSC_VER)

/* Microsoft built-in types */
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

typedef __int8  int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
#		define	WINK_HAS_STDINT 1
#		define	WINK_U64(X) X##ui64
#		define	WINK_I64(X) X##i64
#	endif /* _MSC_VER */

#endif

#if !defined(WINK_HAS_STDINT)
#error "WINK_HAS_STDINT failure"
#endif

namespace wink
{
    const char *platform_name() throw();
    void        critical_error(int,const char*) throw();
}


#endif
