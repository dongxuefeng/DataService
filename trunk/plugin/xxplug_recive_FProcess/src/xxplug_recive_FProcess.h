#ifndef _XXPLUG_RECIVE_UPDATAVIODATA_
#define _XXPLUG_RECIVE_UPDATAVIODATA_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#define TYPE 11
#define TYPESTR "0011"

#include <cxx_xml.h>

int getticketid(cxx_xml xx_xml, char **outxmldata);
int cancleticketid(cxx_xml xx_xml, char **outxmldata);
int putResult(cxx_xml xx_xml, char **outxmldata);
int SearchVio(cxx_xml xx_xml, char **outxmldata);
int WorkSearchVio(cxx_xml xx_xml, char **outxmldata);
int DetailVio(cxx_xml xx_xml, char **outxmldata);
int DetailVioDispose(cxx_xml xx_xml, char **outxmldata);
int SearchQZCS(cxx_xml xx_xml, char **outxmldata);
int SearchAccidentDuty(cxx_xml xx_xml, char **outxmldata);
int DetailAccidentDuty(cxx_xml xx_xml, char **outxmldata);
int AddNumber(cxx_xml xx_xml, char **outxmldata);
int SearchNumber(cxx_xml xx_xml, char **outxmldata);

int getIDLeaveLine(cxx_xml xx_xml, char **outxmldata);

#endif
