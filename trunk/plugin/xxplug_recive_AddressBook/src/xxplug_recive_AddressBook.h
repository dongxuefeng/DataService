#ifndef _XXPLUG_RECIVE_GPS_
#define _XXPLUG_RECIVE_GPS_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#include <cxx_xml.h>

#define TYPE 16
#define TYPESTR "0016"

//web
int InsertLinkman(cxx_xml xx_xml, char ** outxmldata);
int DeleteLinkman(cxx_xml xx_xml, char ** outxmldata);
int UpdateLinkman(cxx_xml xx_xml, char ** outxmldata);
int SelectLinkman(cxx_xml xx_xml, char ** outxmldata);
int SelectGroup(cxx_xml xx_xml, char ** outxmldata);
//phone
int GetAddressBookFileURL(cxx_xml xx_xml, char ** outxmldata);
#endif
