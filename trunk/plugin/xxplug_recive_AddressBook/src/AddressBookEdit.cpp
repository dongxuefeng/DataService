#include "xxplug_recive_AddressBook.h"
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
extern string fileDir;

//web
int InsertLinkman(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "insert into xxdb_data_addressbook (`name`,phone1,phone2,phone3,`group`,_ID) values ('";
	sql += xx_xml.xpath_getprop("/userdata/data/name","value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/phone1","value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/phone2","value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/phone3","value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/group","value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/ID","value");
	sql += "')";

	printf("SQL: %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR,1,1,0,resxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int DeleteLinkman(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "delete from xxdb_data_addressbook where id = ";
	sql += xx_xml.xpath_getprop("/userdata/data/id","value");

	printf("SQL: %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR,1,1,0,resxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int UpdateLinkman(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "update xxdb_data_addressbook set `name` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/name","value");
	sql += "',phone1 = '";
	sql += xx_xml.xpath_getprop("/userdata/data/phone1","value");
	sql += "',phone2 = '";
	sql += xx_xml.xpath_getprop("/userdata/data/phone2","value");
	sql += "',phone3 = '";
	sql += xx_xml.xpath_getprop("/userdata/data/phone3","value");
	sql += "',`group` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/group","value");
	sql += "',_ID = '";
	sql += xx_xml.xpath_getprop("/userdata/data/ID","value");
	sql += "' where id = ";
	sql += xx_xml.xpath_getprop("/userdata/data/id","value");

	printf("SQL: %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR,1,1,0,resxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int SelectLinkman(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	string str = "";
	int count = 0, sumcount = 0;
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index","value"));

	sql = "SELECT count(0) FROM xxdb_data_addressbook WHERE 1=1 ";

	str = xx_xml.xpath_getprop("/userdata/data/name","value");
	if(str.compare("") != 0)
	{
		sql += " AND `name` = '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/phone1","value");
	if(str.compare("") != 0)
	{
		sql += " AND phone1 = '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/phone2","value");
	if(str.compare("") != 0)
	{
		sql += " AND phone2 = '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/phone3","value");
	if(str.compare("") != 0)
	{
		sql += " AND phone3 = '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/group","value");
	if(str.compare("") != 0)
	{
		sql += " AND `group` = '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/ID","value");
	if(str.compare("") != 0)
	{
		sql += " AND _ID = '";
		sql += str;
		sql += "'";
	}

	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result))) sumcount = atoi(row[0]);

		mysql_free_result(result);
	}

	if(sumcount > 0)
	{
		sql = "SELECT * FROM xxdb_data_addressbook WHERE 1=1";

		str = xx_xml.xpath_getprop("/userdata/data/name","value");
		if(str.compare("") != 0)
		{
			sql += " AND `name` = '";
			sql += str;
			sql += "'";
		}

		str = xx_xml.xpath_getprop("/userdata/data/phone1","value");
		if(str.compare("") != 0)
		{
			sql += " AND phone1 = '";
			sql += str;
			sql += "'";
		}

		str = xx_xml.xpath_getprop("/userdata/data/phone2","value");
		if(str.compare("") != 0)
		{
			sql += " AND phone2 = '";
			sql += str;
			sql += "'";
		}

		str = xx_xml.xpath_getprop("/userdata/data/phone3","value");
		if(str.compare("") != 0)
		{
			sql += " AND phone3 = '";
			sql += str;
			sql += "'";
		}

		str = xx_xml.xpath_getprop("/userdata/data/group","value");
		if(str.compare("") != 0)
		{
			sql += " AND `group` = '";
			sql += str;
			sql += "'";
		}

		str = xx_xml.xpath_getprop("/userdata/data/ID","value");
		if(str.compare("") != 0)
		{
			sql += " AND _ID = '";
			sql += str;
			sql += "'";
		}

		sql += " limit ";
		sql += itoaa(firstIndex);
		sql += ",10";

		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				count++;
				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";

				resxml += new_strprop("id","value",row[0]);
				resxml += new_strprop("name","value",row[1]);
				resxml += new_strprop("phone1","value",row[2]);
				resxml += new_strprop("phone2","value",row[3]);
				resxml += new_strprop("phone3","value",row[4]);
				resxml += new_strprop("group","value",row[5]);
				resxml += new_strprop("ID","value",row[6]);

				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";

			}
			mysql_free_result(result);
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR,count,sumcount,0,resxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int SelectGroup(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	int count = 0;

	sql = "SELECT `group` FROM xxdb_data_addressbook GROUP BY `group`";

	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		string str = "";

		while((row = mysql_fetch_row(result)))
		{
			str = row[0];
			if(str.compare("") != 0)
			{
				count++;

				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";

				resxml += new_strprop("group","value",row[0]);

				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}
		}
		mysql_free_result(result);
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR,count,count,0,resxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

//phone
int GetAddressBookFileURL(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<?xml version=\"1.0\" encoding=\"utf-8\"?><body>";
	int count = 0;

	sql = "select `name`,phone1,phone2,phone3,`group`,_ID from xxdb_data_addressbook order by `name` asc";

	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			count++;
			resxml += "<item";
			resxml += itoaa(count);
			resxml += ">";

			resxml += new_strprop("name","value",row[0]);
			resxml += new_strprop("phone1","value",row[1]);
			resxml += new_strprop("phone2","value",row[2]);
			resxml += new_strprop("phone3","value",row[3]);
			resxml += new_strprop("group","value",row[4]);
			resxml += new_strprop("ID","value",row[5]);

			resxml += "</item";
			resxml += itoaa(count);
			resxml += ">";

		}
		mysql_free_result(result);
	}

	resxml += new_strprop("count","value",itoaa(count));
	resxml += "</body>";

	printf("addressBook.xml : %s\n", resxml.c_str());

	string fileAllpath = fileDir;
	fileAllpath += "/addressBook.xml";
	FILE * f = fopen(fileAllpath.c_str(), "wb");
	fwrite(resxml.c_str(), 1, resxml.size(), f);
	fclose(f);

	resxml = "<resultdata><item1>";
	resxml += "<url value=\"xxpic/addressBook.xml\"/>";
	resxml += "</item1></resultdata>";

	setResXML(TYPESTR,1,1,0,resxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}
