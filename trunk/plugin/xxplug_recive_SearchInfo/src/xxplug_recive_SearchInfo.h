#ifndef _XXPLUG_RECIVE_SEARCHINFO_
#define _XXPLUG_RECIVE_SEARCHINFO_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}
#include <cxx_xml.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <string>

#include <dbmepolice.h>
#include <xxxml.h>
#define TYPE 0001
#define TYPESTR "0001"
#include "webSearch/WebClient.h"

int xx_rcv_getcarinfo(cxx_xml xx_xml, char ** outxmldata, int datafrom);
int xx_rcv_getdriverinfo(cxx_xml xx_xml, char ** outxmldata, int datafrom);
int selectviodatapic(cxx_xml xx_xml, char ** outxmldata, int datafrom);
int selectviodata(cxx_xml xx_xml, char ** outxmldata, int datafrom);
int getcarpic(cxx_xml xx_xml, char ** outxmldata, int datafrom);
int getdriverpic(cxx_xml xx_xml, char ** outxmldata, int datafrom);
int sendPIN(cxx_xml xx_xml, char ** outxmldata);		//发送验证码
int getPinPic(cxx_xml xx_xml, char ** outxmldata);		//重新获取验证码
int CancelSearch(cxx_xml xx_xml, char ** outxmldata);	//取消请求查询

#endif
