#ifndef _XXPLUG_DEFINE_H__20140318__
#define _XXPLUG_DEFINE_H__20140318__

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#endif
