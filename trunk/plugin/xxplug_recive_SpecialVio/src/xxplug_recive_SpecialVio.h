#ifndef _XXPLUG_RECIVE_GPS_
#define _XXPLUG_RECIVE_GPS_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#include <cxx_xml.h>

#define TYPE 13
#define TYPESTR "0013"

int insertExamine(cxx_xml xx_xml, char **outxmldata);
int webGetAuto(cxx_xml xx_xml, char **outxmldata);
int webSetAuto(cxx_xml xx_xml, char **outxmldata);
int webGetExamine(cxx_xml xx_xml, char **outxmldata);
int webSendResult(cxx_xml xx_xml, char **outxmldata);
int uploadPic(cxx_xml xx_xml, char **outxmldata);
int downloadPic(cxx_xml xx_xml, char **outxmldata);
int callback(cxx_xml xx_xml, char **outxmldata);

//int SavePicFile(const char * filename, const char * filedata);

#endif
