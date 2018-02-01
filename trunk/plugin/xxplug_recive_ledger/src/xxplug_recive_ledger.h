#ifndef _XXPLUG_RECIVE_LEDGER_
#define _XXPLUG_RECIVE_LEDGER_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#define TYPE 20
#define TYPESTR "0020"
int CheckUsername(cxx_xml xx_xml, char ** outxmldata);
int InsertLedgerDataIntoSQL(cxx_xml xx_xml, char ** outxmldata);
void Getsystemstorepath(void);
int getLedgerfilename(const char * dev, const char * datetime, int index, string &subfileurl);
int savepicfile(const char * filename, const char * filedata);
int WebCheckLedgerWithAllSQL(cxx_xml xx_xml, char ** outxmldata);
int WebCheckLedgerWithPageSQL(cxx_xml xx_xml, char ** outxmldata);
#endif
