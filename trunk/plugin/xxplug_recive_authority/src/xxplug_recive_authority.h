#ifndef _XXPLUG_RECIVE_UPDATAVIODATA_
#define _XXPLUG_RECIVE_UPDATAVIODATA_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#define TYPE 15
#define TYPESTR "0015"

#include <cxx_xml.h>

int InsertData(cxx_xml xx_xml, char **outxmldata);
int DeleteData(cxx_xml xx_xml, char **outxmldata);
int RelevanceData(cxx_xml xx_xml, char **outxmldata);
int GetAllData(cxx_xml xx_xml, char **outxmldata);
int GetPartData(cxx_xml xx_xml, char **outxmldata);
int GetModuleData(cxx_xml xx_xml, char **outxmldata);

#endif
