#ifndef _XXPLUG_RECIVE_WEBHANDER_
#define _XXPLUG_RECIVE_WEBHANDER_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#define TYPE 21
#define TYPESTR "0021"
int CheckUsername(cxx_xml xx_xml, char ** outxmldata);
int InsertResultDataIntoSQL(cxx_xml xx_xml, char ** outxmldata);
string FindUpNameWithVIOID(string vioid);
int WebCheckWebHanderResultWithPageSQL(cxx_xml xx_xml, char ** outxmldata);
int WebCheckLedgerWithPageSQL(cxx_xml xx_xml, char ** outxmldata);
#endif
