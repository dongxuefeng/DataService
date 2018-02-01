// analysisXML.cpp
#include <xxxml.h>
#include "analysisXML.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

bool set_data(char * xmldata,XMLField * xmlField)
{
	memset(xmlField,0,sizeof(XMLFIELD));

	if (NULL != xmldata)
	{
		//printf("analysis start!\n");
		cxx_xml xx_xml;
		xx_xml.readxml(xmldata);
		//printf("/userdata/head/type char*: %s\n", xx_xml.xpath_getprop("/userdata/head/type", "value"));
		xmlField->ntype = atoi(xx_xml.xpath_getprop("/userdata/head/type", "value"));
		//printf("/userdata/head/type int: %d\n", xmlField->ntype);
		return true;
	}
	else
		printf("XML is (null)!\n");

	return false;
}
