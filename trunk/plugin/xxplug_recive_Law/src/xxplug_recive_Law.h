#ifndef _XXPLUG_RECIVE_GPS_
#define _XXPLUG_RECIVE_GPS_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#include <cxx_xml.h>

#define TYPE 12
#define TYPESTR "0012"

bool getFather(char * father);

//Law
//web
int insertLaw(cxx_xml xx_xml, char **outxmldata);
int deleteLaw(cxx_xml xx_xml, char **outxmldata);
int updateLaw(cxx_xml xx_xml, char **outxmldata);
int searchLaw(cxx_xml xx_xml, char **outxmldata);
//phone
int getLaw(cxx_xml xx_xml, char **outxmldata);
int getKeyList(cxx_xml xx_xml, char **outxmldata);
int getVioTypeList(cxx_xml xx_xml, char **outxmldata);

//plan
//web
//plan
int insertPlan(cxx_xml xx_xml, char **outxmldata);
int deletePlan(cxx_xml xx_xml, char **outxmldata);
int searchPlan(cxx_xml xx_xml, char **outxmldata);
int searchPlanDetail(cxx_xml xx_xml, char **outxmldata);

int IsUsePlanName(cxx_xml xx_xml, char **outxmldata);
//addr
int insertAddr(cxx_xml xx_xml, char **outxmldata);
int deleteAddr(cxx_xml xx_xml, char **outxmldata);
int updateAddr(cxx_xml xx_xml, char **outxmldata);
int searchAddr(cxx_xml xx_xml, char **outxmldata);
//org
int insertOrg(cxx_xml xx_xml, char **outxmldata);
int deleteOrg(cxx_xml xx_xml, char **outxmldata);
int updateOrg(cxx_xml xx_xml, char **outxmldata);
int searchOrg(cxx_xml xx_xml, char **outxmldata);

int getAllOrg(cxx_xml xx_xml, char **outxmldata);

//phone
int getAddPlan(cxx_xml xx_xml, char **outxmldata);
int getAll(cxx_xml xx_xml, char **outxmldata);

//vio
//web
int insertVioArticle(cxx_xml xx_xml, char **outxmldata);
int deleteVioArticle(cxx_xml xx_xml, char **outxmldata);
int updateVioArticle(cxx_xml xx_xml, char **outxmldata);
int searchVioArticle(cxx_xml xx_xml, char **outxmldata);
//phone
int getVio(cxx_xml xx_xml, char **outxmldata);
int getVioKeyList(cxx_xml xx_xml, char **outxmldata);

#endif
