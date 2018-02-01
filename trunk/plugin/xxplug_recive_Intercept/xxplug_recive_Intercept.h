#ifndef _XXPLUG_RECIVE_UPDATAVIODATA_
#define _XXPLUG_RECIVE_UPDATAVIODATA_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#define TYPE 9
#define TYPESTR "0009"

#include <cxx_xml.h>

int insertInterceptData(cxx_xml xx_xml, char ** outxmldata);
int selectInterceptData(cxx_xml xx_xml, char ** outxmldata);
int tidyInterceptData(cxx_xml xx_xml, char ** outxmldata);
int selectInterceptPic(cxx_xml xx_xml, char ** outxmldata);
int statisticsWork(cxx_xml xx_xml, char ** outxmldata);
int downPicPath(cxx_xml xx_xml, char ** outxmldata);

#endif
