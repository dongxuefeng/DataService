#ifndef MAKEZIP_H_
#define MAKEZIP_H_

extern "C" 
{
	// sourceDir = "";
	__declspec(dllexport) void * init();
	__declspec(dllexport) int del(void * obj);
	__declspec(dllexport) int makezip(void * obj, char *sourceDir, char *zipFile);
};


#endif