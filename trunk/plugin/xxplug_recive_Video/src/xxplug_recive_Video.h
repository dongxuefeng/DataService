#ifndef _XXPLUG_RECIVE_UPDATAVIODATA_
#define _XXPLUG_RECIVE_UPDATAVIODATA_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#define TYPE 14
#define TYPESTR "0014"

#include <cxx_xml.h>

typedef struct
{
	char serverIP[21];
	int  rtspPhonePort;
	int  rtspSvrPort;
	int  rtspCliUDPPort;
	void *serverhandle;
	int  serverID;
	void *nvrhandle;
	int  nvrID;
	char dstuser[255];
	int  clientcount;
	int  serverStart;
} SPServer, *pSPServer;

int PhoneLogin(cxx_xml xx_xml, char **outxmldata);
int GetUserList(cxx_xml xx_xml, char **outxmldata);
int StartVideo(cxx_xml xx_xml, char **outxmldata);
int StopVideo(cxx_xml xx_xml, char **outxmldata);

int GetVideoFileList(cxx_xml xx_xml, char **outxmldata);

#endif
