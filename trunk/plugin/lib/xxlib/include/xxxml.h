#ifndef XXXML_H
#define XXXML_H


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <cxx_xml.h>
using namespace std;


inline string AddSelectCondition(const char * condition, const char * value, int type)
{
	static string sql = "";
	string str = value;

	sql = "";
	if(str.compare("") != 0)
	{
		sql = " AND ";
		sql += condition;
		if(type == 1) sql += " '";
		sql += str;
		if(type == 1) sql += "'";
	}

	return sql;
};

inline string AddOrSelectCondition(const char * condition, const char * value, int type)
{
	static string sql = "";
	string str = value;

	sql = "";
	if(str.compare("") != 0)
	{
		sql = " OR ";
		sql += condition;
		if(type == 1) sql += " '";
		sql += str;
		if(type == 1) sql += "'";
	}

	return sql;
};


inline bool _set_data(char * xmldata,int icount,string * pstrkeynamelist,string * pstrvaluelist)
{

	bool bresult = true;
	cxx_xml gcxx_xml;
	gcxx_xml.readxml(xmldata);
	//gcxx_xml.print_xmldata();


	for(int i = 0; i < icount; i++)
	{
		bresult  = bresult  &&
				gcxx_xml.xpath_getprop((pstrkeynamelist+i)->c_str(),"value"
						,*(pstrvaluelist+i));
	};
	return bresult;
}

inline const char * itoaa(int ivalue)
{
	static char strtmp1[256];
	sprintf(strtmp1,"%d",ivalue);
	return strtmp1;
};

inline const char * setResItem(int index,string ItemPoxy)
{
	char strtmp[20];
	sprintf(strtmp, "item%d",index);
	static string str;
	str = "<";
	str += strtmp;
	str += ">";
	str += ItemPoxy;
	str += "</";
	str += strtmp;
	str += ">";
	return str.c_str();
};
inline void setResXML(string type,int count,int sumcount,int FirstIndex,string poxyXML,string &resXML)
{

	resXML = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	resXML += "<userdata><head>";
	resXML += "<type value=\""+type+"\"/>"
				+"<count value=\"";
	resXML += itoaa(count);
	resXML += "\"/>";
	resXML += "<sumcount value=\"";
	resXML += itoaa(sumcount);
	resXML += "\"/>";
	resXML += "<FirstIndex value=\"";
	resXML += +itoaa(FirstIndex);
	resXML += "\"/>";
	resXML += "</head>";
	resXML += poxyXML.c_str();
	resXML += "</userdata>";
};

inline void setValTag(const char * str, int val, string * strXML)
{
	char tmp[128] = {0};
	sprintf(tmp, "<%s=\"%d\"/>", str, val);
	*strXML += tmp;
}

inline void setValTag(const char * str, const char * val, string * strXML)
{
	char tmp[128] = {0};
	sprintf(tmp, "<%s=\"%s\"/>", str, val);
	*strXML += tmp;
}

inline void setNumTag(const char *str, int val, string * strXML)
{
	char tmp[128] = {0};
	sprintf(tmp, "<%s%d>", str, val);
	*strXML += tmp;
}

inline void setHead(const char *type, int count, int sumcount, int firstIndex,const char *str, string * strXML)
{

	*strXML = "<?xml version=\"1.0\" encoding=\"utf-8\"?><userdata><head>";
	setValTag("type value", type, strXML);
	setValTag("count value", count, strXML);
	setValTag("sumcount value", sumcount, strXML);
	setValTag("FirstIndex value", firstIndex, strXML);
	*strXML += "</head><resultdata>";
	*strXML += str;
	*strXML += "</resultdata></userdata>";
}

inline const char * new_strprop(const char * tagname,const char * propname,const char * propvalue)
{
	static string strtmp = "" ;
	strtmp = "";
	strtmp = strtmp+"<"+ tagname+" "+propname+"=\"";
	if(propvalue)
		strtmp.append(propvalue);
	strtmp.append("\"/>");
	return strtmp.c_str();
};

#endif
