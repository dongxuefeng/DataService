#ifndef _WEBCLIENT_H_
#define _WEBCLIENT_H_

extern "C" __declspec(dllexport) void SoapInit();
extern "C" __declspec(dllexport) int  DataInteraction(const char * ip, const int port, const char * src, char * dst, const int outTime);
extern "C" __declspec(dllexport) void SoapDestroy();

#endif