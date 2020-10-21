/*
** $Id: lapi.c $
** Lua API
** See Copyright Notice in lua.h
*/

#define lapi_c
#define LUA_CORE


/*** Start of inlined file: lprefix.h ***/
#ifndef lprefix_h
#define lprefix_h

/*
** Allows POSIX/XSI stuff
*/
#if !defined(LUA_USE_C89)	/* { */

#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE           600
#elif _XOPEN_SOURCE == 0
#undef _XOPEN_SOURCE  /* use -D_XOPEN_SOURCE=0 to undefine it */
#endif

/*
** Allows manipulation of large files in gcc and some other compilers
*/
#if !defined(LUA_32BITS) && !defined(_FILE_OFFSET_BITS)
#define _LARGEFILE_SOURCE       1
#define _FILE_OFFSET_BITS       64
#endif

#endif				/* } */

/*
** Windows stuff
*/
#if defined(_WIN32)	/* { */

#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS  /* avoid warnings about ISO C functions */
#endif

#endif			/* } */

#endif

/*** End of inlined file: lprefix.h ***/

#include <limits.h>
#include <stdarg.h>
#include <string.h>


/*** Start of inlined file: lua.h ***/
#ifndef lua_h
#define lua_h

#include <stdarg.h>
#include <stddef.h>


/*** Start of inlined file: luaconf.h ***/
#ifndef luaconf_h
#define luaconf_h

#include <limits.h>
#include <stddef.h>

/*
** ===================================================================
** General Configuration File for Lua
**
** Some definitions here can be changed externally, through the
** compiler (e.g., with '-D' options). Those are protected by
** '#if !defined' guards. However, several other definitions should
** be changed directly here, either because they affect the Lua
** ABI (by making the changes here, you ensure that all software
** connected to Lua, such as C libraries, will be compiled with the
** same configuration); or because they are seldom changed.
**
** Search for "@@" to find all configurable definitions.
** ===================================================================
*/

/*
** {====================================================================
** System Configuration: macros to adapt (if needed) Lua to some
** particular platform, for instance restricting it to C89.
** =====================================================================
*/

/*
@@ LUAI_MAXCSTACK defines the maximum depth for nested calls and
** also limits the maximum depth of other recursive algorithms in
** the implementation, such as syntactic analysis. A value too
** large may allow the interpreter to crash (C-stack overflow).
** The default value seems ok for regular machines, but may be
** too high for restricted hardware.
** The test file 'cstack.lua' may help finding a good limit.
** (It will crash with a limit too high.)
*/
#if !defined(LUAI_MAXCSTACK)
#define LUAI_MAXCSTACK		2000
#endif

/*
@@ LUA_USE_C89 controls the use of non-ISO-C89 features.
** Define it if you want Lua to avoid the use of a few C99 features
** or Windows-specific features on Windows.
*/
/* #define LUA_USE_C89 */

/*
** By default, Lua on Windows use (some) specific Windows features
*/
#if !defined(LUA_USE_C89) && defined(_WIN32) && !defined(_WIN32_WCE)
#define LUA_USE_WINDOWS  /* enable goodies for regular Windows */
#endif

#if defined(LUA_USE_WINDOWS)
#define LUA_DL_DLL	/* enable support for DLL */
#define LUA_USE_C89	/* broadly, Windows is C89 */
#endif

#if defined(LUA_USE_LINUX)
#define LUA_USE_POSIX
#define LUA_USE_DLOPEN		/* needs an extra library: -ldl */
#endif

#if defined(LUA_USE_MACOSX)
#define LUA_USE_POSIX
#define LUA_USE_DLOPEN		/* MacOS does not need -ldl */
#endif

/*
@@ LUAI_IS32INT is true iff 'int' has (at least) 32 bits.
*/
#define LUAI_IS32INT	((UINT_MAX >> 30) >= 3)

/* }================================================================== */

/*
** {==================================================================
** Configuration for Number types.
** ===================================================================
*/

/*
@@ LUA_32BITS enables Lua with 32-bit integers and 32-bit floats.
*/
/* #define LUA_32BITS */

/*
@@ LUA_C89_NUMBERS ensures that Lua uses the largest types available for
** C89 ('long' and 'double'); Windows always has '__int64', so it does
** not need to use this case.
*/
#if defined(LUA_USE_C89) && !defined(LUA_USE_WINDOWS)
#define LUA_C89_NUMBERS
#endif

/*
@@ LUA_INT_TYPE defines the type for Lua integers.
@@ LUA_FLOAT_TYPE defines the type for Lua floats.
** Lua should work fine with any mix of these options supported
** by your C compiler. The usual configurations are 64-bit integers
** and 'double' (the default), 32-bit integers and 'float' (for
** restricted platforms), and 'long'/'double' (for C compilers not
** compliant with C99, which may not have support for 'long long').
*/

/* predefined options for LUA_INT_TYPE */
#define LUA_INT_INT		1
#define LUA_INT_LONG		2
#define LUA_INT_LONGLONG	3

/* predefined options for LUA_FLOAT_TYPE */
#define LUA_FLOAT_FLOAT		1
#define LUA_FLOAT_DOUBLE	2
#define LUA_FLOAT_LONGDOUBLE	3

#if defined(LUA_32BITS)		/* { */
/*
** 32-bit integers and 'float'
*/
#if LUAI_IS32INT  /* use 'int' if big enough */
#define LUA_INT_TYPE	LUA_INT_INT
#else  /* otherwise use 'long' */
#define LUA_INT_TYPE	LUA_INT_LONG
#endif
#define LUA_FLOAT_TYPE	LUA_FLOAT_FLOAT

#elif defined(LUA_C89_NUMBERS)	/* }{ */
/*
** largest types available for C89 ('long' and 'double')
*/
#define LUA_INT_TYPE	LUA_INT_LONG
#define LUA_FLOAT_TYPE	LUA_FLOAT_DOUBLE

#endif				/* } */

/*
** default configuration for 64-bit Lua ('long long' and 'double')
*/
#if !defined(LUA_INT_TYPE)
#define LUA_INT_TYPE	LUA_INT_LONGLONG
#endif

#if !defined(LUA_FLOAT_TYPE)
#define LUA_FLOAT_TYPE	LUA_FLOAT_DOUBLE
#endif

/* }================================================================== */

/*
** {==================================================================
** Configuration for Paths.
** ===================================================================
*/

/*
** LUA_PATH_SEP is the character that separates templates in a path.
** LUA_PATH_MARK is the string that marks the substitution points in a
** template.
** LUA_EXEC_DIR in a Windows path is replaced by the executable's
** directory.
*/
#define LUA_PATH_SEP            ";"
#define LUA_PATH_MARK           "?"
#define LUA_EXEC_DIR            "!"

/*
@@ LUA_PATH_DEFAULT is the default path that Lua uses to look for
** Lua libraries.
@@ LUA_CPATH_DEFAULT is the default path that Lua uses to look for
** C libraries.
** CHANGE them if your machine has a non-conventional directory
** hierarchy or if you want to install your libraries in
** non-conventional directories.
*/

#define LUA_VDIR	LUA_VERSION_MAJOR "." LUA_VERSION_MINOR
#if defined(_WIN32)	/* { */
/*
** In Windows, any exclamation mark ('!') in the path is replaced by the
** path of the directory of the executable file of the current process.
*/
#define LUA_LDIR	"!\\lua\\"
#define LUA_CDIR	"!\\"
#define LUA_SHRDIR	"!\\..\\share\\lua\\" LUA_VDIR "\\"

#if !defined(LUA_PATH_DEFAULT)
#define LUA_PATH_DEFAULT  \
  LUA_LDIR"?.lua;"  LUA_LDIR"?\\init.lua;" \
  LUA_CDIR"?.lua;"  LUA_CDIR"?\\init.lua;" \
  LUA_SHRDIR"?.lua;" LUA_SHRDIR"?\\init.lua;" \
  ".\\?.lua;" ".\\?\\init.lua"
#endif

#if !defined(LUA_CPATH_DEFAULT)
#define LUA_CPATH_DEFAULT \
  LUA_CDIR"?.dll;" \
  LUA_CDIR"..\\lib\\lua\\" LUA_VDIR "\\?.dll;" \
  LUA_CDIR"loadall.dll;" ".\\?.dll"
#endif

#else			/* }{ */

#define LUA_ROOT	"/usr/local/"
#define LUA_LDIR	LUA_ROOT "share/lua/" LUA_VDIR "/"
#define LUA_CDIR	LUA_ROOT "lib/lua/" LUA_VDIR "/"

#if !defined(LUA_PATH_DEFAULT)
#define LUA_PATH_DEFAULT  \
  LUA_LDIR"?.lua;"  LUA_LDIR"?/init.lua;" \
  LUA_CDIR"?.lua;"  LUA_CDIR"?/init.lua;" \
  "./?.lua;" "./?/init.lua"
#endif

#if !defined(LUA_CPATH_DEFAULT)
#define LUA_CPATH_DEFAULT \
  LUA_CDIR"?.so;" LUA_CDIR"loadall.so;" "./?.so"
#endif

#endif			/* } */

/*
@@ LUA_DIRSEP is the directory separator (for submodules).
** CHANGE it if your machine does not use "/" as the directory separator
** and is not Windows. (On Windows Lua automatically uses "\".)
*/
#if !defined(LUA_DIRSEP)

#if defined(_WIN32)
#define LUA_DIRSEP	"\\"
#else
#define LUA_DIRSEP	"/"
#endif

#endif

/* }================================================================== */

/*
** {==================================================================
** Marks for exported symbols in the C code
** ===================================================================
*/

/*
@@ LUA_API is a mark for all core API functions.
@@ LUALIB_API is a mark for all auxiliary library functions.
@@ LUAMOD_API is a mark for all standard library opening functions.
** CHANGE them if you need to define those functions in some special way.
** For instance, if you want to create one Windows DLL with the core and
** the libraries, you may want to use the following definition (define
** LUA_BUILD_AS_DLL to get it).
*/
#if defined(LUA_BUILD_AS_DLL)	/* { */

#if defined(LUA_CORE) || defined(LUA_LIB)	/* { */
#define LUA_API __declspec(dllexport)
#else						/* }{ */
#define LUA_API __declspec(dllimport)
#endif						/* } */

#else				/* }{ */

#define LUA_API		extern

#endif				/* } */

/*
** More often than not the libs go together with the core.
*/
#define LUALIB_API	LUA_API
#define LUAMOD_API	LUA_API

/*
@@ LUAI_FUNC is a mark for all extern functions that are not to be
** exported to outside modules.
@@ LUAI_DDEF and LUAI_DDEC are marks for all extern (const) variables,
** none of which to be exported to outside modules (LUAI_DDEF for
** definitions and LUAI_DDEC for declarations).
** CHANGE them if you need to mark them in some special way. Elf/gcc
** (versions 3.2 and later) mark them as "hidden" to optimize access
** when Lua is compiled as a shared library. Not all elf targets support
** this attribute. Unfortunately, gcc does not offer a way to check
** whether the target offers that support, and those without support
** give a warning about it. To avoid these warnings, change to the
** default definition.
*/
#if defined(__GNUC__) && ((__GNUC__*100 + __GNUC_MINOR__) >= 302) && \
    defined(__ELF__)		/* { */
#define LUAI_FUNC	__attribute__((visibility("internal"))) extern
#else				/* }{ */
#define LUAI_FUNC	extern
#endif				/* } */

#define LUAI_DDEC(dec)	LUAI_FUNC dec
#define LUAI_DDEF	/* empty */

/* }================================================================== */

/*
** {==================================================================
** Compatibility with previous versions
** ===================================================================
*/

/*
@@ LUA_COMPAT_5_3 controls other macros for compatibility with Lua 5.3.
** You can define it to get all options, or change specific options
** to fit your specific needs.
*/
#if defined(LUA_COMPAT_5_3)	/* { */

/*
@@ LUA_COMPAT_MATHLIB controls the presence of several deprecated
** functions in the mathematical library.
** (These functions were already officially removed in 5.3;
** nevertheless they are still available here.)
*/
#define LUA_COMPAT_MATHLIB

/*
@@ LUA_COMPAT_APIINTCASTS controls the presence of macros for
** manipulating other integer types (lua_pushunsigned, lua_tounsigned,
** luaL_checkint, luaL_checklong, etc.)
** (These macros were also officially removed in 5.3, but they are still
** available here.)
*/
#define LUA_COMPAT_APIINTCASTS

/*
@@ LUA_COMPAT_LT_LE controls the emulation of the '__le' metamethod
** using '__lt'.
*/
#define LUA_COMPAT_LT_LE

/*
@@ The following macros supply trivial compatibility for some
** changes in the API. The macros themselves document how to
** change your code to avoid using them.
** (Once more, these macros were officially removed in 5.3, but they are
** still available here.)
*/
#define lua_strlen(L,i)		lua_rawlen(L, (i))

#define lua_objlen(L,i)		lua_rawlen(L, (i))

#define lua_equal(L,idx1,idx2)		lua_compare(L,(idx1),(idx2),LUA_OPEQ)
#define lua_lessthan(L,idx1,idx2)	lua_compare(L,(idx1),(idx2),LUA_OPLT)

#endif				/* } */

/* }================================================================== */

/*
** {==================================================================
** Configuration for Numbers.
** Change these definitions if no predefined LUA_FLOAT_* / LUA_INT_*
** satisfy your needs.
** ===================================================================
*/

/*
@@ LUA_NUMBER is the floating-point type used by Lua.
@@ LUAI_UACNUMBER is the result of a 'default argument promotion'
@@ over a floating number.
@@ l_floatatt(x) corrects float attribute 'x' to the proper float type
** by prefixing it with one of FLT/DBL/LDBL.
@@ LUA_NUMBER_FRMLEN is the length modifier for writing floats.
@@ LUA_NUMBER_FMT is the format for writing floats.
@@ lua_number2str converts a float to a string.
@@ l_mathop allows the addition of an 'l' or 'f' to all math operations.
@@ l_floor takes the floor of a float.
@@ lua_str2number converts a decimal numeral to a number.
*/

/* The following definitions are good for most cases here */

#define l_floor(x)		(l_mathop(floor)(x))

#define lua_number2str(s,sz,n)  \
  l_sprintf((s), sz, LUA_NUMBER_FMT, (LUAI_UACNUMBER)(n))

/*
@@ lua_numbertointeger converts a float number with an integral value
** to an integer, or returns 0 if float is not within the range of
** a lua_Integer.  (The range comparisons are tricky because of
** rounding. The tests here assume a two-complement representation,
** where MININTEGER always has an exact representation as a float;
** MAXINTEGER may not have one, and therefore its conversion to float
** may have an ill-defined value.)
*/
#define lua_numbertointeger(n,p) \
  ((n) >= (LUA_NUMBER)(LUA_MININTEGER) && \
   (n) < -(LUA_NUMBER)(LUA_MININTEGER) && \
   (*(p) = (LUA_INTEGER)(n), 1))

/* now the variable definitions */

#if LUA_FLOAT_TYPE == LUA_FLOAT_FLOAT		/* { single float */

#define LUA_NUMBER	float

#define l_floatatt(n)		(FLT_##n)

#define LUAI_UACNUMBER	double

#define LUA_NUMBER_FRMLEN	""
#define LUA_NUMBER_FMT		"%.7g"

#define l_mathop(op)		op##f

#define lua_str2number(s,p)	strtof((s), (p))

#elif LUA_FLOAT_TYPE == LUA_FLOAT_LONGDOUBLE	/* }{ long double */

#define LUA_NUMBER	long double

#define l_floatatt(n)		(LDBL_##n)

#define LUAI_UACNUMBER	long double

#define LUA_NUMBER_FRMLEN	"L"
#define LUA_NUMBER_FMT		"%.19Lg"

#define l_mathop(op)		op##l

#define lua_str2number(s,p)	strtold((s), (p))

#elif LUA_FLOAT_TYPE == LUA_FLOAT_DOUBLE	/* }{ double */

#define LUA_NUMBER	double

#define l_floatatt(n)		(DBL_##n)

#define LUAI_UACNUMBER	double

#define LUA_NUMBER_FRMLEN	""
#define LUA_NUMBER_FMT		"%.14g"

#define l_mathop(op)		op

#define lua_str2number(s,p)	strtod((s), (p))

#else						/* }{ */

#error "numeric float type not defined"

#endif					/* } */

/*
@@ LUA_INTEGER is the integer type used by Lua.
**
@@ LUA_UNSIGNED is the unsigned version of LUA_INTEGER.
**
@@ LUAI_UACINT is the result of a 'default argument promotion'
@@ over a LUA_INTEGER.
@@ LUA_INTEGER_FRMLEN is the length modifier for reading/writing integers.
@@ LUA_INTEGER_FMT is the format for writing integers.
@@ LUA_MAXINTEGER is the maximum value for a LUA_INTEGER.
@@ LUA_MININTEGER is the minimum value for a LUA_INTEGER.
@@ LUA_MAXUNSIGNED is the maximum value for a LUA_UNSIGNED.
@@ LUA_UNSIGNEDBITS is the number of bits in a LUA_UNSIGNED.
@@ lua_integer2str converts an integer to a string.
*/

/* The following definitions are good for most cases here */

#define LUA_INTEGER_FMT		"%" LUA_INTEGER_FRMLEN "d"

#define LUAI_UACINT		LUA_INTEGER

#define lua_integer2str(s,sz,n)  \
  l_sprintf((s), sz, LUA_INTEGER_FMT, (LUAI_UACINT)(n))

/*
** use LUAI_UACINT here to avoid problems with promotions (which
** can turn a comparison between unsigneds into a signed comparison)
*/
#define LUA_UNSIGNED		unsigned LUAI_UACINT

#define LUA_UNSIGNEDBITS	(sizeof(LUA_UNSIGNED) * CHAR_BIT)

/* now the variable definitions */

#if LUA_INT_TYPE == LUA_INT_INT		/* { int */

#define LUA_INTEGER		int
#define LUA_INTEGER_FRMLEN	""

#define LUA_MAXINTEGER		INT_MAX
#define LUA_MININTEGER		INT_MIN

#define LUA_MAXUNSIGNED		UINT_MAX

#elif LUA_INT_TYPE == LUA_INT_LONG	/* }{ long */

#define LUA_INTEGER		long
#define LUA_INTEGER_FRMLEN	"l"

#define LUA_MAXINTEGER		LONG_MAX
#define LUA_MININTEGER		LONG_MIN

#define LUA_MAXUNSIGNED		ULONG_MAX

#elif LUA_INT_TYPE == LUA_INT_LONGLONG	/* }{ long long */

/* use presence of macro LLONG_MAX as proxy for C99 compliance */
#if defined(LLONG_MAX)		/* { */
/* use ISO C99 stuff */

#define LUA_INTEGER		long long
#define LUA_INTEGER_FRMLEN	"ll"

#define LUA_MAXINTEGER		LLONG_MAX
#define LUA_MININTEGER		LLONG_MIN

#define LUA_MAXUNSIGNED		ULLONG_MAX

#elif defined(LUA_USE_WINDOWS) /* }{ */
/* in Windows, can use specific Windows types */

#define LUA_INTEGER		__int64
#define LUA_INTEGER_FRMLEN	"I64"

#define LUA_MAXINTEGER		_I64_MAX
#define LUA_MININTEGER		_I64_MIN

#define LUA_MAXUNSIGNED		_UI64_MAX

#else				/* }{ */

#error "Compiler does not support 'long long'. Use option '-DLUA_32BITS' \

or '-DLUA_C89_NUMBERS'( see file 'luaconf.h' for details )"

#endif				/* } */

#else				/* }{ */

#error "numeric integer type not defined"

#endif				/* } */

/* }================================================================== */

/*
** {==================================================================
** Dependencies with C99 and other C details
** ===================================================================
*/

/*
@@ l_sprintf is equivalent to 'snprintf' or 'sprintf' in C89.
** (All uses in Lua have only one format item.)
*/
#if !defined(LUA_USE_C89)
#define l_sprintf(s,sz,f,i)	snprintf(s,sz,f,i)
#else
#define l_sprintf(s,sz,f,i)	((void)(sz), sprintf(s,f,i))
#endif

/*
@@ lua_strx2number converts a hexadecimal numeral to a number.
** In C99, 'strtod' does that conversion. Otherwise, you can
** leave 'lua_strx2number' undefined and Lua will provide its own
** implementation.
*/
#if !defined(LUA_USE_C89)
#define lua_strx2number(s,p)		lua_str2number(s,p)
#endif

/*
@@ lua_pointer2str converts a pointer to a readable string in a
** non-specified way.
*/
#define lua_pointer2str(buff,sz,p)	l_sprintf(buff,sz,"%p",p)

/*
@@ lua_number2strx converts a float to a hexadecimal numeral.
** In C99, 'sprintf' (with format specifiers '%a'/'%A') does that.
** Otherwise, you can leave 'lua_number2strx' undefined and Lua will
** provide its own implementation.
*/
#if !defined(LUA_USE_C89)
#define lua_number2strx(L,b,sz,f,n)  \
  ((void)L, l_sprintf(b,sz,f,(LUAI_UACNUMBER)(n)))
#endif

/*
** 'strtof' and 'opf' variants for math functions are not valid in
** C89. Otherwise, the macro 'HUGE_VALF' is a good proxy for testing the
** availability of these variants. ('math.h' is already included in
** all files that use these macros.)
*/
#if defined(LUA_USE_C89) || (defined(HUGE_VAL) && !defined(HUGE_VALF))
#undef l_mathop  /* variants not available */
#undef lua_str2number
#define l_mathop(op)		(lua_Number)op  /* no variant */
#define lua_str2number(s,p)	((lua_Number)strtod((s), (p)))
#endif

/*
@@ LUA_KCONTEXT is the type of the context ('ctx') for continuation
** functions.  It must be a numerical type; Lua will use 'intptr_t' if
** available, otherwise it will use 'ptrdiff_t' (the nearest thing to
** 'intptr_t' in C89)
*/
#define LUA_KCONTEXT	ptrdiff_t

#if !defined(LUA_USE_C89) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
#include <stdint.h>
#if defined(INTPTR_MAX)  /* even in C99 this type is optional */
#undef LUA_KCONTEXT
#define LUA_KCONTEXT	intptr_t
#endif
#endif

/*
@@ lua_getlocaledecpoint gets the locale "radix character" (decimal point).
** Change that if you do not want to use C locales. (Code using this
** macro must include the header 'locale.h'.)
*/
#if !defined(lua_getlocaledecpoint)
#define lua_getlocaledecpoint()		(localeconv()->decimal_point[0])
#endif

/* }================================================================== */

/*
** {==================================================================
** Language Variations
** =====================================================================
*/

/*
@@ LUA_NOCVTN2S/LUA_NOCVTS2N control how Lua performs some
** coercions. Define LUA_NOCVTN2S to turn off automatic coercion from
** numbers to strings. Define LUA_NOCVTS2N to turn off automatic
** coercion from strings to numbers.
*/
/* #define LUA_NOCVTN2S */
/* #define LUA_NOCVTS2N */

/*
@@ LUA_USE_APICHECK turns on several consistency checks on the C API.
** Define it as a help when debugging C code.
*/
#if defined(LUA_USE_APICHECK)
#include <assert.h>
#define luai_apicheck(l,e)	assert(e)
#endif

/* }================================================================== */

/*
** {==================================================================
** Macros that affect the API and must be stable (that is, must be the
** same when you compile Lua and when you compile code that links to
** Lua).
** =====================================================================
*/

/*
@@ LUAI_MAXSTACK limits the size of the Lua stack.
** CHANGE it if you need a different limit. This limit is arbitrary;
** its only purpose is to stop Lua from consuming unlimited stack
** space (and to reserve some numbers for pseudo-indices).
** (It must fit into max(size_t)/32.)
*/
#if LUAI_IS32INT
#define LUAI_MAXSTACK		1000000
#else
#define LUAI_MAXSTACK		15000
#endif

/*
@@ LUA_EXTRASPACE defines the size of a raw memory area associated with
** a Lua state with very fast access.
** CHANGE it if you need a different size.
*/
#define LUA_EXTRASPACE		(sizeof(void *))

/*
@@ LUA_IDSIZE gives the maximum size for the description of the source
@@ of a function in debug information.
** CHANGE it if you want a different size.
*/
#define LUA_IDSIZE	60

/*
@@ LUAL_BUFFERSIZE is the buffer size used by the lauxlib buffer system.
*/
#define LUAL_BUFFERSIZE   ((int)(16 * sizeof(void*) * sizeof(lua_Number)))

/*
@@ LUAI_MAXALIGN defines fields that, when used in a union, ensure
** maximum alignment for the other items in that union.
*/
#define LUAI_MAXALIGN  lua_Number n; double u; void *s; lua_Integer i; long l

/* }================================================================== */

/* =================================================================== */

/*
** Local configuration. You can use this space to add your redefinitions
** without modifying the main part of the file.
*/

#endif

/*** End of inlined file: luaconf.h ***/

#define LUA_VERSION_MAJOR	"5"
#define LUA_VERSION_MINOR	"4"
#define LUA_VERSION_RELEASE	"0"

#define LUA_VERSION_NUM			504
#define LUA_VERSION_RELEASE_NUM		(LUA_VERSION_NUM * 100 + 0)

#define LUA_VERSION	"Lua " LUA_VERSION_MAJOR "." LUA_VERSION_MINOR
#define LUA_RELEASE	LUA_VERSION "." LUA_VERSION_RELEASE
#define LUA_COPYRIGHT	LUA_RELEASE "  Copyright (C) 1994-2020 Lua.org, PUC-Rio"
#define LUA_AUTHORS	"R. Ierusalimschy, L. H. de Figueiredo, W. Celes"

/* mark for precompiled code ('<esc>Lua') */
#define LUA_SIGNATURE	"\x1bLua"

/* option for multiple returns in 'lua_pcall' and 'lua_call' */
#define LUA_MULTRET	(-1)

/*
** Pseudo-indices
** (-LUAI_MAXSTACK is the minimum valid index; we keep some free empty
** space after that to help overflow detection)
*/
#define LUA_REGISTRYINDEX	(-LUAI_MAXSTACK - 1000)
#define lua_upvalueindex(i)	(LUA_REGISTRYINDEX - (i))

/* thread status */
#define LUA_OK		0
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5

typedef struct lua_State lua_State;

/*
** basic types
*/
#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define LUA_NUMTYPES		9

/* minimum Lua stack available to a C function */
#define LUA_MINSTACK	20

/* predefined values in the registry */
#define LUA_RIDX_MAINTHREAD	1
#define LUA_RIDX_GLOBALS	2
#define LUA_RIDX_LAST		LUA_RIDX_GLOBALS

/* type of numbers in Lua */
typedef LUA_NUMBER lua_Number;

/* type for integer functions */
typedef LUA_INTEGER lua_Integer;

/* unsigned integer type */
typedef LUA_UNSIGNED lua_Unsigned;

/* type for continuation-function contexts */
typedef LUA_KCONTEXT lua_KContext;

/*
** Type for C functions registered with Lua
*/
typedef int ( *lua_CFunction )( lua_State *L );

/*
** Type for continuation functions
*/
typedef int ( *lua_KFunction )( lua_State *L, int status, lua_KContext ctx );

/*
** Type for functions that read/write blocks when loading/dumping Lua chunks
*/
typedef const char *( *lua_Reader )( lua_State *L, void *ud, size_t *sz );

typedef int ( *lua_Writer )( lua_State *L, const void *p, size_t sz, void *ud );

/*
** Type for memory-allocation functions
*/
typedef void *( *lua_Alloc )( void *ud, void *ptr, size_t osize, size_t nsize );

/*
** Type for warning functions
*/
typedef void ( *lua_WarnFunction )( void *ud, const char *msg, int tocont );

/*
** generic extra include file
*/
#if defined(LUA_USER_H)
#include LUA_USER_H
#endif

/*
** RCS ident string
*/
extern const char lua_ident[];

/*
** state manipulation
*/
LUA_API lua_State *( lua_newstate )( lua_Alloc f, void *ud );
LUA_API void ( lua_close )( lua_State *L );
LUA_API lua_State *( lua_newthread )( lua_State *L );
LUA_API int ( lua_resetthread )( lua_State *L );

LUA_API lua_CFunction( lua_atpanic )( lua_State *L, lua_CFunction panicf );

LUA_API lua_Number( lua_version )( lua_State *L );

/*
** basic stack manipulation
*/
LUA_API int ( lua_absindex )( lua_State *L, int idx );
LUA_API int ( lua_gettop )( lua_State *L );
LUA_API void ( lua_settop )( lua_State *L, int idx );
LUA_API void ( lua_pushvalue )( lua_State *L, int idx );
LUA_API void ( lua_rotate )( lua_State *L, int idx, int n );
LUA_API void ( lua_copy )( lua_State *L, int fromidx, int toidx );
LUA_API int ( lua_checkstack )( lua_State *L, int n );

LUA_API void ( lua_xmove )( lua_State *from, lua_State *to, int n );

/*
** access functions (stack -> C)
*/

LUA_API int ( lua_isnumber )( lua_State *L, int idx );
LUA_API int ( lua_isstring )( lua_State *L, int idx );
LUA_API int ( lua_iscfunction )( lua_State *L, int idx );
LUA_API int ( lua_isinteger )( lua_State *L, int idx );
LUA_API int ( lua_isuserdata )( lua_State *L, int idx );
LUA_API int ( lua_type )( lua_State *L, int idx );
LUA_API const char     *( lua_typename )( lua_State *L, int tp );

LUA_API lua_Number( lua_tonumberx )( lua_State *L, int idx, int *isnum );
LUA_API lua_Integer( lua_tointegerx )( lua_State *L, int idx, int *isnum );
LUA_API int ( lua_toboolean )( lua_State *L, int idx );
LUA_API const char     *( lua_tolstring )( lua_State *L, int idx, size_t *len );
LUA_API lua_Unsigned( lua_rawlen )( lua_State *L, int idx );
LUA_API lua_CFunction( lua_tocfunction )( lua_State *L, int idx );
LUA_API void	       *( lua_touserdata )( lua_State *L, int idx );
LUA_API lua_State      *( lua_tothread )( lua_State *L, int idx );
LUA_API const void     *( lua_topointer )( lua_State *L, int idx );

/*
** Comparison and arithmetic functions
*/

#define LUA_OPADD	0	/* ORDER TM, ORDER OP */
#define LUA_OPSUB	1
#define LUA_OPMUL	2
#define LUA_OPMOD	3
#define LUA_OPPOW	4
#define LUA_OPDIV	5
#define LUA_OPIDIV	6
#define LUA_OPBAND	7
#define LUA_OPBOR	8
#define LUA_OPBXOR	9
#define LUA_OPSHL	10
#define LUA_OPSHR	11
#define LUA_OPUNM	12
#define LUA_OPBNOT	13

LUA_API void ( lua_arith )( lua_State *L, int op );

#define LUA_OPEQ	0
#define LUA_OPLT	1
#define LUA_OPLE	2

LUA_API int ( lua_rawequal )( lua_State *L, int idx1, int idx2 );
LUA_API int ( lua_compare )( lua_State *L, int idx1, int idx2, int op );

/*
** push functions (C -> stack)
*/
LUA_API void ( lua_pushnil )( lua_State *L );
LUA_API void ( lua_pushnumber )( lua_State *L, lua_Number n );
LUA_API void ( lua_pushinteger )( lua_State *L, lua_Integer n );
LUA_API const char *( lua_pushlstring )( lua_State *L, const char *s, size_t len );
LUA_API const char *( lua_pushstring )( lua_State *L, const char *s );
LUA_API const char *( lua_pushvfstring )( lua_State *L, const char *fmt,
    va_list argp );
LUA_API const char *( lua_pushfstring )( lua_State *L, const char *fmt, ... );
LUA_API void ( lua_pushcclosure )( lua_State *L, lua_CFunction fn, int n );
LUA_API void ( lua_pushboolean )( lua_State *L, int b );
LUA_API void ( lua_pushlightuserdata )( lua_State *L, void *p );
LUA_API int ( lua_pushthread )( lua_State *L );

/*
** get functions (Lua -> stack)
*/
LUA_API int ( lua_getglobal )( lua_State *L, const char *name );
LUA_API int ( lua_gettable )( lua_State *L, int idx );
LUA_API int ( lua_getfield )( lua_State *L, int idx, const char *k );
LUA_API int ( lua_geti )( lua_State *L, int idx, lua_Integer n );
LUA_API int ( lua_rawget )( lua_State *L, int idx );
LUA_API int ( lua_rawgeti )( lua_State *L, int idx, lua_Integer n );
LUA_API int ( lua_rawgetp )( lua_State *L, int idx, const void *p );

LUA_API void ( lua_createtable )( lua_State *L, int narr, int nrec );
LUA_API void *( lua_newuserdatauv )( lua_State *L, size_t sz, int nuvalue );
LUA_API int ( lua_getmetatable )( lua_State *L, int objindex );
LUA_API int ( lua_getiuservalue )( lua_State *L, int idx, int n );

/*
** set functions (stack -> Lua)
*/
LUA_API void ( lua_setglobal )( lua_State *L, const char *name );
LUA_API void ( lua_settable )( lua_State *L, int idx );
LUA_API void ( lua_setfield )( lua_State *L, int idx, const char *k );
LUA_API void ( lua_seti )( lua_State *L, int idx, lua_Integer n );
LUA_API void ( lua_rawset )( lua_State *L, int idx );
LUA_API void ( lua_rawseti )( lua_State *L, int idx, lua_Integer n );
LUA_API void ( lua_rawsetp )( lua_State *L, int idx, const void *p );
LUA_API int ( lua_setmetatable )( lua_State *L, int objindex );
LUA_API int ( lua_setiuservalue )( lua_State *L, int idx, int n );

/*
** 'load' and 'call' functions (load and run Lua code)
*/
LUA_API void ( lua_callk )( lua_State *L, int nargs, int nresults,
                            lua_KContext ctx, lua_KFunction k );
#define lua_call(L,n,r)		lua_callk(L, (n), (r), 0, NULL)

LUA_API int ( lua_pcallk )( lua_State *L, int nargs, int nresults, int errfunc,
                            lua_KContext ctx, lua_KFunction k );
#define lua_pcall(L,n,r,f)	lua_pcallk(L, (n), (r), (f), 0, NULL)

LUA_API int ( lua_load )( lua_State *L, lua_Reader reader, void *dt,
                          const char *chunkname, const char *mode );

LUA_API int ( lua_dump )( lua_State *L, lua_Writer writer, void *data, int strip );

/*
** coroutine functions
*/
LUA_API int ( lua_yieldk )( lua_State *L, int nresults, lua_KContext ctx,
                            lua_KFunction k );
LUA_API int ( lua_resume )( lua_State *L, lua_State *from, int narg,
                            int *nres );
LUA_API int ( lua_status )( lua_State *L );
LUA_API int ( lua_isyieldable )( lua_State *L );

#define lua_yield(L,n)		lua_yieldk(L, (n), 0, NULL)

/*
** Warning-related functions
*/
LUA_API void ( lua_setwarnf )( lua_State *L, lua_WarnFunction f, void *ud );
LUA_API void ( lua_warning )( lua_State *L, const char *msg, int tocont );

/*
** garbage-collection function and options
*/

#define LUA_GCSTOP		0
#define LUA_GCRESTART		1
#define LUA_GCCOLLECT		2
#define LUA_GCCOUNT		3
#define LUA_GCCOUNTB		4
#define LUA_GCSTEP		5
#define LUA_GCSETPAUSE		6
#define LUA_GCSETSTEPMUL	7
#define LUA_GCISRUNNING		9
#define LUA_GCGEN		10
#define LUA_GCINC		11

LUA_API int ( lua_gc )( lua_State *L, int what, ... );

/*
** miscellaneous functions
*/

LUA_API int ( lua_error )( lua_State *L );

LUA_API int ( lua_next )( lua_State *L, int idx );

LUA_API void ( lua_concat )( lua_State *L, int n );
LUA_API void ( lua_len )( lua_State *L, int idx );

LUA_API size_t ( lua_stringtonumber )( lua_State *L, const char *s );

LUA_API lua_Alloc( lua_getallocf )( lua_State *L, void **ud );
LUA_API void ( lua_setallocf )( lua_State *L, lua_Alloc f, void *ud );

LUA_API void ( lua_toclose )( lua_State *L, int idx );

/*
** {==============================================================
** some useful macros
** ===============================================================
*/

#define lua_getextraspace(L)	((void *)((char *)(L) - LUA_EXTRASPACE))

#define lua_tonumber(L,i)	lua_tonumberx(L,(i),NULL)
#define lua_tointeger(L,i)	lua_tointegerx(L,(i),NULL)

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_newtable(L)		lua_createtable(L, 0, 0)

#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)

#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)

#define lua_pushliteral(L, s)	lua_pushstring(L, "" s)

#define lua_pushglobaltable(L)  \
  ((void)lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS))

#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)

#define lua_insert(L,idx)	lua_rotate(L, (idx), 1)

#define lua_remove(L,idx)	(lua_rotate(L, (idx), -1), lua_pop(L, 1))

#define lua_replace(L,idx)	(lua_copy(L, -1, (idx)), lua_pop(L, 1))

/* }============================================================== */

/*
** {==============================================================
** compatibility macros
** ===============================================================
*/
#if defined(LUA_COMPAT_APIINTCASTS)

#define lua_pushunsigned(L,n)	lua_pushinteger(L, (lua_Integer)(n))
#define lua_tounsignedx(L,i,is)	((lua_Unsigned)lua_tointegerx(L,i,is))
#define lua_tounsigned(L,i)	lua_tounsignedx(L,(i),NULL)

#endif

#define lua_newuserdata(L,s)	lua_newuserdatauv(L,s,1)
#define lua_getuservalue(L,idx)	lua_getiuservalue(L,idx,1)
#define lua_setuservalue(L,idx)	lua_setiuservalue(L,idx,1)

#define LUA_NUMTAGS		LUA_NUMTYPES

/* }============================================================== */

/*
** {======================================================================
** Debug API
** =======================================================================
*/

/*
** Event codes
*/
#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILCALL 4

/*
** Event masks
*/
#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

typedef struct lua_Debug lua_Debug;  /* activation record */

/* Functions to be called by the debugger in specific events */
typedef void ( *lua_Hook )( lua_State *L, lua_Debug *ar );

LUA_API int ( lua_getstack )( lua_State *L, int level, lua_Debug *ar );
LUA_API int ( lua_getinfo )( lua_State *L, const char *what, lua_Debug *ar );
LUA_API const char *( lua_getlocal )( lua_State *L, const lua_Debug *ar, int n );
LUA_API const char *( lua_setlocal )( lua_State *L, const lua_Debug *ar, int n );
LUA_API const char *( lua_getupvalue )( lua_State *L, int funcindex, int n );
LUA_API const char *( lua_setupvalue )( lua_State *L, int funcindex, int n );

LUA_API void *( lua_upvalueid )( lua_State *L, int fidx, int n );
LUA_API void ( lua_upvaluejoin )( lua_State *L, int fidx1, int n1,
                                  int fidx2, int n2 );

LUA_API void ( lua_sethook )( lua_State *L, lua_Hook func, int mask, int count );
LUA_API lua_Hook( lua_gethook )( lua_State *L );
LUA_API int ( lua_gethookmask )( lua_State *L );
LUA_API int ( lua_gethookcount )( lua_State *L );

LUA_API int ( lua_setcstacklimit )( lua_State *L, unsigned int limit );

struct lua_Debug {
  int event;
  const char *name;	/* (n) */
  const char *namewhat;	/* (n) 'global', 'local', 'field', 'method' */
  const char *what;	/* (S) 'Lua', 'C', 'main', 'tail' */
  const char *source;	/* (S) */
  size_t srclen;	/* (S) */
  int currentline;	/* (l) */
  int linedefined;	/* (S) */
  int lastlinedefined;	/* (S) */
  unsigned char nups;	/* (u) number of upvalues */
  unsigned char nparams;/* (u) number of parameters */
  char isvararg;        /* (u) */
  char istailcall;	/* (t) */
  unsigned short ftransfer;   /* (r) index of first value transferred */
  unsigned short ntransfer;   /* (r) number of transferred values */
  char short_src[LUA_IDSIZE]; /* (S) */
  /* private part */
  struct CallInfo *i_ci;  /* active function */
};

/* }====================================================================== */

/******************************************************************************
* Copyright (C) 1994-2020 Lua.org, PUC-Rio.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#endif

/*** End of inlined file: lua.h ***/


/*** Start of inlined file: lapi.h ***/
#ifndef lapi_h
#define lapi_h


/*** Start of inlined file: llimits.h ***/
#ifndef llimits_h
#define llimits_h

#include <limits.h>
#include <stddef.h>

/*
** 'lu_mem' and 'l_mem' are unsigned/signed integers big enough to count
** the total memory used by Lua (in bytes). Usually, 'size_t' and
** 'ptrdiff_t' should work, but we use 'long' for 16-bit machines.
*/
#if defined(LUAI_MEM)		/* { external definitions? */
typedef LUAI_UMEM lu_mem;
typedef LUAI_MEM l_mem;
#elif LUAI_IS32INT	/* }{ */
typedef size_t lu_mem;
typedef ptrdiff_t l_mem;
#else  /* 16-bit ints */	/* }{ */
typedef unsigned long lu_mem;
typedef long l_mem;
#endif				/* } */

/* chars used as small naturals (so that 'char' is reserved for characters) */
typedef unsigned char lu_byte;
typedef signed char ls_byte;

/* maximum value for size_t */
#define MAX_SIZET	((size_t)(~(size_t)0))

/* maximum size visible for Lua (must be representable in a lua_Integer) */
#define MAX_SIZE	(sizeof(size_t) < sizeof(lua_Integer) ? MAX_SIZET \
                   : (size_t)(LUA_MAXINTEGER))

#define MAX_LUMEM	((lu_mem)(~(lu_mem)0))

#define MAX_LMEM	((l_mem)(MAX_LUMEM >> 1))

#define MAX_INT		INT_MAX  /* maximum value of an int */

/*
** floor of the log2 of the maximum signed value for integral type 't'.
** (That is, maximum 'n' such that '2^n' fits in the given signed type.)
*/
#define log2maxs(t)	(sizeof(t) * 8 - 2)

/*
** test whether an unsigned value is a power of 2 (or zero)
*/
#define ispow2(x)	(((x) & ((x) - 1)) == 0)

/* number of chars of a literal string without the ending \0 */
#define LL(x)   (sizeof(x)/sizeof(char) - 1)

/*
** conversion of pointer to unsigned integer:
** this is for hashing only; there is no problem if the integer
** cannot hold the whole pointer value
*/
#define point2uint(p)	((unsigned int)((size_t)(p) & UINT_MAX))

/* types of 'usual argument conversions' for lua_Number and lua_Integer */
typedef LUAI_UACNUMBER l_uacNumber;
typedef LUAI_UACINT l_uacInt;

/* internal assertions for in-house debugging */
#if defined(lua_assert)
#define check_exp(c,e)		(lua_assert(c), (e))
/* to avoid problems with conditions too long */
#define lua_longassert(c)	((c) ? (void)0 : lua_assert(0))
#else
#define lua_assert(c)		((void)0)
#define check_exp(c,e)		(e)
#define lua_longassert(c)	((void)0)
#endif

/*
** assertion for checking API calls
*/
#if !defined(luai_apicheck)
#define luai_apicheck(l,e)	((void)l, lua_assert(e))
#endif

#define api_check(l,e,msg)	luai_apicheck(l,(e) && msg)

/* macro to avoid warnings about unused variables */
#if !defined(UNUSED)
#define UNUSED(x)	((void)(x))
#endif

/* type casts (a macro highlights casts in the code) */
#define cast(t, exp)	((t)(exp))

#define cast_void(i)	cast(void, (i))
#define cast_voidp(i)	cast(void *, (i))
#define cast_num(i)	cast(lua_Number, (i))
#define cast_int(i)	cast(int, (i))
#define cast_uint(i)	cast(unsigned int, (i))
#define cast_byte(i)	cast(lu_byte, (i))
#define cast_uchar(i)	cast(unsigned char, (i))
#define cast_char(i)	cast(char, (i))
#define cast_charp(i)	cast(char *, (i))
#define cast_sizet(i)	cast(size_t, (i))

/* cast a signed lua_Integer to lua_Unsigned */
#if !defined(l_castS2U)
#define l_castS2U(i)	((lua_Unsigned)(i))
#endif

/*
** cast a lua_Unsigned to a signed lua_Integer; this cast is
** not strict ISO C, but two-complement architectures should
** work fine.
*/
#if !defined(l_castU2S)
#define l_castU2S(i)	((lua_Integer)(i))
#endif

/*
** macros to improve jump prediction (used mainly for error handling)
*/
#if !defined(likely)

#if defined(__GNUC__)
#define likely(x)	(__builtin_expect(((x) != 0), 1))
#define unlikely(x)	(__builtin_expect(((x) != 0), 0))
#else
#define likely(x)	(x)
#define unlikely(x)	(x)
#endif

#endif

/*
** non-return type
*/
#if !defined(l_noret)

#if defined(__GNUC__)
#define l_noret		void __attribute__((noreturn))
#elif defined(_MSC_VER) && _MSC_VER >= 1200
#define l_noret		void __declspec(noreturn)
#else
#define l_noret		void
#endif

#endif

/*
** type for virtual-machine instructions;
** must be an unsigned with (at least) 4 bytes (see details in lopcodes.h)
*/
#if LUAI_IS32INT
typedef unsigned int l_uint32;
#else
typedef unsigned long l_uint32;
#endif

typedef l_uint32 Instruction;

/*
** Maximum length for short strings, that is, strings that are
** internalized. (Cannot be smaller than reserved words or tags for
** metamethods, as these strings must be internalized;
** #("function") = 8, #("__newindex") = 10.)
*/
#if !defined(LUAI_MAXSHORTLEN)
#define LUAI_MAXSHORTLEN	40
#endif

/*
** Initial size for the string table (must be power of 2).
** The Lua core alone registers ~50 strings (reserved words +
** metaevent keys + a few others). Libraries would typically add
** a few dozens more.
*/
#if !defined(MINSTRTABSIZE)
#define MINSTRTABSIZE	128
#endif

/*
** Size of cache for strings in the API. 'N' is the number of
** sets (better be a prime) and "M" is the size of each set (M == 1
** makes a direct cache.)
*/
#if !defined(STRCACHE_N)
#define STRCACHE_N		53
#define STRCACHE_M		2
#endif

/* minimum size for string buffer */
#if !defined(LUA_MINBUFFER)
#define LUA_MINBUFFER	32
#endif

/*
** macros that are executed whenever program enters the Lua core
** ('lua_lock') and leaves the core ('lua_unlock')
*/
#if !defined(lua_lock)
#define lua_lock(L)	((void) 0)
#define lua_unlock(L)	((void) 0)
#endif

/*
** macro executed during Lua functions at points where the
** function can yield.
*/
#if !defined(luai_threadyield)
#define luai_threadyield(L)	{lua_unlock(L); lua_lock(L);}
#endif

/*
** these macros allow user-specific actions when a thread is
** created/deleted/resumed/yielded.
*/
#if !defined(luai_userstateopen)
#define luai_userstateopen(L)		((void)L)
#endif

#if !defined(luai_userstateclose)
#define luai_userstateclose(L)		((void)L)
#endif

#if !defined(luai_userstatethread)
#define luai_userstatethread(L,L1)	((void)L)
#endif

#if !defined(luai_userstatefree)
#define luai_userstatefree(L,L1)	((void)L)
#endif

#if !defined(luai_userstateresume)
#define luai_userstateresume(L,n)	((void)L)
#endif

#if !defined(luai_userstateyield)
#define luai_userstateyield(L,n)	((void)L)
#endif

/*
** The luai_num* macros define the primitive operations over numbers.
*/

/* floor division (defined as 'floor(a/b)') */
#if !defined(luai_numidiv)
#define luai_numidiv(L,a,b)     ((void)L, l_floor(luai_numdiv(L,a,b)))
#endif

/* float division */
#if !defined(luai_numdiv)
#define luai_numdiv(L,a,b)      ((a)/(b))
#endif

/*
** modulo: defined as 'a - floor(a/b)*b'; the direct computation
** using this definition has several problems with rounding errors,
** so it is better to use 'fmod'. 'fmod' gives the result of
** 'a - trunc(a/b)*b', and therefore must be corrected when
** 'trunc(a/b) ~= floor(a/b)'. That happens when the division has a
** non-integer negative result: non-integer result is equivalent to
** a non-zero remainder 'm'; negative result is equivalent to 'a' and
** 'b' with different signs, or 'm' and 'b' with different signs
** (as the result 'm' of 'fmod' has the same sign of 'a').
*/
#if !defined(luai_nummod)
#define luai_nummod(L,a,b,m)  \
  { (void)L; (m) = l_mathop(fmod)(a,b); \
    if (((m) > 0) ? (b) < 0 : ((m) < 0 && (b) > 0)) (m) += (b); }
#endif

/* exponentiation */
#if !defined(luai_numpow)
#define luai_numpow(L,a,b)      ((void)L, l_mathop(pow)(a,b))
#endif

/* the others are quite standard operations */
#if !defined(luai_numadd)
#define luai_numadd(L,a,b)      ((a)+(b))
#define luai_numsub(L,a,b)      ((a)-(b))
#define luai_nummul(L,a,b)      ((a)*(b))
#define luai_numunm(L,a)        (-(a))
#define luai_numeq(a,b)         ((a)==(b))
#define luai_numlt(a,b)         ((a)<(b))
#define luai_numle(a,b)         ((a)<=(b))
#define luai_numgt(a,b)         ((a)>(b))
#define luai_numge(a,b)         ((a)>=(b))
#define luai_numisnan(a)        (!luai_numeq((a), (a)))
#endif

/*
** macro to control inclusion of some hard tests on stack reallocation
*/
#if !defined(HARDSTACKTESTS)
#define condmovestack(L,pre,pos)	((void)0)
#else
/* realloc stack keeping its size */
#define condmovestack(L,pre,pos)  \
  { int sz_ = (L)->stacksize; pre; luaD_reallocstack((L), sz_, 0); pos; }
#endif

#if !defined(HARDMEMTESTS)
#define condchangemem(L,pre,pos)	((void)0)
#else
#define condchangemem(L,pre,pos)  \
  { if (G(L)->gcrunning) { pre; luaC_fullgc(L, 0); pos; } }
#endif

#endif

/*** End of inlined file: llimits.h ***/


/*** Start of inlined file: lstate.h ***/
#ifndef lstate_h
#define lstate_h


/*** Start of inlined file: lobject.h ***/
#ifndef lobject_h
#define lobject_h

#include <stdarg.h>

/*
** Extra types for collectable non-values
*/
#define LUA_TUPVAL	LUA_NUMTYPES  /* upvalues */
#define LUA_TPROTO	(LUA_NUMTYPES+1)  /* function prototypes */

/*
** number of all possible types (including LUA_TNONE)
*/
#define LUA_TOTALTYPES		(LUA_TPROTO + 2)

/*
** tags for Tagged Values have the following use of bits:
** bits 0-3: actual tag (a LUA_T* constant)
** bits 4-5: variant bits
** bit 6: whether value is collectable
*/

/* add variant bits to a type */
#define makevariant(t,v)	((t) | ((v) << 4))

/*
** Union of all Lua values
*/
typedef union Value {
  struct GCObject *gc;    /* collectable objects */
  void *p;         /* light userdata */
  lua_CFunction f; /* light C functions */
  lua_Integer i;   /* integer numbers */
  lua_Number n;    /* float numbers */
} Value;

/*
** Tagged Values. This is the basic representation of values in Lua:
** an actual value plus a tag with its type.
*/

#define TValuefields	Value value_; lu_byte tt_

typedef struct TValue {
  TValuefields;
} TValue;

#define val_(o)		((o)->value_)
#define valraw(o)	(&val_(o))

/* raw type tag of a TValue */
#define rawtt(o)	((o)->tt_)

/* tag with no variants (bits 0-3) */
#define novariant(t)	((t) & 0x0F)

/* type tag of a TValue (bits 0-3 for tags + variant bits 4-5) */
#define withvariant(t)	((t) & 0x3F)
#define ttypetag(o)	withvariant(rawtt(o))

/* type of a TValue */
#define ttype(o)	(novariant(rawtt(o)))

/* Macros to test type */
#define checktag(o,t)		(rawtt(o) == (t))
#define checktype(o,t)		(ttype(o) == (t))

/* Macros for internal tests */

/* collectable object has the same tag as the original value */
#define righttt(obj)		(ttypetag(obj) == gcvalue(obj)->tt)

/*
** Any value being manipulated by the program either is non
** collectable, or the collectable object has the right tag
** and it is not dead.
*/
#define checkliveness(L,obj) \
  ((void)L, lua_longassert(!iscollectable(obj) || \
                           (righttt(obj) && (L == NULL || !isdead(G(L),gcvalue(obj))))))

/* Macros to set values */

/* set a value's tag */
#define settt_(o,t)	((o)->tt_=(t))

/* main macro to copy values (from 'obj1' to 'obj2') */
#define setobj(L,obj1,obj2) \
  { TValue *io1=(obj1); const TValue *io2=(obj2); \
    io1->value_ = io2->value_; settt_(io1, io2->tt_); \
    checkliveness(L,io1); lua_assert(!isnonstrictnil(io1)); }

/*
** Different types of assignments, according to source and destination.
** (They are mostly equal now, but may be different in the future.)
*/

/* from stack to stack */
#define setobjs2s(L,o1,o2)	setobj(L,s2v(o1),s2v(o2))
/* to stack (not from same stack) */
#define setobj2s(L,o1,o2)	setobj(L,s2v(o1),o2)
/* from table to same table */
#define setobjt2t	setobj
/* to new object */
#define setobj2n	setobj
/* to table */
#define setobj2t	setobj

/*
** Entries in the Lua stack
*/
typedef union StackValue {
  TValue val;
} StackValue;

/* index to stack elements */
typedef StackValue *StkId;

/* convert a 'StackValue' to a 'TValue' */
#define s2v(o)	(&(o)->val)

/*
** {==================================================================
** Nil
** ===================================================================
*/

/* Standard nil */
#define LUA_VNIL	makevariant(LUA_TNIL, 0)

/* Empty slot (which might be different from a slot containing nil) */
#define LUA_VEMPTY	makevariant(LUA_TNIL, 1)

/* Value returned for a key not found in a table (absent key) */
#define LUA_VABSTKEY	makevariant(LUA_TNIL, 2)

/* macro to test for (any kind of) nil */
#define ttisnil(v)		checktype((v), LUA_TNIL)

/* macro to test for a standard nil */
#define ttisstrictnil(o)	checktag((o), LUA_VNIL)

#define setnilvalue(obj) settt_(obj, LUA_VNIL)

#define isabstkey(v)		checktag((v), LUA_VABSTKEY)

/*
** macro to detect non-standard nils (used only in assertions)
*/
#define isnonstrictnil(v)	(ttisnil(v) && !ttisstrictnil(v))

/*
** By default, entries with any kind of nil are considered empty.
** (In any definition, values associated with absent keys must also
** be accepted as empty.)
*/
#define isempty(v)		ttisnil(v)

/* macro defining a value corresponding to an absent key */
#define ABSTKEYCONSTANT		{NULL}, LUA_VABSTKEY

/* mark an entry as empty */
#define setempty(v)		settt_(v, LUA_VEMPTY)

/* }================================================================== */

/*
** {==================================================================
** Booleans
** ===================================================================
*/

#define LUA_VFALSE	makevariant(LUA_TBOOLEAN, 0)
#define LUA_VTRUE	makevariant(LUA_TBOOLEAN, 1)

#define ttisboolean(o)		checktype((o), LUA_TBOOLEAN)
#define ttisfalse(o)		checktag((o), LUA_VFALSE)
#define ttistrue(o)		checktag((o), LUA_VTRUE)

#define l_isfalse(o)	(ttisfalse(o) || ttisnil(o))

#define setbfvalue(obj)		settt_(obj, LUA_VFALSE)
#define setbtvalue(obj)		settt_(obj, LUA_VTRUE)

/* }================================================================== */

/*
** {==================================================================
** Threads
** ===================================================================
*/

#define LUA_VTHREAD		makevariant(LUA_TTHREAD, 0)

#define ttisthread(o)		checktag((o), ctb(LUA_VTHREAD))

#define thvalue(o)	check_exp(ttisthread(o), gco2th(val_(o).gc))

#define setthvalue(L,obj,x) \
  { TValue *io = (obj); lua_State *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_VTHREAD)); \
    checkliveness(L,io); }

#define setthvalue2s(L,o,t)	setthvalue(L,s2v(o),t)

/* }================================================================== */

/*
** {==================================================================
** Collectable Objects
** ===================================================================
*/

/*
** Common Header for all collectable objects (in macro form, to be
** included in other objects)
*/
#define CommonHeader	struct GCObject *next; lu_byte tt; lu_byte marked

/* Common type for all collectable objects */
typedef struct GCObject {
  CommonHeader;
} GCObject;

/* Bit mark for collectable types */
#define BIT_ISCOLLECTABLE	(1 << 6)

#define iscollectable(o)	(rawtt(o) & BIT_ISCOLLECTABLE)

/* mark a tag as collectable */
#define ctb(t)			((t) | BIT_ISCOLLECTABLE)

#define gcvalue(o)	check_exp(iscollectable(o), val_(o).gc)

#define gcvalueraw(v)	((v).gc)

#define setgcovalue(L,obj,x) \
  { TValue *io = (obj); GCObject *i_g=(x); \
    val_(io).gc = i_g; settt_(io, ctb(i_g->tt)); }

/* }================================================================== */

/*
** {==================================================================
** Numbers
** ===================================================================
*/

/* Variant tags for numbers */
#define LUA_VNUMINT	makevariant(LUA_TNUMBER, 0)  /* integer numbers */
#define LUA_VNUMFLT	makevariant(LUA_TNUMBER, 1)  /* float numbers */

#define ttisnumber(o)		checktype((o), LUA_TNUMBER)
#define ttisfloat(o)		checktag((o), LUA_VNUMFLT)
#define ttisinteger(o)		checktag((o), LUA_VNUMINT)

#define nvalue(o)	check_exp(ttisnumber(o), \
                            (ttisinteger(o) ? cast_num(ivalue(o)) : fltvalue(o)))
#define fltvalue(o)	check_exp(ttisfloat(o), val_(o).n)
#define ivalue(o)	check_exp(ttisinteger(o), val_(o).i)

#define fltvalueraw(v)	((v).n)
#define ivalueraw(v)	((v).i)

#define setfltvalue(obj,x) \
  { TValue *io=(obj); val_(io).n=(x); settt_(io, LUA_VNUMFLT); }

#define chgfltvalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisfloat(io)); val_(io).n=(x); }

#define setivalue(obj,x) \
  { TValue *io=(obj); val_(io).i=(x); settt_(io, LUA_VNUMINT); }

#define chgivalue(obj,x) \
  { TValue *io=(obj); lua_assert(ttisinteger(io)); val_(io).i=(x); }

/* }================================================================== */

/*
** {==================================================================
** Strings
** ===================================================================
*/

/* Variant tags for strings */
#define LUA_VSHRSTR	makevariant(LUA_TSTRING, 0)  /* short strings */
#define LUA_VLNGSTR	makevariant(LUA_TSTRING, 1)  /* long strings */

#define ttisstring(o)		checktype((o), LUA_TSTRING)
#define ttisshrstring(o)	checktag((o), ctb(LUA_VSHRSTR))
#define ttislngstring(o)	checktag((o), ctb(LUA_VLNGSTR))

#define tsvalueraw(v)	(gco2ts((v).gc))

#define tsvalue(o)	check_exp(ttisstring(o), gco2ts(val_(o).gc))

#define setsvalue(L,obj,x) \
  { TValue *io = (obj); TString *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(x_->tt)); \
    checkliveness(L,io); }

/* set a string to the stack */
#define setsvalue2s(L,o,s)	setsvalue(L,s2v(o),s)

/* set a string to a new object */
#define setsvalue2n	setsvalue

/*
** Header for a string value.
*/
typedef struct TString {
  CommonHeader;
  lu_byte extra;  /* reserved words for short strings; "has hash" for longs */
  lu_byte shrlen;  /* length for short strings */
  unsigned int hash;
  union {
    size_t lnglen;  /* length for long strings */
    struct TString *hnext;  /* linked list for hash table */
  } u;
  char contents[1];
} TString;

/*
** Get the actual string (array of bytes) from a 'TString'.
*/
#define getstr(ts)  ((ts)->contents)

/* get the actual string (array of bytes) from a Lua value */
#define svalue(o)       getstr(tsvalue(o))

/* get string length from 'TString *s' */
#define tsslen(s)	((s)->tt == LUA_VSHRSTR ? (s)->shrlen : (s)->u.lnglen)

/* get string length from 'TValue *o' */
#define vslen(o)	tsslen(tsvalue(o))

/* }================================================================== */

/*
** {==================================================================
** Userdata
** ===================================================================
*/

/*
** Light userdata should be a variant of userdata, but for compatibility
** reasons they are also different types.
*/
#define LUA_VLIGHTUSERDATA	makevariant(LUA_TLIGHTUSERDATA, 0)

#define LUA_VUSERDATA		makevariant(LUA_TUSERDATA, 0)

#define ttislightuserdata(o)	checktag((o), LUA_VLIGHTUSERDATA)
#define ttisfulluserdata(o)	checktag((o), ctb(LUA_VUSERDATA))

#define pvalue(o)	check_exp(ttislightuserdata(o), val_(o).p)
#define uvalue(o)	check_exp(ttisfulluserdata(o), gco2u(val_(o).gc))

#define pvalueraw(v)	((v).p)

#define setpvalue(obj,x) \
  { TValue *io=(obj); val_(io).p=(x); settt_(io, LUA_VLIGHTUSERDATA); }

#define setuvalue(L,obj,x) \
  { TValue *io = (obj); Udata *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_VUSERDATA)); \
    checkliveness(L,io); }

/* Ensures that addresses after this type are always fully aligned. */
typedef union UValue {
  TValue uv;
  LUAI_MAXALIGN;  /* ensures maximum alignment for udata bytes */
} UValue;

/*
** Header for userdata with user values;
** memory area follows the end of this structure.
*/
typedef struct Udata {
  CommonHeader;
  unsigned short nuvalue;  /* number of user values */
  size_t len;  /* number of bytes */
  struct Table *metatable;
  GCObject *gclist;
  UValue uv[1];  /* user values */
} Udata;

/*
** Header for userdata with no user values. These userdata do not need
** to be gray during GC, and therefore do not need a 'gclist' field.
** To simplify, the code always use 'Udata' for both kinds of userdata,
** making sure it never accesses 'gclist' on userdata with no user values.
** This structure here is used only to compute the correct size for
** this representation. (The 'bindata' field in its end ensures correct
** alignment for binary data following this header.)
*/
typedef struct Udata0 {
  CommonHeader;
  unsigned short nuvalue;  /* number of user values */
  size_t len;  /* number of bytes */
  struct Table *metatable;
  union {
    LUAI_MAXALIGN;
  } bindata;
} Udata0;

/* compute the offset of the memory area of a userdata */
#define udatamemoffset(nuv) \
  ((nuv) == 0 ? offsetof(Udata0, bindata)  \
   : offsetof(Udata, uv) + (sizeof(UValue) * (nuv)))

/* get the address of the memory block inside 'Udata' */
#define getudatamem(u)	(cast_charp(u) + udatamemoffset((u)->nuvalue))

/* compute the size of a userdata */
#define sizeudata(nuv,nb)	(udatamemoffset(nuv) + (nb))

/* }================================================================== */

/*
** {==================================================================
** Prototypes
** ===================================================================
*/

#define LUA_VPROTO	makevariant(LUA_TPROTO, 0)

/*
** Description of an upvalue for function prototypes
*/
typedef struct Upvaldesc {
  TString *name;  /* upvalue name (for debug information) */
  lu_byte instack;  /* whether it is in stack (register) */
  lu_byte idx;  /* index of upvalue (in stack or in outer function's list) */
  lu_byte kind;  /* kind of corresponding variable */
} Upvaldesc;

/*
** Description of a local variable for function prototypes
** (used for debug information)
*/
typedef struct LocVar {
  TString *varname;
  int startpc;  /* first point where variable is active */
  int endpc;    /* first point where variable is dead */
} LocVar;

/*
** Associates the absolute line source for a given instruction ('pc').
** The array 'lineinfo' gives, for each instruction, the difference in
** lines from the previous instruction. When that difference does not
** fit into a byte, Lua saves the absolute line for that instruction.
** (Lua also saves the absolute line periodically, to speed up the
** computation of a line number: we can use binary search in the
** absolute-line array, but we must traverse the 'lineinfo' array
** linearly to compute a line.)
*/
typedef struct AbsLineInfo {
  int pc;
  int line;
} AbsLineInfo;

/*
** Function Prototypes
*/
typedef struct Proto {
  CommonHeader;
  lu_byte numparams;  /* number of fixed (named) parameters */
  lu_byte is_vararg;
  lu_byte maxstacksize;  /* number of registers needed by this function */
  int sizeupvalues;  /* size of 'upvalues' */
  int sizek;  /* size of 'k' */
  int sizecode;
  int sizelineinfo;
  int sizep;  /* size of 'p' */
  int sizelocvars;
  int sizeabslineinfo;  /* size of 'abslineinfo' */
  int linedefined;  /* debug information  */
  int lastlinedefined;  /* debug information  */
  TValue *k;  /* constants used by the function */
  Instruction *code;  /* opcodes */
  struct Proto **p;  /* functions defined inside the function */
  Upvaldesc *upvalues;  /* upvalue information */
  ls_byte *lineinfo;  /* information about source lines (debug information) */
  AbsLineInfo *abslineinfo;  /* idem */
  LocVar *locvars;  /* information about local variables (debug information) */
  TString  *source;  /* used for debug information */
  GCObject *gclist;
} Proto;

/* }================================================================== */

/*
** {==================================================================
** Closures
** ===================================================================
*/

#define LUA_VUPVAL	makevariant(LUA_TUPVAL, 0)

/* Variant tags for functions */
#define LUA_VLCL	makevariant(LUA_TFUNCTION, 0)  /* Lua closure */
#define LUA_VLCF	makevariant(LUA_TFUNCTION, 1)  /* light C function */
#define LUA_VCCL	makevariant(LUA_TFUNCTION, 2)  /* C closure */

#define ttisfunction(o)		checktype(o, LUA_TFUNCTION)
#define ttisclosure(o)		((rawtt(o) & 0x1F) == LUA_VLCL)
#define ttisLclosure(o)		checktag((o), ctb(LUA_VLCL))
#define ttislcf(o)		checktag((o), LUA_VLCF)
#define ttisCclosure(o)		checktag((o), ctb(LUA_VCCL))

#define isLfunction(o)	ttisLclosure(o)

#define clvalue(o)	check_exp(ttisclosure(o), gco2cl(val_(o).gc))
#define clLvalue(o)	check_exp(ttisLclosure(o), gco2lcl(val_(o).gc))
#define fvalue(o)	check_exp(ttislcf(o), val_(o).f)
#define clCvalue(o)	check_exp(ttisCclosure(o), gco2ccl(val_(o).gc))

#define fvalueraw(v)	((v).f)

#define setclLvalue(L,obj,x) \
  { TValue *io = (obj); LClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_VLCL)); \
    checkliveness(L,io); }

#define setclLvalue2s(L,o,cl)	setclLvalue(L,s2v(o),cl)

#define setfvalue(obj,x) \
  { TValue *io=(obj); val_(io).f=(x); settt_(io, LUA_VLCF); }

#define setclCvalue(L,obj,x) \
  { TValue *io = (obj); CClosure *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_VCCL)); \
    checkliveness(L,io); }

/*
** Upvalues for Lua closures
*/
typedef struct UpVal {
  CommonHeader;
  lu_byte tbc;  /* true if it represents a to-be-closed variable */
  TValue *v;  /* points to stack or to its own value */
  union {
    struct {  /* (when open) */
      struct UpVal *next;  /* linked list */
      struct UpVal **previous;
    } open;
    TValue value;  /* the value (when closed) */
  } u;
} UpVal;

#define ClosureHeader \
  CommonHeader; lu_byte nupvalues; GCObject *gclist

typedef struct CClosure {
  ClosureHeader;
  lua_CFunction f;
  TValue upvalue[1];  /* list of upvalues */
} CClosure;

typedef struct LClosure {
  ClosureHeader;
  struct Proto *p;
  UpVal *upvals[1];  /* list of upvalues */
} LClosure;

typedef union Closure {
  CClosure c;
  LClosure l;
} Closure;

#define getproto(o)	(clLvalue(o)->p)

/* }================================================================== */

/*
** {==================================================================
** Tables
** ===================================================================
*/

#define LUA_VTABLE	makevariant(LUA_TTABLE, 0)

#define ttistable(o)		checktag((o), ctb(LUA_VTABLE))

#define hvalue(o)	check_exp(ttistable(o), gco2t(val_(o).gc))

#define sethvalue(L,obj,x) \
  { TValue *io = (obj); Table *x_ = (x); \
    val_(io).gc = obj2gco(x_); settt_(io, ctb(LUA_VTABLE)); \
    checkliveness(L,io); }

#define sethvalue2s(L,o,h)	sethvalue(L,s2v(o),h)

/*
** Nodes for Hash tables: A pack of two TValue's (key-value pairs)
** plus a 'next' field to link colliding entries. The distribution
** of the key's fields ('key_tt' and 'key_val') not forming a proper
** 'TValue' allows for a smaller size for 'Node' both in 4-byte
** and 8-byte alignments.
*/
typedef union Node {
  struct NodeKey {
    TValuefields;  /* fields for value */
    lu_byte key_tt;  /* key type */
    int next;  /* for chaining */
    Value key_val;  /* key value */
  } u;
  TValue i_val;  /* direct access to node's value as a proper 'TValue' */
} Node;

/* copy a value into a key */
#define setnodekey(L,node,obj) \
  { Node *n_=(node); const TValue *io_=(obj); \
    n_->u.key_val = io_->value_; n_->u.key_tt = io_->tt_; \
    checkliveness(L,io_); }

/* copy a value from a key */
#define getnodekey(L,obj,node) \
  { TValue *io_=(obj); const Node *n_=(node); \
    io_->value_ = n_->u.key_val; io_->tt_ = n_->u.key_tt; \
    checkliveness(L,io_); }

/*
** About 'alimit': if 'isrealasize(t)' is true, then 'alimit' is the
** real size of 'array'. Otherwise, the real size of 'array' is the
** smallest power of two not smaller than 'alimit' (or zero iff 'alimit'
** is zero); 'alimit' is then used as a hint for #t.
*/

#define BITRAS		(1 << 7)
#define isrealasize(t)		(!((t)->marked & BITRAS))
#define setrealasize(t)		((t)->marked &= cast_byte(~BITRAS))
#define setnorealasize(t)	((t)->marked |= BITRAS)

typedef struct Table {
  CommonHeader;
  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
  lu_byte lsizenode;  /* log2 of size of 'node' array */
  unsigned int alimit;  /* "limit" of 'array' array */
  TValue *array;  /* array part */
  Node *node;
  Node *lastfree;  /* any free position is before this position */
  struct Table *metatable;
  GCObject *gclist;
} Table;

/*
** Macros to manipulate keys inserted in nodes
*/
#define keytt(node)		((node)->u.key_tt)
#define keyval(node)		((node)->u.key_val)

#define keyisnil(node)		(keytt(node) == LUA_TNIL)
#define keyisinteger(node)	(keytt(node) == LUA_VNUMINT)
#define keyival(node)		(keyval(node).i)
#define keyisshrstr(node)	(keytt(node) == ctb(LUA_VSHRSTR))
#define keystrval(node)		(gco2ts(keyval(node).gc))

#define setnilkey(node)		(keytt(node) = LUA_TNIL)

#define keyiscollectable(n)	(keytt(n) & BIT_ISCOLLECTABLE)

#define gckey(n)	(keyval(n).gc)
#define gckeyN(n)	(keyiscollectable(n) ? gckey(n) : NULL)

/*
** Use a "nil table" to mark dead keys in a table. Those keys serve
** to keep space for removed entries, which may still be part of
** chains. Note that the 'keytt' does not have the BIT_ISCOLLECTABLE
** set, so these values are considered not collectable and are different
** from any valid value.
*/
#define setdeadkey(n)	(keytt(n) = LUA_TTABLE, gckey(n) = NULL)

/* }================================================================== */

/*
** 'module' operation for hashing (size is always a power of 2)
*/
#define lmod(s,size) \
  (check_exp((size&(size-1))==0, (cast_int((s) & ((size)-1)))))

#define twoto(x)	(1<<(x))
#define sizenode(t)	(twoto((t)->lsizenode))

/* size of buffer for 'luaO_utf8esc' function */
#define UTF8BUFFSZ	8

LUAI_FUNC int luaO_utf8esc( char *buff, unsigned long x );
LUAI_FUNC int luaO_ceillog2( unsigned int x );
LUAI_FUNC int luaO_rawarith( lua_State *L, int op, const TValue *p1,
                             const TValue *p2, TValue *res );
LUAI_FUNC void luaO_arith( lua_State *L, int op, const TValue *p1,
                           const TValue *p2, StkId res );
LUAI_FUNC size_t luaO_str2num( const char *s, TValue *o );
LUAI_FUNC int luaO_hexavalue( int c );
LUAI_FUNC void luaO_tostring( lua_State *L, TValue *obj );
LUAI_FUNC const char *luaO_pushvfstring( lua_State *L, const char *fmt,
    va_list argp );
LUAI_FUNC const char *luaO_pushfstring( lua_State *L, const char *fmt, ... );
LUAI_FUNC void luaO_chunkid( char *out, const char *source, size_t srclen );

#endif

/*** End of inlined file: lobject.h ***/


/*** Start of inlined file: ltm.h ***/
#ifndef ltm_h
#define ltm_h

/*
* WARNING: if you change the order of this enumeration,
* grep "ORDER TM" and "ORDER OP"
*/
typedef enum {
  TM_INDEX,
  TM_NEWINDEX,
  TM_GC,
  TM_MODE,
  TM_LEN,
  TM_EQ,  /* last tag method with fast access */
  TM_ADD,
  TM_SUB,
  TM_MUL,
  TM_MOD,
  TM_POW,
  TM_DIV,
  TM_IDIV,
  TM_BAND,
  TM_BOR,
  TM_BXOR,
  TM_SHL,
  TM_SHR,
  TM_UNM,
  TM_BNOT,
  TM_LT,
  TM_LE,
  TM_CONCAT,
  TM_CALL,
  TM_CLOSE,
  TM_N		/* number of elements in the enum */
} TMS;

/*
** Test whether there is no tagmethod.
** (Because tagmethods use raw accesses, the result may be an "empty" nil.)
*/
#define notm(tm)	ttisnil(tm)

#define gfasttm(g,et,e) ((et) == NULL ? NULL : \
                         ((et)->flags & (1u<<(e))) ? NULL : luaT_gettm(et, e, (g)->tmname[e]))

#define fasttm(l,et,e)	gfasttm(G(l), et, e)

#define ttypename(x)	luaT_typenames_[(x) + 1]

LUAI_DDEC( const char *const luaT_typenames_[LUA_TOTALTYPES]; )

LUAI_FUNC const char *luaT_objtypename( lua_State *L, const TValue *o );

LUAI_FUNC const TValue *luaT_gettm( Table *events, TMS event, TString *ename );
LUAI_FUNC const TValue *luaT_gettmbyobj( lua_State *L, const TValue *o,
    TMS event );
LUAI_FUNC void luaT_init( lua_State *L );

LUAI_FUNC void luaT_callTM( lua_State *L, const TValue *f, const TValue *p1,
                            const TValue *p2, const TValue *p3 );
LUAI_FUNC void luaT_callTMres( lua_State *L, const TValue *f,
                               const TValue *p1, const TValue *p2, StkId p3 );
LUAI_FUNC void luaT_trybinTM( lua_State *L, const TValue *p1, const TValue *p2,
                              StkId res, TMS event );
LUAI_FUNC void luaT_tryconcatTM( lua_State *L );
LUAI_FUNC void luaT_trybinassocTM( lua_State *L, const TValue *p1,
                                   const TValue *p2, int inv, StkId res, TMS event );
LUAI_FUNC void luaT_trybiniTM( lua_State *L, const TValue *p1, lua_Integer i2,
                               int inv, StkId res, TMS event );
LUAI_FUNC int luaT_callorderTM( lua_State *L, const TValue *p1,
                                const TValue *p2, TMS event );
LUAI_FUNC int luaT_callorderiTM( lua_State *L, const TValue *p1, int v2,
                                 int inv, int isfloat, TMS event );

LUAI_FUNC void luaT_adjustvarargs( lua_State *L, int nfixparams,
                                   struct CallInfo *ci, const Proto *p );
LUAI_FUNC void luaT_getvarargs( lua_State *L, struct CallInfo *ci,
                                StkId where, int wanted );

#endif

/*** End of inlined file: ltm.h ***/


/*** Start of inlined file: lzio.h ***/
#ifndef lzio_h
#define lzio_h


/*** Start of inlined file: lmem.h ***/
#ifndef lmem_h
#define lmem_h

#include <stddef.h>

#define luaM_error(L)	luaD_throw(L, LUA_ERRMEM)

/*
** This macro tests whether it is safe to multiply 'n' by the size of
** type 't' without overflows. Because 'e' is always constant, it avoids
** the runtime division MAX_SIZET/(e).
** (The macro is somewhat complex to avoid warnings:  The 'sizeof'
** comparison avoids a runtime comparison when overflow cannot occur.
** The compiler should be able to optimize the real test by itself, but
** when it does it, it may give a warning about "comparison is always
** false due to limited range of data type"; the +1 tricks the compiler,
** avoiding this warning but also this optimization.)
*/
#define luaM_testsize(n,e)  \
  (sizeof(n) >= sizeof(size_t) && cast_sizet((n)) + 1 > MAX_SIZET/(e))

#define luaM_checksize(L,n,e)  \
  (luaM_testsize(n,e) ? luaM_toobig(L) : cast_void(0))

/*
** Computes the minimum between 'n' and 'MAX_SIZET/sizeof(t)', so that
** the result is not larger than 'n' and cannot overflow a 'size_t'
** when multiplied by the size of type 't'. (Assumes that 'n' is an
** 'int' or 'unsigned int' and that 'int' is not larger than 'size_t'.)
*/
#define luaM_limitN(n,t)  \
  ((cast_sizet(n) <= MAX_SIZET/sizeof(t)) ? (n) :  \
   cast_uint((MAX_SIZET/sizeof(t))))

/*
** Arrays of chars do not need any test
*/
#define luaM_reallocvchar(L,b,on,n)  \
  cast_charp(luaM_saferealloc_(L, (b), (on)*sizeof(char), (n)*sizeof(char)))

#define luaM_freemem(L, b, s)	luaM_free_(L, (b), (s))
#define luaM_free(L, b)		luaM_free_(L, (b), sizeof(*(b)))
#define luaM_freearray(L, b, n)   luaM_free_(L, (b), (n)*sizeof(*(b)))

#define luaM_new(L,t)		cast(t*, luaM_malloc_(L, sizeof(t), 0))
#define luaM_newvector(L,n,t)	cast(t*, luaM_malloc_(L, (n)*sizeof(t), 0))
#define luaM_newvectorchecked(L,n,t) \
  (luaM_checksize(L,n,sizeof(t)), luaM_newvector(L,n,t))

#define luaM_newobject(L,tag,s)	luaM_malloc_(L, (s), tag)

#define luaM_growvector(L,v,nelems,size,t,limit,e) \
  ((v)=cast(t *, luaM_growaux_(L,v,nelems,&(size),sizeof(t), \
                               luaM_limitN(limit,t),e)))

#define luaM_reallocvector(L, v,oldn,n,t) \
  (cast(t *, luaM_realloc_(L, v, cast_sizet(oldn) * sizeof(t), \
                           cast_sizet(n) * sizeof(t))))

#define luaM_shrinkvector(L,v,size,fs,t) \
  ((v)=cast(t *, luaM_shrinkvector_(L, v, &(size), fs, sizeof(t))))

LUAI_FUNC l_noret luaM_toobig( lua_State *L );

/* not to be called directly */
LUAI_FUNC void *luaM_realloc_( lua_State *L, void *block, size_t oldsize,
                               size_t size );
LUAI_FUNC void *luaM_saferealloc_( lua_State *L, void *block, size_t oldsize,
                                   size_t size );
LUAI_FUNC void luaM_free_( lua_State *L, void *block, size_t osize );
LUAI_FUNC void *luaM_growaux_( lua_State *L, void *block, int nelems,
                               int *size, int size_elem, int limit,
                               const char *what );
LUAI_FUNC void *luaM_shrinkvector_( lua_State *L, void *block, int *nelem,
                                    int final_n, int size_elem );
LUAI_FUNC void *luaM_malloc_( lua_State *L, size_t size, int tag );

#endif

/*** End of inlined file: lmem.h ***/

#define EOZ	(-1)			/* end of stream */

typedef struct Zio ZIO;

#define zgetc(z)  (((z)->n--)>0 ?  cast_uchar(*(z)->p++) : luaZ_fill(z))

typedef struct Mbuffer {
  char *buffer;
  size_t n;
  size_t buffsize;
} Mbuffer;

#define luaZ_initbuffer(L, buff) ((buff)->buffer = NULL, (buff)->buffsize = 0)

#define luaZ_buffer(buff)	((buff)->buffer)
#define luaZ_sizebuffer(buff)	((buff)->buffsize)
#define luaZ_bufflen(buff)	((buff)->n)

#define luaZ_buffremove(buff,i)	((buff)->n -= (i))
#define luaZ_resetbuffer(buff) ((buff)->n = 0)

#define luaZ_resizebuffer(L, buff, size) \
  ((buff)->buffer = luaM_reallocvchar(L, (buff)->buffer, \
                                      (buff)->buffsize, size), \
   (buff)->buffsize = size)

#define luaZ_freebuffer(L, buff)	luaZ_resizebuffer(L, buff, 0)

LUAI_FUNC void luaZ_init( lua_State *L, ZIO *z, lua_Reader reader,
                          void *data );
LUAI_FUNC size_t luaZ_read( ZIO *z, void *b, size_t n );	/* read next n bytes */

/* --------- Private Part ------------------ */

struct Zio {
  size_t n;			/* bytes still unread */
  const char *p;		/* current position in buffer */
  lua_Reader reader;		/* reader function */
  void *data;			/* additional data */
  lua_State *L;			/* Lua state (for reader) */
};

LUAI_FUNC int luaZ_fill( ZIO *z );

#endif

/*** End of inlined file: lzio.h ***/

/*
** Some notes about garbage-collected objects: All objects in Lua must
** be kept somehow accessible until being freed, so all objects always
** belong to one (and only one) of these lists, using field 'next' of
** the 'CommonHeader' for the link:
**
** 'allgc': all objects not marked for finalization;
** 'finobj': all objects marked for finalization;
** 'tobefnz': all objects ready to be finalized;
** 'fixedgc': all objects that are not to be collected (currently
** only small strings, such as reserved words).
**
** For the generational collector, some of these lists have marks for
** generations. Each mark points to the first element in the list for
** that particular generation; that generation goes until the next mark.
**
** 'allgc' -> 'survival': new objects;
** 'survival' -> 'old': objects that survived one collection;
** 'old' -> 'reallyold': objects that became old in last collection;
** 'reallyold' -> NULL: objects old for more than one cycle.
**
** 'finobj' -> 'finobjsur': new objects marked for finalization;
** 'finobjsur' -> 'finobjold': survived   """";
** 'finobjold' -> 'finobjrold': just old  """";
** 'finobjrold' -> NULL: really old       """".
*/

/*
** Moreover, there is another set of lists that control gray objects.
** These lists are linked by fields 'gclist'. (All objects that
** can become gray have such a field. The field is not the same
** in all objects, but it always has this name.)  Any gray object
** must belong to one of these lists, and all objects in these lists
** must be gray:
**
** 'gray': regular gray objects, still waiting to be visited.
** 'grayagain': objects that must be revisited at the atomic phase.
**   That includes
**   - black objects got in a write barrier;
**   - all kinds of weak tables during propagation phase;
**   - all threads.
** 'weak': tables with weak values to be cleared;
** 'ephemeron': ephemeron tables with white->white entries;
** 'allweak': tables with weak keys and/or weak values to be cleared.
*/

/*
** About 'nCcalls': each thread in Lua (a lua_State) keeps a count of
** how many "C calls" it still can do in the C stack, to avoid C-stack
** overflow.  This count is very rough approximation; it considers only
** recursive functions inside the interpreter, as non-recursive calls
** can be considered using a fixed (although unknown) amount of stack
** space.
**
** The count has two parts: the lower part is the count itself; the
** higher part counts the number of non-yieldable calls in the stack.
** (They are together so that we can change both with one instruction.)
**
** Because calls to external C functions can use an unknown amount
** of space (e.g., functions using an auxiliary buffer), calls
** to these functions add more than one to the count (see CSTACKCF).
**
** The proper count excludes the number of CallInfo structures allocated
** by Lua, as a kind of "potential" calls. So, when Lua calls a function
** (and "consumes" one CallInfo), it needs neither to decrement nor to
** check 'nCcalls', as its use of C stack is already accounted for.
*/

/* number of "C stack slots" used by an external C function */
#define CSTACKCF	10

/*
** The C-stack size is sliced in the following zones:
** - larger than CSTACKERR: normal stack;
** - [CSTACKMARK, CSTACKERR]: buffer zone to signal a stack overflow;
** - [CSTACKCF, CSTACKERRMARK]: error-handling zone;
** - below CSTACKERRMARK: buffer zone to signal overflow during overflow;
** (Because the counter can be decremented CSTACKCF at once, we need
** the so called "buffer zones", with at least that size, to properly
** detect a change from one zone to the next.)
*/
#define CSTACKERR	(8 * CSTACKCF)
#define CSTACKMARK	(CSTACKERR - (CSTACKCF + 2))
#define CSTACKERRMARK	(CSTACKCF + 2)

/* initial limit for the C-stack of threads */
#define CSTACKTHREAD	(2 * CSTACKERR)

/* true if this thread does not have non-yieldable calls in the stack */
#define yieldable(L)		(((L)->nCcalls & 0xffff0000) == 0)

/* real number of C calls */
#define getCcalls(L)	((L)->nCcalls & 0xffff)

/* Increment the number of non-yieldable calls */
#define incnny(L)	((L)->nCcalls += 0x10000)

/* Decrement the number of non-yieldable calls */
#define decnny(L)	((L)->nCcalls -= 0x10000)

/* Increment the number of non-yieldable calls and decrement nCcalls */
#define incXCcalls(L)	((L)->nCcalls += 0x10000 - CSTACKCF)

/* Decrement the number of non-yieldable calls and increment nCcalls */
#define decXCcalls(L)	((L)->nCcalls -= 0x10000 - CSTACKCF)

struct lua_longjmp;  /* defined in ldo.c */

/*
** Atomic type (relative to signals) to better ensure that 'lua_sethook'
** is thread safe
*/
#if !defined(l_signalT)
#include <signal.h>
#define l_signalT	sig_atomic_t
#endif

/* extra stack space to handle TM calls and some other extras */
#define EXTRA_STACK   5

#define BASIC_STACK_SIZE        (2*LUA_MINSTACK)

/* kinds of Garbage Collection */
#define KGC_INC		0	/* incremental gc */
#define KGC_GEN		1	/* generational gc */

typedef struct stringtable {
  TString **hash;
  int nuse;  /* number of elements */
  int size;
} stringtable;

/*
** Information about a call.
*/
typedef struct CallInfo {
  StkId func;  /* function index in the stack */
  StkId	top;  /* top for this function */
  struct CallInfo *previous, *next;  /* dynamic call link */
  union {
    struct {  /* only for Lua functions */
      const Instruction *savedpc;
      volatile l_signalT trap;
      int nextraargs;  /* # of extra arguments in vararg functions */
    } l;
    struct {  /* only for C functions */
      lua_KFunction k;  /* continuation in case of yields */
      ptrdiff_t old_errfunc;
      lua_KContext ctx;  /* context info. in case of yields */
    } c;
  } u;
  union {
    int funcidx;  /* called-function index */
    int nyield;  /* number of values yielded */
    struct {  /* info about transferred values (for call/return hooks) */
      unsigned short ftransfer;  /* offset of first value transferred */
      unsigned short ntransfer;  /* number of values transferred */
    } transferinfo;
  } u2;
  short nresults;  /* expected number of results from this function */
  unsigned short callstatus;
} CallInfo;

/*
** Bits in CallInfo status
*/
#define CIST_OAH	(1<<0)	/* original value of 'allowhook' */
#define CIST_C		(1<<1)	/* call is running a C function */
#define CIST_HOOKED	(1<<2)	/* call is running a debug hook */
#define CIST_YPCALL	(1<<3)	/* call is a yieldable protected call */
#define CIST_TAIL	(1<<4)	/* call was tail called */
#define CIST_HOOKYIELD	(1<<5)	/* last hook called yielded */
#define CIST_FIN	(1<<6)  /* call is running a finalizer */
#define CIST_TRAN	(1<<7)	/* 'ci' has transfer information */
#if defined(LUA_COMPAT_LT_LE)
#define CIST_LEQ	(1<<8)  /* using __lt for __le */
#endif

/* active function is a Lua function */
#define isLua(ci)	(!((ci)->callstatus & CIST_C))

/* call is running Lua code (not a hook) */
#define isLuacode(ci)	(!((ci)->callstatus & (CIST_C | CIST_HOOKED)))

/* assume that CIST_OAH has offset 0 and that 'v' is strictly 0/1 */
#define setoah(st,v)	((st) = ((st) & ~CIST_OAH) | (v))
#define getoah(st)	((st) & CIST_OAH)

/*
** 'global state', shared by all threads of this state
*/
typedef struct global_State {
  lua_Alloc frealloc;  /* function to reallocate memory */
  void *ud;         /* auxiliary data to 'frealloc' */
  l_mem totalbytes;  /* number of bytes currently allocated - GCdebt */
  l_mem GCdebt;  /* bytes allocated not yet compensated by the collector */
  lu_mem GCestimate;  /* an estimate of the non-garbage memory in use */
  lu_mem lastatomic;  /* see function 'genstep' in file 'lgc.c' */
  stringtable strt;  /* hash table for strings */
  TValue l_registry;
  TValue nilvalue;  /* a nil value */
  unsigned int seed;  /* randomized seed for hashes */
  lu_byte currentwhite;
  lu_byte gcstate;  /* state of garbage collector */
  lu_byte gckind;  /* kind of GC running */
  lu_byte genminormul;  /* control for minor generational collections */
  lu_byte genmajormul;  /* control for major generational collections */
  lu_byte gcrunning;  /* true if GC is running */
  lu_byte gcemergency;  /* true if this is an emergency collection */
  lu_byte gcpause;  /* size of pause between successive GCs */
  lu_byte gcstepmul;  /* GC "speed" */
  lu_byte gcstepsize;  /* (log2 of) GC granularity */
  GCObject *allgc;  /* list of all collectable objects */
  GCObject **sweepgc;  /* current position of sweep in list */
  GCObject *finobj;  /* list of collectable objects with finalizers */
  GCObject *gray;  /* list of gray objects */
  GCObject *grayagain;  /* list of objects to be traversed atomically */
  GCObject *weak;  /* list of tables with weak values */
  GCObject *ephemeron;  /* list of ephemeron tables (weak keys) */
  GCObject *allweak;  /* list of all-weak tables */
  GCObject *tobefnz;  /* list of userdata to be GC */
  GCObject *fixedgc;  /* list of objects not to be collected */
  /* fields for generational collector */
  GCObject *survival;  /* start of objects that survived one GC cycle */
  GCObject *old;  /* start of old objects */
  GCObject *reallyold;  /* old objects with more than one cycle */
  GCObject *finobjsur;  /* list of survival objects with finalizers */
  GCObject *finobjold;  /* list of old objects with finalizers */
  GCObject *finobjrold;  /* list of really old objects with finalizers */
  struct lua_State *twups;  /* list of threads with open upvalues */
  lua_CFunction panic;  /* to be called in unprotected errors */
  struct lua_State *mainthread;
  TString *memerrmsg;  /* message for memory-allocation errors */
  TString *tmname[TM_N];  /* array with tag-method names */
  struct Table *mt[LUA_NUMTAGS];  /* metatables for basic types */
  TString *strcache[STRCACHE_N][STRCACHE_M];  /* cache for strings in API */
  lua_WarnFunction warnf;  /* warning function */
  void *ud_warn;         /* auxiliary data to 'warnf' */
  unsigned int Cstacklimit;  /* current limit for the C stack */
} global_State;

/*
** 'per thread' state
*/
struct lua_State {
  CommonHeader;
  lu_byte status;
  lu_byte allowhook;
  unsigned short nci;  /* number of items in 'ci' list */
  StkId top;  /* first free slot in the stack */
  global_State *l_G;
  CallInfo *ci;  /* call info for current function */
  const Instruction *oldpc;  /* last pc traced */
  StkId stack_last;  /* last free slot in the stack */
  StkId stack;  /* stack base */
  UpVal *openupval;  /* list of open upvalues in this stack */
  GCObject *gclist;
  struct lua_State *twups;  /* list of threads with open upvalues */
  struct lua_longjmp *errorJmp;  /* current error recover point */
  CallInfo base_ci;  /* CallInfo for first level (C calling Lua) */
  volatile lua_Hook hook;
  ptrdiff_t errfunc;  /* current error handling function (stack index) */
  l_uint32 nCcalls;  /* number of allowed nested C calls - 'nci' */
  int stacksize;
  int basehookcount;
  int hookcount;
  volatile l_signalT hookmask;
};

#define G(L)	(L->l_G)

/*
** Union of all collectable objects (only for conversions)
*/
union GCUnion {
  GCObject gc;  /* common header */
  struct TString ts;
  struct Udata u;
  union Closure cl;
  struct Table h;
  struct Proto p;
  struct lua_State th;  /* thread */
  struct UpVal upv;
};

#define cast_u(o)	cast(union GCUnion *, (o))

/* macros to convert a GCObject into a specific value */
#define gco2ts(o)  \
  check_exp(novariant((o)->tt) == LUA_TSTRING, &((cast_u(o))->ts))
#define gco2u(o)  check_exp((o)->tt == LUA_VUSERDATA, &((cast_u(o))->u))
#define gco2lcl(o)  check_exp((o)->tt == LUA_VLCL, &((cast_u(o))->cl.l))
#define gco2ccl(o)  check_exp((o)->tt == LUA_VCCL, &((cast_u(o))->cl.c))
#define gco2cl(o)  \
  check_exp(novariant((o)->tt) == LUA_TFUNCTION, &((cast_u(o))->cl))
#define gco2t(o)  check_exp((o)->tt == LUA_VTABLE, &((cast_u(o))->h))
#define gco2p(o)  check_exp((o)->tt == LUA_VPROTO, &((cast_u(o))->p))
#define gco2th(o)  check_exp((o)->tt == LUA_VTHREAD, &((cast_u(o))->th))
#define gco2upv(o)	check_exp((o)->tt == LUA_VUPVAL, &((cast_u(o))->upv))

/*
** macro to convert a Lua object into a GCObject
** (The access to 'tt' tries to ensure that 'v' is actually a Lua object.)
*/
#define obj2gco(v)	check_exp((v)->tt >= LUA_TSTRING, &(cast_u(v)->gc))

/* actual number of total bytes allocated */
#define gettotalbytes(g)	cast(lu_mem, (g)->totalbytes + (g)->GCdebt)

LUAI_FUNC void luaE_setdebt( global_State *g, l_mem debt );
LUAI_FUNC void luaE_freethread( lua_State *L, lua_State *L1 );
LUAI_FUNC CallInfo *luaE_extendCI( lua_State *L );
LUAI_FUNC void luaE_freeCI( lua_State *L );
LUAI_FUNC void luaE_shrinkCI( lua_State *L );
LUAI_FUNC void luaE_enterCcall( lua_State *L );
LUAI_FUNC void luaE_warning( lua_State *L, const char *msg, int tocont );
LUAI_FUNC void luaE_warnerror( lua_State *L, const char *where );

#define luaE_exitCcall(L)	((L)->nCcalls++)

#endif

/*** End of inlined file: lstate.h ***/

/* Increments 'L->top', checking for stack overflows */
#define api_incr_top(L)   {L->top++; api_check(L, L->top <= L->ci->top, \
      "stack overflow");}

/*
** If a call returns too many multiple returns, the callee may not have
** stack space to accommodate all results. In this case, this macro
** increases its stack space ('L->ci->top').
*/
#define adjustresults(L,nres) \
  { if ((nres) <= LUA_MULTRET && L->ci->top < L->top) L->ci->top = L->top; }

/* Ensure the stack has at least 'n' elements */
#define api_checknelems(L,n)	api_check(L, (n) < (L->top - L->ci->func), \
                                        "not enough elements in the stack")

/*
** To reduce the overhead of returning from C functions, the presence of
** to-be-closed variables in these functions is coded in the CallInfo's
** field 'nresults', in a way that functions with no to-be-closed variables
** with zero, one, or "all" wanted results have no overhead. Functions
** with other number of wanted results, as well as functions with
** variables to be closed, have an extra check.
*/

#define hastocloseCfunc(n)	((n) < LUA_MULTRET)

#define codeNresults(n)		(-(n) - 3)

#endif

/*** End of inlined file: lapi.h ***/


/*** Start of inlined file: ldebug.h ***/
#ifndef ldebug_h
#define ldebug_h

#define pcRel(pc, p)	(cast_int((pc) - (p)->code) - 1)

#define resethookcount(L)	(L->hookcount = L->basehookcount)

/*
** mark for entries in 'lineinfo' array that has absolute information in
** 'abslineinfo' array
*/
#define ABSLINEINFO	(-0x80)

LUAI_FUNC int luaG_getfuncline( const Proto *f, int pc );
LUAI_FUNC const char *luaG_findlocal( lua_State *L, CallInfo *ci, int n,
                                      StkId *pos );
LUAI_FUNC l_noret luaG_typeerror( lua_State *L, const TValue *o,
                                  const char *opname );
LUAI_FUNC l_noret luaG_forerror( lua_State *L, const TValue *o,
                                 const char *what );
LUAI_FUNC l_noret luaG_concaterror( lua_State *L, const TValue *p1,
                                    const TValue *p2 );
LUAI_FUNC l_noret luaG_opinterror( lua_State *L, const TValue *p1,
                                   const TValue *p2,
                                   const char *msg );
LUAI_FUNC l_noret luaG_tointerror( lua_State *L, const TValue *p1,
                                   const TValue *p2 );
LUAI_FUNC l_noret luaG_ordererror( lua_State *L, const TValue *p1,
                                   const TValue *p2 );
LUAI_FUNC l_noret luaG_runerror( lua_State *L, const char *fmt, ... );
LUAI_FUNC const char *luaG_addinfo( lua_State *L, const char *msg,
                                    TString *src, int line );
LUAI_FUNC l_noret luaG_errormsg( lua_State *L );
LUAI_FUNC int luaG_traceexec( lua_State *L, const Instruction *pc );

#endif

/*** End of inlined file: ldebug.h ***/


/*** Start of inlined file: ldo.h ***/
#ifndef ldo_h
#define ldo_h

/*
** Macro to check stack size and grow stack if needed.  Parameters
** 'pre'/'pos' allow the macro to preserve a pointer into the
** stack across reallocations, doing the work only when needed.
** 'condmovestack' is used in heavy tests to force a stack reallocation
** at every check.
*/
#define luaD_checkstackaux(L,n,pre,pos)  \
  if (L->stack_last - L->top <= (n)) \
  { pre; luaD_growstack(L, n, 1); pos; } \
  else { condmovestack(L,pre,pos); }

/* In general, 'pre'/'pos' are empty (nothing to save) */
#define luaD_checkstack(L,n)	luaD_checkstackaux(L,n,(void)0,(void)0)

#define savestack(L,p)		((char *)(p) - (char *)L->stack)
#define restorestack(L,n)	((StkId)((char *)L->stack + (n)))

/* macro to check stack size, preserving 'p' */
#define checkstackp(L,n,p)  \
  luaD_checkstackaux(L, n, \
                     ptrdiff_t t__ = savestack(L, p);  /* save 'p' */ \
                     luaC_checkGC(L),  /* stack grow uses memory */ \
                     p = restorestack(L, t__))  /* 'pos' part: restore 'p' */

/* macro to check stack size and GC */
#define checkstackGC(L,fsize)  \
  luaD_checkstackaux(L, (fsize), (void)0, luaC_checkGC(L))

/* type of protected functions, to be ran by 'runprotected' */
typedef void ( *Pfunc )( lua_State *L, void *ud );

LUAI_FUNC void luaD_seterrorobj( lua_State *L, int errcode, StkId oldtop );
LUAI_FUNC int luaD_protectedparser( lua_State *L, ZIO *z, const char *name,
                                    const char *mode );
LUAI_FUNC void luaD_hook( lua_State *L, int event, int line,
                          int fTransfer, int nTransfer );
LUAI_FUNC void luaD_hookcall( lua_State *L, CallInfo *ci );
LUAI_FUNC void luaD_pretailcall( lua_State *L, CallInfo *ci, StkId func, int n );
LUAI_FUNC void luaD_call( lua_State *L, StkId func, int nResults );
LUAI_FUNC void luaD_callnoyield( lua_State *L, StkId func, int nResults );
LUAI_FUNC void luaD_tryfuncTM( lua_State *L, StkId func );
LUAI_FUNC int luaD_pcall( lua_State *L, Pfunc func, void *u,
                          ptrdiff_t oldtop, ptrdiff_t ef );
LUAI_FUNC void luaD_poscall( lua_State *L, CallInfo *ci, int nres );
LUAI_FUNC int luaD_reallocstack( lua_State *L, int newsize, int raiseerror );
LUAI_FUNC int luaD_growstack( lua_State *L, int n, int raiseerror );
LUAI_FUNC void luaD_shrinkstack( lua_State *L );
LUAI_FUNC void luaD_inctop( lua_State *L );

LUAI_FUNC l_noret luaD_throw( lua_State *L, int errcode );
LUAI_FUNC int luaD_rawrunprotected( lua_State *L, Pfunc f, void *ud );

#endif

/*** End of inlined file: ldo.h ***/


/*** Start of inlined file: lfunc.h ***/
#ifndef lfunc_h
#define lfunc_h

#define sizeCclosure(n)	(cast_int(offsetof(CClosure, upvalue)) + \
                         cast_int(sizeof(TValue)) * (n))

#define sizeLclosure(n)	(cast_int(offsetof(LClosure, upvals)) + \
                         cast_int(sizeof(TValue *)) * (n))

/* test whether thread is in 'twups' list */
#define isintwups(L)	(L->twups != L)

/*
** maximum number of upvalues in a closure (both C and Lua). (Value
** must fit in a VM register.)
*/
#define MAXUPVAL	255

#define upisopen(up)	((up)->v != &(up)->u.value)

#define uplevel(up)	check_exp(upisopen(up), cast(StkId, (up)->v))

/*
** maximum number of misses before giving up the cache of closures
** in prototypes
*/
#define MAXMISS		10

/*
** Special "status" for 'luaF_close'
*/

/* close upvalues without running their closing methods */
#define NOCLOSINGMETH	(-1)

/* close upvalues running all closing methods in protected mode */
#define CLOSEPROTECT	(-2)

LUAI_FUNC Proto *luaF_newproto( lua_State *L );
LUAI_FUNC CClosure *luaF_newCclosure( lua_State *L, int nupvals );
LUAI_FUNC LClosure *luaF_newLclosure( lua_State *L, int nupvals );
LUAI_FUNC void luaF_initupvals( lua_State *L, LClosure *cl );
LUAI_FUNC UpVal *luaF_findupval( lua_State *L, StkId level );
LUAI_FUNC void luaF_newtbcupval( lua_State *L, StkId level );
LUAI_FUNC int luaF_close( lua_State *L, StkId level, int status );
LUAI_FUNC void luaF_unlinkupval( UpVal *uv );
LUAI_FUNC void luaF_freeproto( lua_State *L, Proto *f );
LUAI_FUNC const char *luaF_getlocalname( const Proto *func, int local_number,
    int pc );

#endif

/*** End of inlined file: lfunc.h ***/


/*** Start of inlined file: lgc.h ***/
#ifndef lgc_h
#define lgc_h

/*
** Collectable objects may have one of three colors: white, which
** means the object is not marked; gray, which means the
** object is marked, but its references may be not marked; and
** black, which means that the object and all its references are marked.
** The main invariant of the garbage collector, while marking objects,
** is that a black object can never point to a white one. Moreover,
** any gray object must be in a "gray list" (gray, grayagain, weak,
** allweak, ephemeron) so that it can be visited again before finishing
** the collection cycle. These lists have no meaning when the invariant
** is not being enforced (e.g., sweep phase).
*/

/*
** Possible states of the Garbage Collector
*/
#define GCSpropagate	0
#define GCSenteratomic	1
#define GCSatomic	2
#define GCSswpallgc	3
#define GCSswpfinobj	4
#define GCSswptobefnz	5
#define GCSswpend	6
#define GCScallfin	7
#define GCSpause	8

#define issweepphase(g)  \
  (GCSswpallgc <= (g)->gcstate && (g)->gcstate <= GCSswpend)

/*
** macro to tell when main invariant (white objects cannot point to black
** ones) must be kept. During a collection, the sweep
** phase may break the invariant, as objects turned white may point to
** still-black objects. The invariant is restored when sweep ends and
** all objects are white again.
*/

#define keepinvariant(g)	((g)->gcstate <= GCSatomic)

/*
** some useful bit tricks
*/
#define resetbits(x,m)		((x) &= cast_byte(~(m)))
#define setbits(x,m)		((x) |= (m))
#define testbits(x,m)		((x) & (m))
#define bitmask(b)		(1<<(b))
#define bit2mask(b1,b2)		(bitmask(b1) | bitmask(b2))
#define l_setbit(x,b)		setbits(x, bitmask(b))
#define resetbit(x,b)		resetbits(x, bitmask(b))
#define testbit(x,b)		testbits(x, bitmask(b))

/*
** Layout for bit use in 'marked' field. First three bits are
** used for object "age" in generational mode. Last bit is free
** to be used by respective objects.
*/
#define WHITE0BIT	3  /* object is white (type 0) */
#define WHITE1BIT	4  /* object is white (type 1) */
#define BLACKBIT	5  /* object is black */
#define FINALIZEDBIT	6  /* object has been marked for finalization */

#define WHITEBITS	bit2mask(WHITE0BIT, WHITE1BIT)

#define iswhite(x)      testbits((x)->marked, WHITEBITS)
#define isblack(x)      testbit((x)->marked, BLACKBIT)
#define isgray(x)  /* neither white nor black */  \
  (!testbits((x)->marked, WHITEBITS | bitmask(BLACKBIT)))

#define tofinalize(x)	testbit((x)->marked, FINALIZEDBIT)

#define otherwhite(g)	((g)->currentwhite ^ WHITEBITS)
#define isdeadm(ow,m)	((m) & (ow))
#define isdead(g,v)	isdeadm(otherwhite(g), (v)->marked)

#define changewhite(x)	((x)->marked ^= WHITEBITS)
#define gray2black(x)	l_setbit((x)->marked, BLACKBIT)

#define luaC_white(g)	cast_byte((g)->currentwhite & WHITEBITS)

/* object age in generational mode */
#define G_NEW		0	/* created in current cycle */
#define G_SURVIVAL	1	/* created in previous cycle */
#define G_OLD0		2	/* marked old by frw. barrier in this cycle */
#define G_OLD1		3	/* first full cycle as old */
#define G_OLD		4	/* really old object (not to be visited) */
#define G_TOUCHED1	5	/* old object touched this cycle */
#define G_TOUCHED2	6	/* old object touched in previous cycle */

#define AGEBITS		7  /* all age bits (111) */

#define getage(o)	((o)->marked & AGEBITS)
#define setage(o,a)  ((o)->marked = cast_byte(((o)->marked & (~AGEBITS)) | a))
#define isold(o)	(getage(o) > G_SURVIVAL)

#define changeage(o,f,t)  \
  check_exp(getage(o) == (f), (o)->marked ^= ((f)^(t)))

/* Default Values for GC parameters */
#define LUAI_GENMAJORMUL         100
#define LUAI_GENMINORMUL         20

/* wait memory to double before starting new cycle */
#define LUAI_GCPAUSE    200

/*
** some gc parameters are stored divided by 4 to allow a maximum value
** up to 1023 in a 'lu_byte'.
*/
#define getgcparam(p)	((p) * 4)
#define setgcparam(p,v)	((p) = (v) / 4)

#define LUAI_GCMUL      100

/* how much to allocate before next GC step (log2) */
#define LUAI_GCSTEPSIZE 13      /* 8 KB */

/*
** Check whether the declared GC mode is generational. While in
** generational mode, the collector can go temporarily to incremental
** mode to improve performance. This is signaled by 'g->lastatomic != 0'.
*/
#define isdecGCmodegen(g)	(g->gckind == KGC_GEN || g->lastatomic != 0)

/*
** Does one step of collection when debt becomes positive. 'pre'/'pos'
** allows some adjustments to be done only when needed. macro
** 'condchangemem' is used only for heavy tests (forcing a full
** GC cycle on every opportunity)
*/
#define luaC_condGC(L,pre,pos) \
  { if (G(L)->GCdebt > 0) { pre; luaC_step(L); pos;}; \
    condchangemem(L,pre,pos); }

/* more often than not, 'pre'/'pos' are empty */
#define luaC_checkGC(L)		luaC_condGC(L,(void)0,(void)0)

#define luaC_barrier(L,p,v) (  \
                               (iscollectable(v) && isblack(p) && iswhite(gcvalue(v))) ?  \
                               luaC_barrier_(L,obj2gco(p),gcvalue(v)) : cast_void(0))

#define luaC_barrierback(L,p,v) (  \
                                   (iscollectable(v) && isblack(p) && iswhite(gcvalue(v))) ? \
                                   luaC_barrierback_(L,p) : cast_void(0))

#define luaC_objbarrier(L,p,o) (  \
                                  (isblack(p) && iswhite(o)) ? \
                                  luaC_barrier_(L,obj2gco(p),obj2gco(o)) : cast_void(0))

LUAI_FUNC void luaC_fix( lua_State *L, GCObject *o );
LUAI_FUNC void luaC_freeallobjects( lua_State *L );
LUAI_FUNC void luaC_step( lua_State *L );
LUAI_FUNC void luaC_runtilstate( lua_State *L, int statesmask );
LUAI_FUNC void luaC_fullgc( lua_State *L, int isemergency );
LUAI_FUNC GCObject *luaC_newobj( lua_State *L, int tt, size_t sz );
LUAI_FUNC void luaC_barrier_( lua_State *L, GCObject *o, GCObject *v );
LUAI_FUNC void luaC_barrierback_( lua_State *L, GCObject *o );
LUAI_FUNC void luaC_checkfinalizer( lua_State *L, GCObject *o, Table *mt );
LUAI_FUNC void luaC_changemode( lua_State *L, int newmode );

#endif

/*** End of inlined file: lgc.h ***/


/*** Start of inlined file: lstring.h ***/
#ifndef lstring_h
#define lstring_h

/*
** Memory-allocation error message must be preallocated (it cannot
** be created after memory is exhausted)
*/
#define MEMERRMSG       "not enough memory"

/*
** Size of a TString: Size of the header plus space for the string
** itself (including final '\0').
*/
#define sizelstring(l)  (offsetof(TString, contents) + ((l) + 1) * sizeof(char))

#define luaS_newliteral(L, s)	(luaS_newlstr(L, "" s, \
                               (sizeof(s)/sizeof(char))-1))

/*
** test whether a string is a reserved word
*/
#define isreserved(s)	((s)->tt == LUA_VSHRSTR && (s)->extra > 0)

/*
** equality for short strings, which are always internalized
*/
#define eqshrstr(a,b)	check_exp((a)->tt == LUA_VSHRSTR, (a) == (b))

LUAI_FUNC unsigned int luaS_hash( const char *str, size_t l,
                                  unsigned int seed, size_t step );
LUAI_FUNC unsigned int luaS_hashlongstr( TString *ts );
LUAI_FUNC int luaS_eqlngstr( TString *a, TString *b );
LUAI_FUNC void luaS_resize( lua_State *L, int newsize );
LUAI_FUNC void luaS_clearcache( global_State *g );
LUAI_FUNC void luaS_init( lua_State *L );
LUAI_FUNC void luaS_remove( lua_State *L, TString *ts );
LUAI_FUNC Udata *luaS_newudata( lua_State *L, size_t s, int nuvalue );
LUAI_FUNC TString *luaS_newlstr( lua_State *L, const char *str, size_t l );
LUAI_FUNC TString *luaS_new( lua_State *L, const char *str );
LUAI_FUNC TString *luaS_createlngstrobj( lua_State *L, size_t l );

#endif

/*** End of inlined file: lstring.h ***/


/*** Start of inlined file: ltable.h ***/
#ifndef ltable_h
#define ltable_h

#define gnode(t,i)	(&(t)->node[i])
#define gval(n)		(&(n)->i_val)
#define gnext(n)	((n)->u.next)

#define invalidateTMcache(t)	((t)->flags = 0)

/* true when 't' is using 'dummynode' as its hash part */
#define isdummy(t)		((t)->lastfree == NULL)

/* allocated size for hash nodes */
#define allocsizenode(t)	(isdummy(t) ? 0 : sizenode(t))

/* returns the Node, given the value of a table entry */
#define nodefromval(v)	cast(Node *, (v))

LUAI_FUNC const TValue *luaH_getint( Table *t, lua_Integer key );
LUAI_FUNC void luaH_setint( lua_State *L, Table *t, lua_Integer key,
                            TValue *value );
LUAI_FUNC const TValue *luaH_getshortstr( Table *t, TString *key );
LUAI_FUNC const TValue *luaH_getstr( Table *t, TString *key );
LUAI_FUNC const TValue *luaH_get( Table *t, const TValue *key );
LUAI_FUNC TValue *luaH_newkey( lua_State *L, Table *t, const TValue *key );
LUAI_FUNC TValue *luaH_set( lua_State *L, Table *t, const TValue *key );
LUAI_FUNC Table *luaH_new( lua_State *L );
LUAI_FUNC void luaH_resize( lua_State *L, Table *t, unsigned int nasize,
                            unsigned int nhsize );
LUAI_FUNC void luaH_resizearray( lua_State *L, Table *t, unsigned int nasize );
LUAI_FUNC void luaH_free( lua_State *L, Table *t );
LUAI_FUNC int luaH_next( lua_State *L, Table *t, StkId key );
LUAI_FUNC lua_Unsigned luaH_getn( Table *t );
LUAI_FUNC unsigned int luaH_realasize( const Table *t );

#if defined(LUA_DEBUG)
LUAI_FUNC Node *luaH_mainposition( const Table *t, const TValue *key );
LUAI_FUNC int luaH_isdummy( const Table *t );
#endif

#endif

/*** End of inlined file: ltable.h ***/


/*** Start of inlined file: lundump.h ***/
#ifndef lundump_h
#define lundump_h

/* data to catch conversion errors */
#define LUAC_DATA	"\x19\x93\r\n\x1a\n"

#define LUAC_INT	0x5678
#define LUAC_NUM	cast_num(370.5)

/*
** Encode major-minor version in one byte, one nibble for each
*/
#define MYINT(s)	(s[0]-'0')  /* assume one-digit numerals */
#define LUAC_VERSION	(MYINT(LUA_VERSION_MAJOR)*16+MYINT(LUA_VERSION_MINOR))

#define LUAC_FORMAT	0	/* this is the official format */

/* load one chunk; from lundump.c */
LUAI_FUNC LClosure *luaU_undump( lua_State *L, ZIO *Z, const char *name );

/* dump one chunk; from ldump.c */
LUAI_FUNC int luaU_dump( lua_State *L, const Proto *f, lua_Writer w,
                         void *data, int strip );

#endif

/*** End of inlined file: lundump.h ***/


/*** Start of inlined file: lvm.h ***/
#ifndef lvm_h
#define lvm_h

#if !defined(LUA_NOCVTN2S)
#define cvt2str(o)	ttisnumber(o)
#else
#define cvt2str(o)	0	/* no conversion from numbers to strings */
#endif

#if !defined(LUA_NOCVTS2N)
#define cvt2num(o)	ttisstring(o)
#else
#define cvt2num(o)	0	/* no conversion from strings to numbers */
#endif

/*
** You can define LUA_FLOORN2I if you want to convert floats to integers
** by flooring them (instead of raising an error if they are not
** integral values)
*/
#if !defined(LUA_FLOORN2I)
#define LUA_FLOORN2I		F2Ieq
#endif

/*
** Rounding modes for float->integer coercion
 */
typedef enum {
  F2Ieq,     /* no rounding; accepts only integral values */
  F2Ifloor,  /* takes the floor of the number */
  F2Iceil    /* takes the ceil of the number */
} F2Imod;

/* convert an object to a float (including string coercion) */
#define tonumber(o,n) \
  (ttisfloat(o) ? (*(n) = fltvalue(o), 1) : luaV_tonumber_(o,n))

/* convert an object to a float (without string coercion) */
#define tonumberns(o,n) \
  (ttisfloat(o) ? ((n) = fltvalue(o), 1) : \
   (ttisinteger(o) ? ((n) = cast_num(ivalue(o)), 1) : 0))

/* convert an object to an integer (including string coercion) */
#define tointeger(o,i) \
  (ttisinteger(o) ? (*(i) = ivalue(o), 1) : luaV_tointeger(o,i,LUA_FLOORN2I))

/* convert an object to an integer (without string coercion) */
#define tointegerns(o,i) \
  (ttisinteger(o) ? (*(i) = ivalue(o), 1) : luaV_tointegerns(o,i,LUA_FLOORN2I))

#define intop(op,v1,v2) l_castU2S(l_castS2U(v1) op l_castS2U(v2))

#define luaV_rawequalobj(t1,t2)		luaV_equalobj(NULL,t1,t2)

/*
** fast track for 'gettable': if 't' is a table and 't[k]' is present,
** return 1 with 'slot' pointing to 't[k]' (position of final result).
** Otherwise, return 0 (meaning it will have to check metamethod)
** with 'slot' pointing to an empty 't[k]' (if 't' is a table) or NULL
** (otherwise). 'f' is the raw get function to use.
*/
#define luaV_fastget(L,t,k,slot,f) \
  (!ttistable(t)  \
   ? (slot = NULL, 0)  /* not a table; 'slot' is NULL and result is 0 */  \
   : (slot = f(hvalue(t), k),  /* else, do raw access */  \
      !isempty(slot)))  /* result not empty? */

/*
** Special case of 'luaV_fastget' for integers, inlining the fast case
** of 'luaH_getint'.
*/
#define luaV_fastgeti(L,t,k,slot) \
  (!ttistable(t)  \
   ? (slot = NULL, 0)  /* not a table; 'slot' is NULL and result is 0 */  \
   : (slot = (l_castS2U(k) - 1u < hvalue(t)->alimit) \
             ? &hvalue(t)->array[k - 1] : luaH_getint(hvalue(t), k), \
      !isempty(slot)))  /* result not empty? */

/*
** Finish a fast set operation (when fast get succeeds). In that case,
** 'slot' points to the place to put the value.
*/
#define luaV_finishfastset(L,t,slot,v) \
  { setobj2t(L, cast(TValue *,slot), v); \
    luaC_barrierback(L, gcvalue(t), v); }

LUAI_FUNC int luaV_equalobj( lua_State *L, const TValue *t1, const TValue *t2 );
LUAI_FUNC int luaV_lessthan( lua_State *L, const TValue *l, const TValue *r );
LUAI_FUNC int luaV_lessequal( lua_State *L, const TValue *l, const TValue *r );
LUAI_FUNC int luaV_tonumber_( const TValue *obj, lua_Number *n );
LUAI_FUNC int luaV_tointeger( const TValue *obj, lua_Integer *p, F2Imod mode );
LUAI_FUNC int luaV_tointegerns( const TValue *obj, lua_Integer *p,
                                F2Imod mode );
LUAI_FUNC int luaV_flttointeger( lua_Number n, lua_Integer *p, F2Imod mode );
LUAI_FUNC void luaV_finishget( lua_State *L, const TValue *t, TValue *key,
                               StkId val, const TValue *slot );
LUAI_FUNC void luaV_finishset( lua_State *L, const TValue *t, TValue *key,
                               TValue *val, const TValue *slot );
LUAI_FUNC void luaV_finishOp( lua_State *L );
LUAI_FUNC void luaV_execute( lua_State *L, CallInfo *ci );
LUAI_FUNC void luaV_concat( lua_State *L, int total );
LUAI_FUNC lua_Integer luaV_idiv( lua_State *L, lua_Integer x, lua_Integer y );
LUAI_FUNC lua_Integer luaV_mod( lua_State *L, lua_Integer x, lua_Integer y );
LUAI_FUNC lua_Number luaV_modf( lua_State *L, lua_Number x, lua_Number y );
LUAI_FUNC lua_Integer luaV_shiftl( lua_Integer x, lua_Integer y );
LUAI_FUNC void luaV_objlen( lua_State *L, StkId ra, const TValue *rb );

#endif

/*** End of inlined file: lvm.h ***/

const char lua_ident[] =
  "$LuaVersion: " LUA_COPYRIGHT " $"
  "$LuaAuthors: " LUA_AUTHORS " $";

/*
** Test for a valid index.
** '!ttisnil(o)' implies 'o != &G(L)->nilvalue', so it is not needed.
** However, it covers the most common cases in a faster way.
*/
#define isvalid(L, o)	(!ttisnil(o) || o != &G(L)->nilvalue)

/* test for pseudo index */
#define ispseudo(i)		((i) <= LUA_REGISTRYINDEX)

/* test for upvalue */
#define isupvalue(i)		((i) < LUA_REGISTRYINDEX)

static TValue *index2value( lua_State *L, int idx ) {
  CallInfo *ci = L->ci;

  if( idx > 0 ) {
    StkId o = ci->func + idx;
    api_check( L, idx <= L->ci->top - ( ci->func + 1 ), "unacceptable index" );

    if( o >= L->top ) {
      return &G( L )->nilvalue;
    } else {
      return s2v( o );
    }
  } else if( !ispseudo( idx ) ) { /* negative index */
    api_check( L, idx != 0 && -idx <= L->top - ( ci->func + 1 ), "invalid index" );
    return s2v( L->top + idx );
  } else if( idx == LUA_REGISTRYINDEX ) {
    return &G( L )->l_registry;
  } else { /* upvalues */
    idx = LUA_REGISTRYINDEX - idx;
    api_check( L, idx <= MAXUPVAL + 1, "upvalue index too large" );

    if( ttislcf( s2v( ci->func ) ) ) { /* light C function? */
      return &G( L )->nilvalue;  /* it has no upvalues */
    } else {
      CClosure *func = clCvalue( s2v( ci->func ) );
      return ( idx <= func->nupvalues ) ? &func->upvalue[idx - 1] : &G( L )->nilvalue;
    }
  }
}

static StkId index2stack( lua_State *L, int idx ) {
  CallInfo *ci = L->ci;

  if( idx > 0 ) {
    StkId o = ci->func + idx;
    api_check( L, o < L->top, "unacceptable index" );
    return o;
  } else {  /* non-positive index */
    api_check( L, idx != 0 && -idx <= L->top - ( ci->func + 1 ), "invalid index" );
    api_check( L, !ispseudo( idx ), "invalid index" );
    return L->top + idx;
  }
}

LUA_API int lua_checkstack( lua_State *L, int n ) {
  int res;
  CallInfo *ci = L->ci;
  lua_lock( L );
  api_check( L, n >= 0, "negative 'n'" );

  if( L->stack_last - L->top > n ) { /* stack large enough? */
    res = 1;  /* yes; check is OK */
  } else { /* no; need to grow stack */
    int inuse = cast_int( L->top - L->stack ) + EXTRA_STACK;

    if( inuse > LUAI_MAXSTACK - n ) { /* can grow without overflow? */
      res = 0;  /* no */
    } else { /* try to grow stack */
      res = luaD_growstack( L, n, 0 );
    }
  }

  if( res && ci->top < L->top + n ) {
    ci->top = L->top + n;  /* adjust frame top */
  }

  lua_unlock( L );
  return res;
}

LUA_API void lua_xmove( lua_State *from, lua_State *to, int n ) {
  int i;

  if( from == to ) {
    return;
  }

  lua_lock( to );
  api_checknelems( from, n );
  api_check( from, G( from ) == G( to ), "moving among independent states" );
  api_check( from, to->ci->top - to->top >= n, "stack overflow" );
  from->top -= n;

  for( i = 0; i < n; i++ ) {
    setobjs2s( to, to->top, from->top + i );
    to->top++;  /* stack already checked by previous 'api_check' */
  }

  lua_unlock( to );
}

LUA_API lua_CFunction lua_atpanic( lua_State *L, lua_CFunction panicf ) {
  lua_CFunction old;
  lua_lock( L );
  old = G( L )->panic;
  G( L )->panic = panicf;
  lua_unlock( L );
  return old;
}

LUA_API lua_Number lua_version( lua_State *L ) {
  UNUSED( L );
  return LUA_VERSION_NUM;
}

/*
** basic stack manipulation
*/

/*
** convert an acceptable stack index into an absolute index
*/
LUA_API int lua_absindex( lua_State *L, int idx ) {
  return ( idx > 0 || ispseudo( idx ) )
         ? idx
         : cast_int( L->top - L->ci->func ) + idx;
}

LUA_API int lua_gettop( lua_State *L ) {
  return cast_int( L->top - ( L->ci->func + 1 ) );
}

LUA_API void lua_settop( lua_State *L, int idx ) {
  CallInfo *ci = L->ci;
  StkId func = ci->func;
  ptrdiff_t diff;  /* difference for new top */
  lua_lock( L );

  if( idx >= 0 ) {
    api_check( L, idx <= ci->top - ( func + 1 ), "new top too large" );
    diff = ( ( func + 1 ) + idx ) - L->top;

    for( ; diff > 0; diff-- ) {
      setnilvalue( s2v( L->top++ ) );  /* clear new slots */
    }
  } else {
    api_check( L, -( idx + 1 ) <= ( L->top - ( func + 1 ) ), "invalid new top" );
    diff = idx + 1;  /* will "subtract" index (as it is negative) */
  }

  if( diff < 0 && hastocloseCfunc( ci->nresults ) ) {
    luaF_close( L, L->top + diff, LUA_OK );
  }

  L->top += diff;  /* correct top only after closing any upvalue */
  lua_unlock( L );
}

/*
** Reverse the stack segment from 'from' to 'to'
** (auxiliary to 'lua_rotate')
** Note that we move(copy) only the value inside the stack.
** (We do not move additional fields that may exist.)
*/
static void reverse( lua_State *L, StkId from, StkId to ) {
  for( ; from < to; from++, to-- ) {
    TValue temp;
    setobj( L, &temp, s2v( from ) );
    setobjs2s( L, from, to );
    setobj2s( L, to, &temp );
  }
}

/*
** Let x = AB, where A is a prefix of length 'n'. Then,
** rotate x n == BA. But BA == (A^r . B^r)^r.
*/
LUA_API void lua_rotate( lua_State *L, int idx, int n ) {
  StkId p, t, m;
  lua_lock( L );
  t = L->top - 1;  /* end of stack segment being rotated */
  p = index2stack( L, idx ); /* start of segment */
  api_check( L, ( n >= 0 ? n : -n ) <= ( t - p + 1 ), "invalid 'n'" );
  m = ( n >= 0 ? t - n : p - n - 1 ); /* end of prefix */
  reverse( L, p, m ); /* reverse the prefix with length 'n' */
  reverse( L, m + 1, t ); /* reverse the suffix */
  reverse( L, p, t ); /* reverse the entire segment */
  lua_unlock( L );
}

LUA_API void lua_copy( lua_State *L, int fromidx, int toidx ) {
  TValue *fr, *to;
  lua_lock( L );
  fr = index2value( L, fromidx );
  to = index2value( L, toidx );
  api_check( L, isvalid( L, to ), "invalid index" );
  setobj( L, to, fr );

  if( isupvalue( toidx ) ) { /* function upvalue? */
    luaC_barrier( L, clCvalue( s2v( L->ci->func ) ), fr );
  }

  /* LUA_REGISTRYINDEX does not need gc barrier
     (collector revisits it before finishing collection) */
  lua_unlock( L );
}

LUA_API void lua_pushvalue( lua_State *L, int idx ) {
  lua_lock( L );
  setobj2s( L, L->top, index2value( L, idx ) );
  api_incr_top( L );
  lua_unlock( L );
}

/*
** access functions (stack -> C)
*/

LUA_API int lua_type( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );
  return ( isvalid( L, o ) ? ttype( o ) : LUA_TNONE );
}

LUA_API const char *lua_typename( lua_State *L, int t ) {
  UNUSED( L );
  api_check( L, LUA_TNONE <= t && t < LUA_NUMTYPES, "invalid type" );
  return ttypename( t );
}

LUA_API int lua_iscfunction( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );
  return ( ttislcf( o ) || ( ttisCclosure( o ) ) );
}

LUA_API int lua_isinteger( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );
  return ttisinteger( o );
}

LUA_API int lua_isnumber( lua_State *L, int idx ) {
  lua_Number n;
  const TValue *o = index2value( L, idx );
  return tonumber( o, &n );
}

LUA_API int lua_isstring( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );
  return ( ttisstring( o ) || cvt2str( o ) );
}

LUA_API int lua_isuserdata( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );
  return ( ttisfulluserdata( o ) || ttislightuserdata( o ) );
}

LUA_API int lua_rawequal( lua_State *L, int index1, int index2 ) {
  const TValue *o1 = index2value( L, index1 );
  const TValue *o2 = index2value( L, index2 );
  return ( isvalid( L, o1 ) && isvalid( L, o2 ) ) ? luaV_rawequalobj( o1, o2 ) : 0;
}

LUA_API void lua_arith( lua_State *L, int op ) {
  lua_lock( L );

  if( op != LUA_OPUNM && op != LUA_OPBNOT ) {
    api_checknelems( L, 2 );  /* all other operations expect two operands */
  } else { /* for unary operations, add fake 2nd operand */
    api_checknelems( L, 1 );
    setobjs2s( L, L->top, L->top - 1 );
    api_incr_top( L );
  }

  /* first operand at top - 2, second at top - 1; result go to top - 2 */
  luaO_arith( L, op, s2v( L->top - 2 ), s2v( L->top - 1 ), L->top - 2 );
  L->top--;  /* remove second operand */
  lua_unlock( L );
}

LUA_API int lua_compare( lua_State *L, int index1, int index2, int op ) {
  const TValue *o1;
  const TValue *o2;
  int i = 0;
  lua_lock( L ); /* may call tag method */
  o1 = index2value( L, index1 );
  o2 = index2value( L, index2 );

  if( isvalid( L, o1 ) && isvalid( L, o2 ) ) {
    switch( op ) {
      case LUA_OPEQ:
        i = luaV_equalobj( L, o1, o2 );
        break;

      case LUA_OPLT:
        i = luaV_lessthan( L, o1, o2 );
        break;

      case LUA_OPLE:
        i = luaV_lessequal( L, o1, o2 );
        break;

      default:
        api_check( L, 0, "invalid option" );
    }
  }

  lua_unlock( L );
  return i;
}

LUA_API size_t lua_stringtonumber( lua_State *L, const char *s ) {
  size_t sz = luaO_str2num( s, s2v( L->top ) );

  if( sz != 0 ) {
    api_incr_top( L );
  }

  return sz;
}

LUA_API lua_Number lua_tonumberx( lua_State *L, int idx, int *pisnum ) {
  lua_Number n = 0;
  const TValue *o = index2value( L, idx );
  int isnum = tonumber( o, &n );

  if( pisnum ) {
    *pisnum = isnum;
  }

  return n;
}

LUA_API lua_Integer lua_tointegerx( lua_State *L, int idx, int *pisnum ) {
  lua_Integer res = 0;
  const TValue *o = index2value( L, idx );
  int isnum = tointeger( o, &res );

  if( pisnum ) {
    *pisnum = isnum;
  }

  return res;
}

LUA_API int lua_toboolean( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );
  return !l_isfalse( o );
}

LUA_API const char *lua_tolstring( lua_State *L, int idx, size_t *len ) {
  TValue *o = index2value( L, idx );

  if( !ttisstring( o ) ) {
    if( !cvt2str( o ) ) { /* not convertible? */
      if( len != NULL ) {
        *len = 0;
      }

      return NULL;
    }

    lua_lock( L ); /* 'luaO_tostring' may create a new string */
    luaO_tostring( L, o );
    luaC_checkGC( L );
    o = index2value( L, idx ); /* previous call may reallocate the stack */
    lua_unlock( L );
  }

  if( len != NULL ) {
    *len = vslen( o );
  }

  return svalue( o );
}

LUA_API lua_Unsigned lua_rawlen( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );

  switch( ttypetag( o ) ) {
    case LUA_VSHRSTR:
      return tsvalue( o )->shrlen;

    case LUA_VLNGSTR:
      return tsvalue( o )->u.lnglen;

    case LUA_VUSERDATA:
      return uvalue( o )->len;

    case LUA_VTABLE:
      return luaH_getn( hvalue( o ) );

    default:
      return 0;
  }
}

LUA_API lua_CFunction lua_tocfunction( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );

  if( ttislcf( o ) ) {
    return fvalue( o );
  } else if( ttisCclosure( o ) ) {
    return clCvalue( o )->f;
  } else {
    return NULL;  /* not a C function */
  }
}

static void *touserdata( const TValue *o ) {
  switch( ttype( o ) ) {
    case LUA_TUSERDATA:
      return getudatamem( uvalue( o ) );

    case LUA_TLIGHTUSERDATA:
      return pvalue( o );

    default:
      return NULL;
  }
}

LUA_API void *lua_touserdata( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );
  return touserdata( o );
}

LUA_API lua_State *lua_tothread( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );
  return ( !ttisthread( o ) ) ? NULL : thvalue( o );
}

/*
** Returns a pointer to the internal representation of an object.
** Note that ANSI C does not allow the conversion of a pointer to
** function to a 'void*', so the conversion here goes through
** a 'size_t'. (As the returned pointer is only informative, this
** conversion should not be a problem.)
*/
LUA_API const void *lua_topointer( lua_State *L, int idx ) {
  const TValue *o = index2value( L, idx );

  switch( ttypetag( o ) ) {
    case LUA_VLCF:
      return cast_voidp( cast_sizet( fvalue( o ) ) );

    case LUA_VUSERDATA:
    case LUA_VLIGHTUSERDATA:
      return touserdata( o );

    default: {
        if( iscollectable( o ) ) {
          return gcvalue( o );
        } else {
          return NULL;
        }
      }
  }
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushnil( lua_State *L ) {
  lua_lock( L );
  setnilvalue( s2v( L->top ) );
  api_incr_top( L );
  lua_unlock( L );
}

LUA_API void lua_pushnumber( lua_State *L, lua_Number n ) {
  lua_lock( L );
  setfltvalue( s2v( L->top ), n );
  api_incr_top( L );
  lua_unlock( L );
}

LUA_API void lua_pushinteger( lua_State *L, lua_Integer n ) {
  lua_lock( L );
  setivalue( s2v( L->top ), n );
  api_incr_top( L );
  lua_unlock( L );
}

/*
** Pushes on the stack a string with given length. Avoid using 's' when
** 'len' == 0 (as 's' can be NULL in that case), due to later use of
** 'memcmp' and 'memcpy'.
*/
LUA_API const char *lua_pushlstring( lua_State *L, const char *s, size_t len ) {
  TString *ts;
  lua_lock( L );
  ts = ( len == 0 ) ? luaS_new( L, "" ) : luaS_newlstr( L, s, len );
  setsvalue2s( L, L->top, ts );
  api_incr_top( L );
  luaC_checkGC( L );
  lua_unlock( L );
  return getstr( ts );
}

LUA_API const char *lua_pushstring( lua_State *L, const char *s ) {
  lua_lock( L );

  if( s == NULL ) {
    setnilvalue( s2v( L->top ) );
  } else {
    TString *ts;
    ts = luaS_new( L, s );
    setsvalue2s( L, L->top, ts );
    s = getstr( ts ); /* internal copy's address */
  }

  api_incr_top( L );
  luaC_checkGC( L );
  lua_unlock( L );
  return s;
}

LUA_API const char *lua_pushvfstring( lua_State *L, const char *fmt,
                                      va_list argp ) {
  const char *ret;
  lua_lock( L );
  ret = luaO_pushvfstring( L, fmt, argp );
  luaC_checkGC( L );
  lua_unlock( L );
  return ret;
}

LUA_API const char *lua_pushfstring( lua_State *L, const char *fmt, ... ) {
  const char *ret;
  va_list argp;
  lua_lock( L );
  va_start( argp, fmt );
  ret = luaO_pushvfstring( L, fmt, argp );
  va_end( argp );
  luaC_checkGC( L );
  lua_unlock( L );
  return ret;
}

LUA_API void lua_pushcclosure( lua_State *L, lua_CFunction fn, int n ) {
  lua_lock( L );

  if( n == 0 ) {
    setfvalue( s2v( L->top ), fn );
    api_incr_top( L );
  } else {
    CClosure *cl;
    api_checknelems( L, n );
    api_check( L, n <= MAXUPVAL, "upvalue index too large" );
    cl = luaF_newCclosure( L, n );
    cl->f = fn;
    L->top -= n;

    while( n-- ) {
      setobj2n( L, &cl->upvalue[n], s2v( L->top + n ) );
      /* does not need barrier because closure is white */
    }

    setclCvalue( L, s2v( L->top ), cl );
    api_incr_top( L );
    luaC_checkGC( L );
  }

  lua_unlock( L );
}

LUA_API void lua_pushboolean( lua_State *L, int b ) {
  lua_lock( L );

  if( b ) {
    setbtvalue( s2v( L->top ) );
  } else {
    setbfvalue( s2v( L->top ) );
  }

  api_incr_top( L );
  lua_unlock( L );
}

LUA_API void lua_pushlightuserdata( lua_State *L, void *p ) {
  lua_lock( L );
  setpvalue( s2v( L->top ), p );
  api_incr_top( L );
  lua_unlock( L );
}

LUA_API int lua_pushthread( lua_State *L ) {
  lua_lock( L );
  setthvalue( L, s2v( L->top ), L );
  api_incr_top( L );
  lua_unlock( L );
  return ( G( L )->mainthread == L );
}

/*
** get functions (Lua -> stack)
*/

static int auxgetstr( lua_State *L, const TValue *t, const char *k ) {
  const TValue *slot;
  TString *str = luaS_new( L, k );

  if( luaV_fastget( L, t, str, slot, luaH_getstr ) ) {
    setobj2s( L, L->top, slot );
    api_incr_top( L );
  } else {
    setsvalue2s( L, L->top, str );
    api_incr_top( L );
    luaV_finishget( L, t, s2v( L->top - 1 ), L->top - 1, slot );
  }

  lua_unlock( L );
  return ttype( s2v( L->top - 1 ) );
}

LUA_API int lua_getglobal( lua_State *L, const char *name ) {
  Table *reg = hvalue( &G( L )->l_registry );
  lua_lock( L );
  return auxgetstr( L, luaH_getint( reg, LUA_RIDX_GLOBALS ), name );
}

LUA_API int lua_gettable( lua_State *L, int idx ) {
  const TValue *slot;
  TValue *t;
  lua_lock( L );
  t = index2value( L, idx );

  if( luaV_fastget( L, t, s2v( L->top - 1 ), slot, luaH_get ) ) {
    setobj2s( L, L->top - 1, slot );
  } else {
    luaV_finishget( L, t, s2v( L->top - 1 ), L->top - 1, slot );
  }

  lua_unlock( L );
  return ttype( s2v( L->top - 1 ) );
}

LUA_API int lua_getfield( lua_State *L, int idx, const char *k ) {
  lua_lock( L );
  return auxgetstr( L, index2value( L, idx ), k );
}

LUA_API int lua_geti( lua_State *L, int idx, lua_Integer n ) {
  TValue *t;
  const TValue *slot;
  lua_lock( L );
  t = index2value( L, idx );

  if( luaV_fastgeti( L, t, n, slot ) ) {
    setobj2s( L, L->top, slot );
  } else {
    TValue aux;
    setivalue( &aux, n );
    luaV_finishget( L, t, &aux, L->top, slot );
  }

  api_incr_top( L );
  lua_unlock( L );
  return ttype( s2v( L->top - 1 ) );
}

static int finishrawget( lua_State *L, const TValue *val ) {
  if( isempty( val ) ) { /* avoid copying empty items to the stack */
    setnilvalue( s2v( L->top ) );
  } else {
    setobj2s( L, L->top, val );
  }

  api_incr_top( L );
  lua_unlock( L );
  return ttype( s2v( L->top - 1 ) );
}

static Table *gettable( lua_State *L, int idx ) {
  TValue *t = index2value( L, idx );
  api_check( L, ttistable( t ), "table expected" );
  return hvalue( t );
}

LUA_API int lua_rawget( lua_State *L, int idx ) {
  Table *t;
  const TValue *val;
  lua_lock( L );
  api_checknelems( L, 1 );
  t = gettable( L, idx );
  val = luaH_get( t, s2v( L->top - 1 ) );
  L->top--;  /* remove key */
  return finishrawget( L, val );
}

LUA_API int lua_rawgeti( lua_State *L, int idx, lua_Integer n ) {
  Table *t;
  lua_lock( L );
  t = gettable( L, idx );
  return finishrawget( L, luaH_getint( t, n ) );
}

LUA_API int lua_rawgetp( lua_State *L, int idx, const void *p ) {
  Table *t;
  TValue k;
  lua_lock( L );
  t = gettable( L, idx );
  setpvalue( &k, cast_voidp( p ) );
  return finishrawget( L, luaH_get( t, &k ) );
}

LUA_API void lua_createtable( lua_State *L, int narray, int nrec ) {
  Table *t;
  lua_lock( L );
  t = luaH_new( L );
  sethvalue2s( L, L->top, t );
  api_incr_top( L );

  if( narray > 0 || nrec > 0 ) {
    luaH_resize( L, t, narray, nrec );
  }

  luaC_checkGC( L );
  lua_unlock( L );
}

LUA_API int lua_getmetatable( lua_State *L, int objindex ) {
  const TValue *obj;
  Table *mt;
  int res = 0;
  lua_lock( L );
  obj = index2value( L, objindex );

  switch( ttype( obj ) ) {
    case LUA_TTABLE:
      mt = hvalue( obj )->metatable;
      break;

    case LUA_TUSERDATA:
      mt = uvalue( obj )->metatable;
      break;

    default:
      mt = G( L )->mt[ttype( obj )];
      break;
  }

  if( mt != NULL ) {
    sethvalue2s( L, L->top, mt );
    api_incr_top( L );
    res = 1;
  }

  lua_unlock( L );
  return res;
}

LUA_API int lua_getiuservalue( lua_State *L, int idx, int n ) {
  TValue *o;
  int t;
  lua_lock( L );
  o = index2value( L, idx );
  api_check( L, ttisfulluserdata( o ), "full userdata expected" );

  if( n <= 0 || n > uvalue( o )->nuvalue ) {
    setnilvalue( s2v( L->top ) );
    t = LUA_TNONE;
  } else {
    setobj2s( L, L->top, &uvalue( o )->uv[n - 1].uv );
    t = ttype( s2v( L->top ) );
  }

  api_incr_top( L );
  lua_unlock( L );
  return t;
}

/*
** set functions (stack -> Lua)
*/

/*
** t[k] = value at the top of the stack (where 'k' is a string)
*/
static void auxsetstr( lua_State *L, const TValue *t, const char *k ) {
  const TValue *slot;
  TString *str = luaS_new( L, k );
  api_checknelems( L, 1 );

  if( luaV_fastget( L, t, str, slot, luaH_getstr ) ) {
    luaV_finishfastset( L, t, slot, s2v( L->top - 1 ) );
    L->top--;  /* pop value */
  } else {
    setsvalue2s( L, L->top, str ); /* push 'str' (to make it a TValue) */
    api_incr_top( L );
    luaV_finishset( L, t, s2v( L->top - 1 ), s2v( L->top - 2 ), slot );
    L->top -= 2;  /* pop value and key */
  }

  lua_unlock( L ); /* lock done by caller */
}

LUA_API void lua_setglobal( lua_State *L, const char *name ) {
  Table *reg = hvalue( &G( L )->l_registry );
  lua_lock( L ); /* unlock done in 'auxsetstr' */
  auxsetstr( L, luaH_getint( reg, LUA_RIDX_GLOBALS ), name );
}

LUA_API void lua_settable( lua_State *L, int idx ) {
  TValue *t;
  const TValue *slot;
  lua_lock( L );
  api_checknelems( L, 2 );
  t = index2value( L, idx );

  if( luaV_fastget( L, t, s2v( L->top - 2 ), slot, luaH_get ) ) {
    luaV_finishfastset( L, t, slot, s2v( L->top - 1 ) );
  } else {
    luaV_finishset( L, t, s2v( L->top - 2 ), s2v( L->top - 1 ), slot );
  }

  L->top -= 2;  /* pop index and value */
  lua_unlock( L );
}

LUA_API void lua_setfield( lua_State *L, int idx, const char *k ) {
  lua_lock( L ); /* unlock done in 'auxsetstr' */
  auxsetstr( L, index2value( L, idx ), k );
}

LUA_API void lua_seti( lua_State *L, int idx, lua_Integer n ) {
  TValue *t;
  const TValue *slot;
  lua_lock( L );
  api_checknelems( L, 1 );
  t = index2value( L, idx );

  if( luaV_fastgeti( L, t, n, slot ) ) {
    luaV_finishfastset( L, t, slot, s2v( L->top - 1 ) );
  } else {
    TValue aux;
    setivalue( &aux, n );
    luaV_finishset( L, t, &aux, s2v( L->top - 1 ), slot );
  }

  L->top--;  /* pop value */
  lua_unlock( L );
}

static void aux_rawset( lua_State *L, int idx, TValue *key, int n ) {
  Table *t;
  TValue *slot;
  lua_lock( L );
  api_checknelems( L, n );
  t = gettable( L, idx );
  slot = luaH_set( L, t, key );
  setobj2t( L, slot, s2v( L->top - 1 ) );
  invalidateTMcache( t );
  luaC_barrierback( L, obj2gco( t ), s2v( L->top - 1 ) );
  L->top -= n;
  lua_unlock( L );
}

LUA_API void lua_rawset( lua_State *L, int idx ) {
  aux_rawset( L, idx, s2v( L->top - 2 ), 2 );
}

LUA_API void lua_rawsetp( lua_State *L, int idx, const void *p ) {
  TValue k;
  setpvalue( &k, cast_voidp( p ) );
  aux_rawset( L, idx, &k, 1 );
}

LUA_API void lua_rawseti( lua_State *L, int idx, lua_Integer n ) {
  Table *t;
  lua_lock( L );
  api_checknelems( L, 1 );
  t = gettable( L, idx );
  luaH_setint( L, t, n, s2v( L->top - 1 ) );
  luaC_barrierback( L, obj2gco( t ), s2v( L->top - 1 ) );
  L->top--;
  lua_unlock( L );
}

LUA_API int lua_setmetatable( lua_State *L, int objindex ) {
  TValue *obj;
  Table *mt;
  lua_lock( L );
  api_checknelems( L, 1 );
  obj = index2value( L, objindex );

  if( ttisnil( s2v( L->top - 1 ) ) ) {
    mt = NULL;
  } else {
    api_check( L, ttistable( s2v( L->top - 1 ) ), "table expected" );
    mt = hvalue( s2v( L->top - 1 ) );
  }

  switch( ttype( obj ) ) {
    case LUA_TTABLE: {
        hvalue( obj )->metatable = mt;

        if( mt ) {
          luaC_objbarrier( L, gcvalue( obj ), mt );
          luaC_checkfinalizer( L, gcvalue( obj ), mt );
        }

        break;
      }

    case LUA_TUSERDATA: {
        uvalue( obj )->metatable = mt;

        if( mt ) {
          luaC_objbarrier( L, uvalue( obj ), mt );
          luaC_checkfinalizer( L, gcvalue( obj ), mt );
        }

        break;
      }

    default: {
        G( L )->mt[ttype( obj )] = mt;
        break;
      }
  }

  L->top--;
  lua_unlock( L );
  return 1;
}

LUA_API int lua_setiuservalue( lua_State *L, int idx, int n ) {
  TValue *o;
  int res;
  lua_lock( L );
  api_checknelems( L, 1 );
  o = index2value( L, idx );
  api_check( L, ttisfulluserdata( o ), "full userdata expected" );

  if( !( cast_uint( n ) - 1u < cast_uint( uvalue( o )->nuvalue ) ) ) {
    res = 0;  /* 'n' not in [1, uvalue(o)->nuvalue] */
  } else {
    setobj( L, &uvalue( o )->uv[n - 1].uv, s2v( L->top - 1 ) );
    luaC_barrierback( L, gcvalue( o ), s2v( L->top - 1 ) );
    res = 1;
  }

  L->top--;
  lua_unlock( L );
  return res;
}

/*
** 'load' and 'call' functions (run Lua code)
*/

#define checkresults(L,na,nr) \
  api_check(L, (nr) == LUA_MULTRET || (L->ci->top - L->top >= (nr) - (na)), \
            "results from function overflow current stack size")

LUA_API void lua_callk( lua_State *L, int nargs, int nresults,
                        lua_KContext ctx, lua_KFunction k ) {
  StkId func;
  lua_lock( L );
  api_check( L, k == NULL || !isLua( L->ci ),
             "cannot use continuations inside hooks" );
  api_checknelems( L, nargs + 1 );
  api_check( L, L->status == LUA_OK, "cannot do calls on non-normal thread" );
  checkresults( L, nargs, nresults );
  func = L->top - ( nargs + 1 );

  if( k != NULL && yieldable( L ) ) { /* need to prepare continuation? */
    L->ci->u.c.k = k;  /* save continuation */
    L->ci->u.c.ctx = ctx;  /* save context */
    luaD_call( L, func, nresults ); /* do the call */
  } else { /* no continuation or no yieldable */
    luaD_callnoyield( L, func, nresults );  /* just do the call */
  }

  adjustresults( L, nresults );
  lua_unlock( L );
}

/*
** Execute a protected call.
*/
struct CallS {  /* data to 'f_call' */
  StkId func;
  int nresults;
};

static void f_call( lua_State *L, void *ud ) {
  struct CallS *c = cast( struct CallS *, ud );
  luaD_callnoyield( L, c->func, c->nresults );
}

LUA_API int lua_pcallk( lua_State *L, int nargs, int nresults, int errfunc,
                        lua_KContext ctx, lua_KFunction k ) {
  struct CallS c;
  int status;
  ptrdiff_t func;
  lua_lock( L );
  api_check( L, k == NULL || !isLua( L->ci ),
             "cannot use continuations inside hooks" );
  api_checknelems( L, nargs + 1 );
  api_check( L, L->status == LUA_OK, "cannot do calls on non-normal thread" );
  checkresults( L, nargs, nresults );

  if( errfunc == 0 ) {
    func = 0;
  } else {
    StkId o = index2stack( L, errfunc );
    api_check( L, ttisfunction( s2v( o ) ), "error handler must be a function" );
    func = savestack( L, o );
  }

  c.func = L->top - ( nargs + 1 ); /* function to be called */

  if( k == NULL || !yieldable( L ) ) { /* no continuation or no yieldable? */
    c.nresults = nresults;  /* do a 'conventional' protected call */
    status = luaD_pcall( L, f_call, &c, savestack( L, c.func ), func );
  } else { /* prepare continuation (call is already protected by 'resume') */
    CallInfo *ci = L->ci;
    ci->u.c.k = k;  /* save continuation */
    ci->u.c.ctx = ctx;  /* save context */
    /* save information for error recovery */
    ci->u2.funcidx = cast_int( savestack( L, c.func ) );
    ci->u.c.old_errfunc = L->errfunc;
    L->errfunc = func;
    setoah( ci->callstatus, L->allowhook ); /* save value of 'allowhook' */
    ci->callstatus |= CIST_YPCALL;  /* function can do error recovery */
    luaD_call( L, c.func, nresults ); /* do the call */
    ci->callstatus &= ~CIST_YPCALL;
    L->errfunc = ci->u.c.old_errfunc;
    status = LUA_OK;  /* if it is here, there were no errors */
  }

  adjustresults( L, nresults );
  lua_unlock( L );
  return status;
}

LUA_API int lua_load( lua_State *L, lua_Reader reader, void *data,
                      const char *chunkname, const char *mode ) {
  ZIO z;
  int status;
  lua_lock( L );

  if( !chunkname ) {
    chunkname = "?";
  }

  luaZ_init( L, &z, reader, data );
  status = luaD_protectedparser( L, &z, chunkname, mode );

  if( status == LUA_OK ) { /* no errors? */
    LClosure *f = clLvalue( s2v( L->top - 1 ) ); /* get newly created function */

    if( f->nupvalues >= 1 ) { /* does it have an upvalue? */
      /* get global table from registry */
      Table *reg = hvalue( &G( L )->l_registry );
      const TValue *gt = luaH_getint( reg, LUA_RIDX_GLOBALS );
      /* set global table as 1st upvalue of 'f' (may be LUA_ENV) */
      setobj( L, f->upvals[0]->v, gt );
      luaC_barrier( L, f->upvals[0], gt );
    }
  }

  lua_unlock( L );
  return status;
}

LUA_API int lua_dump( lua_State *L, lua_Writer writer, void *data, int strip ) {
  int status;
  TValue *o;
  lua_lock( L );
  api_checknelems( L, 1 );
  o = s2v( L->top - 1 );

  if( isLfunction( o ) ) {
    status = luaU_dump( L, getproto( o ), writer, data, strip );
  } else {
    status = 1;
  }

  lua_unlock( L );
  return status;
}

LUA_API int lua_status( lua_State *L ) {
  return L->status;
}

/*
** Garbage-collection function
*/
LUA_API int lua_gc( lua_State *L, int what, ... ) {
  va_list argp;
  int res = 0;
  global_State *g = G( L );
  lua_lock( L );
  va_start( argp, what );

  switch( what ) {
    case LUA_GCSTOP: {
        g->gcrunning = 0;
        break;
      }

    case LUA_GCRESTART: {
        luaE_setdebt( g, 0 );
        g->gcrunning = 1;
        break;
      }

    case LUA_GCCOLLECT: {
        luaC_fullgc( L, 0 );
        break;
      }

    case LUA_GCCOUNT: {
        /* GC values are expressed in Kbytes: #bytes/2^10 */
        res = cast_int( gettotalbytes( g ) >> 10 );
        break;
      }

    case LUA_GCCOUNTB: {
        res = cast_int( gettotalbytes( g ) & 0x3ff );
        break;
      }

    case LUA_GCSTEP: {
        int data = va_arg( argp, int );
        l_mem debt = 1;  /* =1 to signal that it did an actual step */
        lu_byte oldrunning = g->gcrunning;
        g->gcrunning = 1;  /* allow GC to run */

        if( data == 0 ) {
          luaE_setdebt( g, 0 ); /* do a basic step */
          luaC_step( L );
        } else { /* add 'data' to total debt */
          debt = cast( l_mem, data ) * 1024 + g->GCdebt;
          luaE_setdebt( g, debt );
          luaC_checkGC( L );
        }

        g->gcrunning = oldrunning;  /* restore previous state */

        if( debt > 0 && g->gcstate == GCSpause ) { /* end of cycle? */
          res = 1;  /* signal it */
        }

        break;
      }

    case LUA_GCSETPAUSE: {
        int data = va_arg( argp, int );
        res = getgcparam( g->gcpause );
        setgcparam( g->gcpause, data );
        break;
      }

    case LUA_GCSETSTEPMUL: {
        int data = va_arg( argp, int );
        res = getgcparam( g->gcstepmul );
        setgcparam( g->gcstepmul, data );
        break;
      }

    case LUA_GCISRUNNING: {
        res = g->gcrunning;
        break;
      }

    case LUA_GCGEN: {
        int minormul = va_arg( argp, int );
        int majormul = va_arg( argp, int );
        res = isdecGCmodegen( g ) ? LUA_GCGEN : LUA_GCINC;

        if( minormul != 0 ) {
          g->genminormul = minormul;
        }

        if( majormul != 0 ) {
          setgcparam( g->genmajormul, majormul );
        }

        luaC_changemode( L, KGC_GEN );
        break;
      }

    case LUA_GCINC: {
        int pause = va_arg( argp, int );
        int stepmul = va_arg( argp, int );
        int stepsize = va_arg( argp, int );
        res = isdecGCmodegen( g ) ? LUA_GCGEN : LUA_GCINC;

        if( pause != 0 ) {
          setgcparam( g->gcpause, pause );
        }

        if( stepmul != 0 ) {
          setgcparam( g->gcstepmul, stepmul );
        }

        if( stepsize != 0 ) {
          g->gcstepsize = stepsize;
        }

        luaC_changemode( L, KGC_INC );
        break;
      }

    default:
      res = -1;  /* invalid option */
  }

  va_end( argp );
  lua_unlock( L );
  return res;
}

/*
** miscellaneous functions
*/

LUA_API int lua_error( lua_State *L ) {
  lua_lock( L );
  api_checknelems( L, 1 );
  luaG_errormsg( L );
  /* code unreachable; will unlock when control actually leaves the kernel */
  return 0;  /* to avoid warnings */
}

LUA_API int lua_next( lua_State *L, int idx ) {
  Table *t;
  int more;
  lua_lock( L );
  api_checknelems( L, 1 );
  t = gettable( L, idx );
  more = luaH_next( L, t, L->top - 1 );

  if( more ) {
    api_incr_top( L );
  } else { /* no more elements */
    L->top -= 1;  /* remove key */
  }

  lua_unlock( L );
  return more;
}

LUA_API void lua_toclose( lua_State *L, int idx ) {
  int nresults;
  StkId o;
  lua_lock( L );
  o = index2stack( L, idx );
  nresults = L->ci->nresults;
  api_check( L, L->openupval == NULL || uplevel( L->openupval ) <= o,
             "marked index below or equal new one" );
  luaF_newtbcupval( L, o ); /* create new to-be-closed upvalue */

  if( !hastocloseCfunc( nresults ) ) { /* function not marked yet? */
    L->ci->nresults = codeNresults( nresults );  /* mark it */
  }

  lua_assert( hastocloseCfunc( L->ci->nresults ) );
  lua_unlock( L );
}

LUA_API void lua_concat( lua_State *L, int n ) {
  lua_lock( L );
  api_checknelems( L, n );

  if( n >= 2 ) {
    luaV_concat( L, n );
  } else if( n == 0 ) { /* push empty string */
    setsvalue2s( L, L->top, luaS_newlstr( L, "", 0 ) );
    api_incr_top( L );
  }

  /* else n == 1; nothing to do */
  luaC_checkGC( L );
  lua_unlock( L );
}

LUA_API void lua_len( lua_State *L, int idx ) {
  TValue *t;
  lua_lock( L );
  t = index2value( L, idx );
  luaV_objlen( L, L->top, t );
  api_incr_top( L );
  lua_unlock( L );
}

LUA_API lua_Alloc lua_getallocf( lua_State *L, void **ud ) {
  lua_Alloc f;
  lua_lock( L );

  if( ud ) {
    *ud = G( L )->ud;
  }

  f = G( L )->frealloc;
  lua_unlock( L );
  return f;
}

LUA_API void lua_setallocf( lua_State *L, lua_Alloc f, void *ud ) {
  lua_lock( L );
  G( L )->ud = ud;
  G( L )->frealloc = f;
  lua_unlock( L );
}

void lua_setwarnf( lua_State *L, lua_WarnFunction f, void *ud ) {
  lua_lock( L );
  G( L )->ud_warn = ud;
  G( L )->warnf = f;
  lua_unlock( L );
}

void lua_warning( lua_State *L, const char *msg, int tocont ) {
  lua_lock( L );
  luaE_warning( L, msg, tocont );
  lua_unlock( L );
}

LUA_API void *lua_newuserdatauv( lua_State *L, size_t size, int nuvalue ) {
  Udata *u;
  lua_lock( L );
  api_check( L, 0 <= nuvalue && nuvalue < USHRT_MAX, "invalid value" );
  u = luaS_newudata( L, size, nuvalue );
  setuvalue( L, s2v( L->top ), u );
  api_incr_top( L );
  luaC_checkGC( L );
  lua_unlock( L );
  return getudatamem( u );
}

static const char *aux_upvalue( TValue *fi, int n, TValue **val,
                                GCObject **owner ) {
  switch( ttypetag( fi ) ) {
    case LUA_VCCL: {  /* C closure */
        CClosure *f = clCvalue( fi );

        if( !( cast_uint( n ) - 1u < cast_uint( f->nupvalues ) ) ) {
          return NULL;  /* 'n' not in [1, f->nupvalues] */
        }

        *val = &f->upvalue[n - 1];

        if( owner ) {
          *owner = obj2gco( f );
        }

        return "";
      }

    case LUA_VLCL: {  /* Lua closure */
        LClosure *f = clLvalue( fi );
        TString *name;
        Proto *p = f->p;

        if( !( cast_uint( n ) - 1u  < cast_uint( p->sizeupvalues ) ) ) {
          return NULL;  /* 'n' not in [1, p->sizeupvalues] */
        }

        *val = f->upvals[n - 1]->v;

        if( owner ) {
          *owner = obj2gco( f->upvals[n - 1] );
        }

        name = p->upvalues[n - 1].name;
        return ( name == NULL ) ? "(no name)" : getstr( name );
      }

    default:
      return NULL;  /* not a closure */
  }
}

LUA_API const char *lua_getupvalue( lua_State *L, int funcindex, int n ) {
  const char *name;
  TValue *val = NULL;  /* to avoid warnings */
  lua_lock( L );
  name = aux_upvalue( index2value( L, funcindex ), n, &val, NULL );

  if( name ) {
    setobj2s( L, L->top, val );
    api_incr_top( L );
  }

  lua_unlock( L );
  return name;
}

LUA_API const char *lua_setupvalue( lua_State *L, int funcindex, int n ) {
  const char *name;
  TValue *val = NULL;  /* to avoid warnings */
  GCObject *owner = NULL;  /* to avoid warnings */
  TValue *fi;
  lua_lock( L );
  fi = index2value( L, funcindex );
  api_checknelems( L, 1 );
  name = aux_upvalue( fi, n, &val, &owner );

  if( name ) {
    L->top--;
    setobj( L, val, s2v( L->top ) );
    luaC_barrier( L, owner, val );
  }

  lua_unlock( L );
  return name;
}

static UpVal **getupvalref( lua_State *L, int fidx, int n, LClosure **pf ) {
  LClosure *f;
  TValue *fi = index2value( L, fidx );
  api_check( L, ttisLclosure( fi ), "Lua function expected" );
  f = clLvalue( fi );
  api_check( L, ( 1 <= n && n <= f->p->sizeupvalues ), "invalid upvalue index" );

  if( pf ) {
    *pf = f;
  }

  return &f->upvals[n - 1];  /* get its upvalue pointer */
}

LUA_API void *lua_upvalueid( lua_State *L, int fidx, int n ) {
  TValue *fi = index2value( L, fidx );

  switch( ttypetag( fi ) ) {
    case LUA_VLCL: {  /* lua closure */
        return *getupvalref( L, fidx, n, NULL );
      }

    case LUA_VCCL: {  /* C closure */
        CClosure *f = clCvalue( fi );
        api_check( L, 1 <= n && n <= f->nupvalues, "invalid upvalue index" );
        return &f->upvalue[n - 1];
      }

    default: {
        api_check( L, 0, "closure expected" );
        return NULL;
      }
  }
}

LUA_API void lua_upvaluejoin( lua_State *L, int fidx1, int n1,
                              int fidx2, int n2 ) {
  LClosure *f1;
  UpVal **up1 = getupvalref( L, fidx1, n1, &f1 );
  UpVal **up2 = getupvalref( L, fidx2, n2, NULL );
  *up1 = *up2;
  luaC_objbarrier( L, f1, *up1 );
}

