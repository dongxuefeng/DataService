#ifndef _XXPLUG_RECIVE_GPS_
#define _XXPLUG_RECIVE_GPS_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#include <cxx_xml.h>

#define TYPE 19
#define TYPESTR "0019"

//Web
//通话请求
int CallStart(cxx_xml xx_xml, char ** outxmldata);

//通话结束
int CallStop(cxx_xml xx_xml, char ** outxmldata);

#endif
