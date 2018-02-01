#ifndef _XXPLUG_RECIVE_GPS_
#define _XXPLUG_RECIVE_GPS_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#define TYPE 17
#define TYPESTR "0017"

#endif
