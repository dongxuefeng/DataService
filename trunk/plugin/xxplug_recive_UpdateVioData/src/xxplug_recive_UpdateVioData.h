#ifndef _XXPLUG_RECIVE_UPDATAVIODATA_
#define _XXPLUG_RECIVE_UPDATAVIODATA_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#define TYPE 0002
#define TYPESTR "0002"

#include <cxx_xml.h>

int insertViodata(cxx_xml xx_xml, char ** outxmldata);
int affirmVioData(cxx_xml xx_xml, char ** outxmldata);
int BlackIsUpdate(cxx_xml xx_xml, char ** outxmldata);
int GetBlackCount(cxx_xml xx_xml, char ** outxmldata);
int GetBlack(cxx_xml xx_xml, char ** outxmldata);
int InsertBlack(cxx_xml xx_xml, char ** outxmldata);
int DeleteBlack(cxx_xml xx_xml, char ** outxmldata);
int UpdateBlack(cxx_xml xx_xml, char ** outxmldata);
int SelectBlack(cxx_xml xx_xml, char ** outxmldata);
int insertSpotDispose(cxx_xml xx_xml, char ** outxmldata);
int insertDisposePic(cxx_xml xx_xml, char ** outxmldata);
int selectLocalViodate(cxx_xml xx_xml, char ** outxmldata);
int selectLocalViopic(cxx_xml xx_xml, char ** outxmldata);
int selectUnaffirmData(cxx_xml xx_xml, char ** outxmldata);
int selectAffirmData(cxx_xml xx_xml, char ** outxmldata);
int selectAffirmPic(cxx_xml xx_xml, char ** outxmldata);
int packAffirmData(cxx_xml xx_xml, char ** outxmldata);
int selectPackList(cxx_xml xx_xml, char ** outxmldata);
int deletePack(cxx_xml xx_xml, char ** outxmldata);
int downloadPack(cxx_xml xx_xml, char ** outxmldata);
int setOutDate(cxx_xml xx_xml, char ** outxmldata);
int getOutDateAndCount(cxx_xml xx_xml, char ** outxmldata);

#endif
