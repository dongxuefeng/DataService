#ifndef TestDll_H_
#define TestDll_H_

#ifdef MYLIBDLL
#define MYLIBDLL extern "C" _declspec(dllimport)
#else
#define MYLIBDLL extern "C" _declspec(dllexport)
#endif

MYLIBDLL int bmp2jpeg(const char* srcName, const char* destName);
MYLIBDLL int jpeg2bmp(const char* srcName, const char* destName);
MYLIBDLL int bmpzoom(const char * srcfile, const char * dstfile, int newWidth, int newHeight);
MYLIBDLL int jpegzoom(const char * srcfile, const char * dstfile, int newWidth, int newHeight);

#endif