#cmakedefine ALLEGRO_BCC32
#cmakedefine ALLEGRO_BEOS
#cmakedefine ALLEGRO_DJGPP
#cmakedefine ALLEGRO_DMC
#cmakedefine ALLEGRO_HAIKU
#cmakedefine ALLEGRO_MACOSX
#cmakedefine ALLEGRO_MINGW32
#cmakedefine ALLEGRO_MPW
#cmakedefine ALLEGRO_MSVC
#cmakedefine ALLEGRO_PSP
#cmakedefine ALLEGRO_QNX
#cmakedefine ALLEGRO_UNIX
#cmakedefine ALLEGRO_WATCOM
#cmakedefine ALLEGRO_SDL2
#cmakedefine ALLEGRO_UNIX_SDL2

/* These are always defined now. */
#if _MSC_VER && !__INTEL_COMPILER
	#define ALLEGRO_MSVC_SDL2
	#define ALLEGRO_SDL2
#endif

#define ALLEGRO_NO_ASM
#define ALLEGRO_USE_C
