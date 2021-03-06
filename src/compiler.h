/* compiler.h

   Compiler-specific #defines and such
   for Tux Paint

   Mostly by Albert Cahalan <albert@users.sf.net>
   Copyright (c) 2002-2006
   Licensed in Tux Paint under GPL 2 or later.

   Moved into tuxmath, minimally edited, and relicensed as GPLv3+:
   Copyright (C) 2008, 2009, 2010, 2011.
   Brendan Luchen, David Bruce.
email: <tuxmath-devel@lists.sourceforge.net>

compiler.h is part of "Tux, of Math Command", a.k.a. "tuxmath".

Tuxmath is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Tuxmath is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.  */




#ifdef WIN32
/* Horrible, dangerous macros. */
/*
   The SDL stderr redirection trick doesn't seem to work for perror().
   This does pretty much the same thing.
   */
#define perror(str) ({ \
        if ( (str) && *(str) ) \
        fprintf(stderr,"%s : ",(str)); \
        fprintf(stderr, \
            "%s [%d]\n", \
            (errno<_sys_nerr)?_sys_errlist[errno]:"unknown",errno ); \
        })

/*
   MinGW implementation of isspace() crashes on some Win98 boxes
   if c is 'out-of-range'.
   */
#define isspace(c) (((c) == 0x20) || ((c) >= 0x09 && (c) <= 0x0D))

/*
   WIN32 and MINGW don't have strcasestr().
   */
#define NOMINMAX
#include "shlwapi.h"
#define strcasestr StrStrI
#endif /* WIN32 */




#ifdef __GNUC__
// This version has strict type checking for safety.
// See the "unnecessary" pointer comparison. (from Linux)
#define min(x,y) ({ \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);            \
        _x < _y ? _x : _y; })
#define max(x,y) ({ \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);            \
        _x > _y ? _x : _y; })
#else
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define clamp(lo,value,hi)    (min(max(value,lo),hi))


// since gcc-2.5
#ifdef __GNUC__
#define NORETURN __attribute__((__noreturn__))
#define FUNCTION __attribute__((__const__))     // no access to global mem, even via ptr, and no side effect
#else
#define NORETURN
#define FUNCTION
#endif

#if !defined(restrict) && __STDC_VERSION__ < 199901
#if __GNUC__ > 2 || __GNUC_MINOR__ >= 92
#define restrict __restrict__
#else
#warning No restrict keyword?
#define restrict
#endif
#endif


#if __GNUC__ > 2 || __GNUC_MINOR__ >= 96
// won't alias anything, and aligned enough for anything
#define MALLOC __attribute__ ((__malloc__))
// no side effect, may read globals
#ifndef WIN32
#define PURE __attribute__ ((__pure__))
#endif
// tell gcc what to expect:   if(unlikely(err)) die(err);
#define likely(x)       __builtin_expect(!!(x),1)
#define unlikely(x)     __builtin_expect(!!(x),0)
#define expected(x,y)   __builtin_expect((x),(y))
#else
#define MALLOC
#define PURE
#define likely(x)       (x)
#define unlikely(x)     (x)
#define expected(x,y)   (x)
#endif


#ifdef __powerpc__
// Ticks at 1/4  the memory bus clock (24.907667 MHz on Albert's Mac Cube)
// This is good for 80-second diff or 160-second total.
#define CLOCK_ASM(tbl) asm volatile("mftb %0" : "=r" (tbl))
#define CLOCK_TYPE unsigned long
#ifndef CLOCK_SPEED
// #warning Benchmark times are based on a 99.63 MHz memory bus.
#define CLOCK_SPEED 24907667.0
#endif
#endif

#ifdef __i386__
#define CLOCK_ASM(tbl) asm volatile("rdtsc" : "=A" (tbl))
#define CLOCK_TYPE unsigned long long
#ifndef CLOCK_SPEED
// #warning Benchmark times are based on a 450 MHz CPU.
#define CLOCK_SPEED 450000000.0
#endif
#endif

#ifndef CLOCK_ASM
// #warning No idea how to read CPU cycles for you, sorry.
#define CLOCK_ASM(tbl)
#define CLOCK_TYPE unsigned long
#define CLOCK_SPEED 1000000000.0
#endif

#ifdef NO_ASM
#undef CLOCK_ASM
#define CLOCK_ASM(x) x=42
#endif
