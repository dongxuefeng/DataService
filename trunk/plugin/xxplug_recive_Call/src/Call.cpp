#include "xxplug_recive_Call.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <xxxml.h>

using namespace std;

extern MYSQL * m_sql;
extern string gresXML;
extern string username;
extern vector<string> cmdList;

//Web
//通话请求
int CallStart(cxx_xml xx_xml, char ** outxmldata)
{
	string resxml = "<resultdata><item1>";
	string cmd = "";
	char * buffer = NULL;
	int calltype = atoi(xx_xml.xpath_getprop("/userdata/data/callType", "value"));

	//由呼叫类型区分  0 网络呼叫  1 电话呼叫
	switch(calltype)
	{
	case 0:
		{
			cmd = "THQQ";
			cmd += xx_xml.xpath_getprop("/userdata/data/callName", "value");
			printf("cmd = %s\ncmd size = %d\n", cmd.c_str(), cmd.size());
			cmdList.push_back(cmd);
		}
		break;
	case 1:
		{

		}
		break;
	default:
		break;
	}

	resxml += new_strprop("res", "value", "1");
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 1, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

//通话结束
int CallStop(cxx_xml xx_xml, char ** outxmldata)
{
	string resxml = "<resultdata><item1>";
	string cmd ="";

	cmd = "THJS";
	cmdList.push_back(cmd);
	printf("cmd = %s\ncmd size = %d\n", cmd.c_str(), cmd.size());

	resxml += new_strprop("res", "value", "1");
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 1, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}
