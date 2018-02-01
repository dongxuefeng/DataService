#ifndef _XXPLUG_DEFINE_ReviceMessage_H__20140318__
#define _XXPLUG_DEFINE_ReviceMessage_H__20140318__

#include <vector>
#include <string.h>
#include <string>
using namespace std;

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#endif
