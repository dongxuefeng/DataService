#ifndef _XXPLUG_RECIVE_VIEWSCENE_
#define _XXPLUG_RECIVE_VIEWSCENE_
#include <xxplug_define.h>
#include <vector>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <cxx_xml.h>
#include <dbmepolice.h>


#define TYPE 0006
#define TYPESTR "0006"
#define bzero(x,y) memset(x, 0, y)
#define BUFFERSIZE 1024*1024*10
#define SENDCOUNT 10
using namespace std;

int insertAccident(cxx_xml xx_xml, char ** outxmldata);
int updateAccident(cxx_xml xx_xml, char ** outxmldata);
int deleteAccident(cxx_xml xx_xml, char ** outxmldata);
int selectAccident(cxx_xml xx_xml, char ** outxmldata);
int selectAccidentAll(cxx_xml xx_xml, char ** outxmldata);

int selectAccidentProgress(cxx_xml xx_xml, char ** outxmldata);
int tidyAccident(cxx_xml xx_xml, char ** outxmldata);
int insertAccidentpic(cxx_xml xx_xml, char ** outxmldata) ;
int insertAccident_process(cxx_xml,char ** outxmldata);
int selectAccidentpic(cxx_xml xx_xml, char ** outxmldata);

int insertWork(cxx_xml xx_xml, char ** outxmldata);
int deleteWork(cxx_xml xx_xml, char ** outxmldata);
int updateWork(cxx_xml xx_xml, char ** outxmldata);
int selectWork(cxx_xml xx_xml, char ** outxmldata);
int tidyWork(cxx_xml xx_xml, char ** outxmldata);
int selectWorkAll(cxx_xml xx_xml, char ** outxmldata);

int getWorktypeCount(cxx_xml xx_xml, char ** outxmldata);
int getManworkCount(cxx_xml xx_xml, char ** outxmldata);

int GetNameList(string group, vector<string> * nameList);

int GetGroupNameList(vector<string> * nameList);
int GetGroupList(char * father, vector<string> &groupname);

#endif
