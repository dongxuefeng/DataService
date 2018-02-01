/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <xxxml.h>
#include "xxplug_recive_Register.h"
#include "MD5.h"
#include "myAes.h"

using namespace std;

MYSQL * m_sql = 0;
string gresXML;
string username = "";
MD5_CTX md5;

int xx_rcv_gettype()
{
	printf("--xxplug_recive_Register--\n");
	connectMySQL(m_sql);
	MD5Init(&md5);

	return TYPE;
}

int xx_rcv_procinfo(char * inxmldata, char ** outxmldata)
{
	cxx_xml xx_xml;
	xx_xml.readxml(inxmldata);

	string str = xx_xml.xpath_getprop("/userdata/data/type", "value");

	printf("*** %s.%s *** IP: %s\n", TYPESTR, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));

	//phone
	if(str.compare("0001") == 0)
		return PhoneRegister(xx_xml, outxmldata);

	//client
	if(str.compare("0002") == 0)
		return GetNewRegisterCode(xx_xml, outxmldata);

	if(str.compare("0003") == 0)
		return GetOldRegisterCode(xx_xml, outxmldata);

	return 0;
}
