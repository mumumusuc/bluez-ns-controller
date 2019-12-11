#ifndef _DEFS_H_
#define _DEFS_H_

#define _STATIC_INLINE_ static inline
#define _FUNC_ printf("call [%s]\n", __func__)
#define _ARRARY_SIZE(array) sizeof(array) / sizeof(array[0])

#define _u16_le(raw) ((raw)[0] & 0x00FF) | (((raw)[1] << 8) & 0xFF00)
#define _u16_be(raw) ((raw)[1] & 0x00FF) | (((raw)[0] << 8) & 0xFF00)

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DLL
#ifdef __GNUC__
#define API_PUBLIC __attribute__((dllexport))
#else
#define API_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define API_PUBLIC __attribute__((dllimport))
#else
#define API_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define API_HIDE
#else
#if __GNUC__ >= 4
#define API_PUBLIC __attribute__((visibility("default")))
#define API_HIDE __attribute__((visibility("hidden")))
#else
#define API_PUBLIC
#define API_HIDE
#endif
#endif

#endif //  _DEFS_H_#define