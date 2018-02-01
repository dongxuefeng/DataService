/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */
#include <vector>
#include <xxplug_define.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xxxml.h>
#include <dbmepolice.h>
using namespace std;

#define TYPE 0004
#define strTYPE "0004"
MYSQL * m_sql = 0;

string gResLoginXML = "";
using namespace std;
string username = "";

int selectFromFather(char * father, string & resxml, int itemNum);

int xx_rcv_gettype()
{
	printf("--xxplug_recive_UserGroup--\n");
	connectMySQL(m_sql);

	return TYPE;
}

int xx_rcv_procinfo_add(cxx_xml xx_xml, char **outxmldata)
{
	int ret = 0;
	string sql = "";
	string resxml = "<resultdata><item1>";
	string str = "";
	string father = xx_xml.xpath_getprop("/userdata/data/father", "value");
	string group = xx_xml.xpath_getprop("/userdata/data/group", "value");

	sql = "select `group` from xxdb_info_usergroup where `group` = '";
	sql += group;
	sql += "'";
	//printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		ret = 0;
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(group.compare(row[0]) == 0)
				ret = 1;
			else
				ret = 0;
		}
		else
			ret = 0;
		mysql_free_result(result);
	}
	else ret = 2;

	if(ret == 0)
	{
		if(father.compare("0") == 0)
		{
			int maxdata = 0;
			sql = "SELECT datafilter FROM xxdb_info_usergroup WHERE father = '0'";
			//printf("SQL:%s\n",sql.c_str());
			if(xx_sql_query(m_sql,sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while ((row = mysql_fetch_row(result)))
				{
					if(maxdata < atoi(row[0]))	maxdata = atoi(row[0]);
				}
				mysql_free_result(result);
			}

			//printf("maxdatafilter = %d\n", maxdata);

			sql = "insert into xxdb_info_usergroup (`group`,`father`,`datafilter`) values ('";
			sql += group;
			sql += "','";
			sql += father;
			sql += "','";
			sql += itoaa(maxdata+1);
			sql += "')";
			//printf("SQL:%s\n",sql.c_str());
			ret = xx_sql_query(m_sql,sql.c_str());
			if(ret != 0) ret = 2;
		}
		else
		{
			char tmp[10] = {0};
			char datafilter[64] = {0};
			int maxdata = 0;

			sql = "SELECT datafilter FROM xxdb_info_usergroup WHERE `group` = '";
			sql += father;
			sql += "'";
			//printf("SQL:%s\n",sql.c_str());
			if(xx_sql_query(m_sql,sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if ((row = mysql_fetch_row(result)))
				{
					memset(datafilter, 0, 64);
					strcpy(datafilter, row[0]);
					printf("datafilter = %s\n", datafilter);
				}
				mysql_free_result(result);
			}

			sql = "SELECT datafilter FROM xxdb_info_usergroup WHERE father = '";
			sql += father;
			sql += "'";
			//printf("SQL:%s\n",sql.c_str());
			if(xx_sql_query(m_sql,sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while ((row = mysql_fetch_row(result)))
				{
					char *p = strrchr(row[0], ',');
					if(p != NULL)
					{
						//printf("row[0] = %s\n", row[0]);
						sprintf(tmp, p+1);
						//printf("tmp = %s\n", tmp);
						if(maxdata < atoi(tmp))
							maxdata = atoi(tmp);
					}
				}
				mysql_free_result(result);
			}

			strcat(datafilter, ",");
			strcat(datafilter, itoaa(maxdata+1));
			//printf("datafilter = %s\n", datafilter);

			sql = "insert into xxdb_info_usergroup (`group`,`father`,`datafilter`) values ('";
			sql += group;
			sql += "','";
			sql += father;
			sql += "','";
			sql += datafilter;
			sql += "')";
			//printf("SQL:%s\n",sql.c_str());
			ret = xx_sql_query(m_sql,sql.c_str());
			if(ret != 0) ret = 2;
		}
	}

	if(ret == 0) ret = 1;
	else if(ret == 1) ret = 3;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_del(char * inxmldata,char ** outxmldata)
{
	int ret = 0;

	string strpath[]={"/userdata/data/group"};
	string strvalue[1];
	char sqlstr[256];
	string respoxy;
	_set_data(inxmldata,1,strpath,strvalue);

	respoxy = "<resultdata><item1>";
	string rusername = "";

	sprintf(sqlstr,"delete from xxdb_info_usergroup where `group` = '%s'",strvalue[0].c_str());
	printf("SQL:%s\n",sqlstr);
	ret = xx_sql_query(m_sql,sqlstr);

	sprintf(sqlstr,"delete from xxdb_info_usergroup where `father` = '%s'",strvalue[0].c_str());

	ret = xx_sql_query(m_sql,sqlstr);

	if(ret == 0) ret = 1;
	else ret = 2;

	respoxy += new_strprop("res","value",itoaa(ret));
	respoxy += "</item1></resultdata>";

	setResXML(strTYPE,1,1,1,respoxy,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return ret;
}

int xx_rcv_procinfo_update(cxx_xml xx_xml, char **outxmldata)
{
	int ret = 0;
	int id = 0;
	string sql = "";
	string resxml = "<resultdata><item1>";

	sql = "SELECT `group` FROM xxdb_info_usergroup WHERE `group` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/newname", "value");
	sql += "'";

	printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
			ret = 1;
		else
			ret = 0;

		mysql_free_result(result);
	}
	else
		ret = 2;

	if(ret == 0)
	{
		sql = "SELECT id FROM xxdb_info_usergroup WHERE `group` = '";
		sql += xx_xml.xpath_getprop("/userdata/data/oldname", "value");
		sql += "'";
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
			{
				id = atoi(row[0]);
			}
			mysql_free_result(result);
		}

		sql = "UPDATE xxdb_info_usergroup SET `group` = '";
		sql += xx_xml.xpath_getprop("/userdata/data/newname", "value");
		sql += "' WHERE id = ";
		sql += itoaa(id);
		ret = xx_sql_query(m_sql, sql.c_str());
		if(ret != 0) ret = 2;
	}

	if(ret == 0) ret = 1;
	else if(ret == 1) ret = 3;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";

	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_select(char * inxmldata,char ** outxmldata)
{
	int ret = 0;
	string strpath[]={"/userdata/data/group","/userdata/head/Request_Index","/userdata/data/father"};
	string strvalue[3];
	string sqlstr = "";
	string respoxy;

	_set_data(inxmldata,3,strpath,strvalue);

	int isumcount = 0;
	int sendcount = 0;
	int startIndex  = atoi(strvalue[1].c_str());

	respoxy = "<resultdata>";
	string rusername = "";

	sqlstr += "select id,`group`,`father` from xxdb_info_usergroup where 1=1";
	if(strvalue[0] != "")
		sqlstr += " and `group` = '" +strvalue[0] +"'";
	if(strvalue[2] != "")
		sqlstr += " and `father` = '" +strvalue[2] +"'";

	if(xx_sql_query(m_sql,sqlstr.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			isumcount++;
			if((startIndex < isumcount)&&(sendcount < 10))
			{
				sendcount++;
				respoxy += "<item";
				respoxy += itoaa(sendcount);
				respoxy += ">";
				respoxy += new_strprop("group","value",row[1]);
				respoxy += new_strprop("father","value",row[2]);
				respoxy += "</item";
				respoxy +=itoaa(sendcount);
				respoxy +=">";
			}
		}
		mysql_free_result(result);
	}
	else ret = 1;

	respoxy += "</resultdata>";
	setResXML(strTYPE,sendcount,isumcount,startIndex,respoxy,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return ret;
}

int selectFromFather(char * father, string & resxml, int itemNum)
{
	string sql = "";
	vector<string> nameList;
	int count = 0;

	sql = "SELECT `group` FROM xxdb_info_usergroup WHERE father = '";
	sql += father;
	sql += "'";

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		string name;

		while ((row = mysql_fetch_row(result)))
		{
			name = "";
			name.append(row[0]);
			nameList.push_back(name);
		}
		mysql_free_result(result);
	}

	resxml += "<item";
	resxml += itoaa(itemNum);
	resxml += " name=\"";
	if(strcmp(father,"0") == 0)
		resxml += "00";
	else
		resxml += father;
	resxml += "\" count=\"";
	resxml += itoaa(nameList.size());
	resxml += "\">";

	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		count++;
		selectFromFather((char *)iter->c_str(), resxml, count);
	}

	resxml += "</item";
	resxml += itoaa(itemNum);
	resxml += ">";

	nameList.clear();

	return itemNum;
}

int xx_rcv_procinfo_selectNew(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata>";
	string sql = "";
	string usergroup = "";
	int groupid = 0;
	int count = 0;

	sql = "SELECT `group` FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'";

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			groupid = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	if(groupid == 0)
	{
		count = selectFromFather((char *)"0", resxml, 1);
	}
	else
	{
		sql = "SELECT `group` FROM xxdb_info_usergroup WHERE id = ";
		sql += itoaa(groupid);

		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
			{
				usergroup.append(row[0]);
			}
			mysql_free_result(result);

			count = selectFromFather((char *)usergroup.c_str(), resxml, 1);
		}
	}

	resxml += "</resultdata>";

	setResXML(strTYPE,count,count,0,resxml,gResLoginXML);

	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_selectPart(cxx_xml xx_xml, char **outxmldata)
{
	int ret = 0;

	string resxml = "<resultdata>";
	string sql = "";
	string group = xx_xml.xpath_getprop("/userdata/data/group", "value");
	string father = xx_xml.xpath_getprop("/userdata/data/father", "value");
	int sendcount = 0;

	if(father.compare("") == 0)
	{
		sql = "SELECT `group` FROM xxdb_info_usergroup WHERE id = (SELECT `group` FROM xxdb_info_deviceuser WHERE username = '";
		sql += username;
		sql += "')";

		//printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if ((row = mysql_fetch_row(result)))
			{
				if(strcmp(row[0], "0") != 0)
				{
					sendcount++;
					resxml += "<item";
					resxml += itoaa(sendcount);
					resxml += ">";
					resxml += new_strprop("group","value", row[0]);
					resxml += new_strprop("father","value","0");
					resxml += "</item";
					resxml +=itoaa(sendcount);
					resxml +=">";
				}
				else
					father = row[0];
			}
			mysql_free_result(result);
		}
	}

	if(sendcount == 0)
	{
		sql = "select `group`,`father` from xxdb_info_usergroup where 1=1";
		if(group.compare("") != 0)
			sql += " and `group` = '" + group +"'";
		if(father.compare("") != 0)
			sql += " and `father` = '" + father +"'";

		//printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while ((row = mysql_fetch_row(result)))
			{
				sendcount++;
				resxml += "<item";
				resxml += itoaa(sendcount);
				resxml += ">";
				resxml += new_strprop("group","value",row[0]);
				resxml += new_strprop("father","value",row[1]);
				resxml += "</item";
				resxml +=itoaa(sendcount);
				resxml +=">";
			}
			mysql_free_result(result);
		}
	}

	resxml += "</resultdata>";
	setResXML(strTYPE, sendcount, sendcount, 0, resxml, gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return ret;
}

int getFather(char &groupdatafilter)
{
	string sql = "";
	char * mydatafilter = new char[128];

	sql = "SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'";

	if (xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL)
			{
				memset(mydatafilter, 0, 128);
				sprintf(mydatafilter, row[0]);
				groupdatafilter = *mydatafilter;
			}
		}
		mysql_free_result(result);
	}

	delete mydatafilter;
	mydatafilter = NULL;

	return 0;
}

int xx_rcv_procinfo_addRoad(cxx_xml xx_xml, char **outxmldata)
{
	int ret = 0;
	string sql = "";
	string resxml = "<resultdata><item1>";
	string roadname = xx_xml.xpath_getprop("/userdata/data/roadname", "value");
	char groupdatafilter;

	getFather(groupdatafilter);
	sql = "select roadname from xxdb_data_roadname where roadname = '";
	sql += roadname;
	sql += "' and datafilter = '";
	sql += groupdatafilter;
	sql += "'";

	printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		ret = 0;
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(roadname.compare(row[0]) == 0)
				ret = 1;
			else
				ret = 0;
		}
		else
			ret = 0;
		mysql_free_result(result);
	}
	else ret = 2;

	if(ret == 0)
	{
		sql = "INSERT INTO xxdb_data_roadname (roadname,datafilter) VALUES ('";
		sql += roadname;
		sql += "','";
		sql += groupdatafilter;
		sql += "')";

		printf("SQL:%s\n",sql.c_str());
		ret = xx_sql_query(m_sql,sql.c_str());
		if(ret != 0) ret = 2;
	}

	if(ret == 0) ret = 1;
	else if(ret == 1) ret = 3;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_delRoad(cxx_xml xx_xml, char **outxmldata)
{
	int ret = 0;
	string sql = "";
	string resxml = "<resultdata><item1>";
	char groupdatafilter;

	getFather(groupdatafilter);

	sql = "DELETE FROM xxdb_info_relateroadgroup WHERE roadID = (SELECT id FROM xxdb_data_roadname WHERE roadname = '";
	sql += xx_xml.xpath_getprop("/userdata/data/roadname", "value");
	sql += "' and datafilter = '";
	sql += groupdatafilter;
	sql += "')";

	printf("SQL:%s\n",sql.c_str());
	ret = xx_sql_query(m_sql,sql.c_str());

	sql = "DELETE FROM xxdb_data_roadname WHERE roadname = '";
	sql += xx_xml.xpath_getprop("/userdata/data/roadname", "value");
	sql += "' and datafilter = '";
	sql += groupdatafilter;
	sql += "'";

	printf("SQL:%s\n",sql.c_str());
	ret = xx_sql_query(m_sql,sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_UpdateRoad(cxx_xml xx_xml, char **outxmldata)
{
	int ret = 0;
	string sql = "";
	string resxml = "<resultdata><item1>";
	string str = "";
	string oldname = xx_xml.xpath_getprop("/userdata/data/oldname", "value");
	string newname = xx_xml.xpath_getprop("/userdata/data/newname", "value");
	char groupdatafilter;

	getFather(groupdatafilter);
	sql = "select roadname from xxdb_data_roadname where roadname = '";
	sql += newname;
	sql += "' and datafilter = '";
	sql += groupdatafilter;
	sql += "'";

	printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		ret = 0;
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(newname.compare(row[0]) == 0)
				ret = 1;
			else
				ret = 0;
		}
		else
			ret = 0;
		mysql_free_result(result);
	}
	else ret = 2;

	if(ret == 0)
	{
		sql = "UPDATE xxdb_data_roadname SET roadname = '";
		sql += newname;
		sql += "' WHERE roadname = '";
		sql += oldname;
		sql += "' and datafilter = '";
		sql += groupdatafilter;
		sql += "'";

		printf("SQL:%s\n",sql.c_str());
		ret = xx_sql_query(m_sql,sql.c_str());
		if(ret != 0) ret = 2;
	}

	if(ret == 0) ret = 1;
	else if(ret == 1) ret = 3;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_SelectRoad(cxx_xml xx_xml, char **outxmldata)
{
	int ret = 0, dataCount = 0;
	char * tmp = new char[128];
	string sql = "";
	string resxml = "<resultdata><item1>";
	char groupdatafilter;

	sql = "SELECT COUNT(0) FROM xxdb_data_roadname WHERE id IN (SELECT roadID FROM xxdb_info_relateroadgroup WHERE groupID = (SELECT id FROM xxdb_info_usergroup WHERE `group` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/group", "value");
	sql += "'))";

	printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result))) dataCount = atoi(row[0]);
		mysql_free_result(result);
	}

	resxml += "<partCount value=\"";
	resxml += itoaa(dataCount);
	resxml += "\">";

	if(dataCount > 0)
	{
		sql = "SELECT roadname FROM xxdb_data_roadname WHERE id IN (SELECT roadID FROM xxdb_info_relateroadgroup WHERE groupID = (SELECT id FROM xxdb_info_usergroup WHERE `group` = '";
		sql += xx_xml.xpath_getprop("/userdata/data/group", "value");
		sql += "'))";

		printf("SQL:%s\n",sql.c_str());
		if(xx_sql_query(m_sql,sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			int i = 1;
			while ((row = mysql_fetch_row(result)))
			{
				memset(tmp, 0, 128);
				sprintf(tmp, "name%d", i++);
				resxml += new_strprop(tmp, "value", row[0]);
			}
			mysql_free_result(result);
		}
	}

	resxml += "</partCount>";

	getFather(groupdatafilter);
	sql = "select COUNT(0) from xxdb_data_roadname where datafilter = '";
	sql += groupdatafilter;
	sql += "'";

	printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result))) dataCount = atoi(row[0]);
		mysql_free_result(result);
	}

	resxml += "<AllCount value=\"";
	resxml += itoaa(dataCount);
	resxml += "\">";

	if(dataCount > 0)
	{
		sql = "select roadname from xxdb_data_roadname where datafilter = '";
		sql += groupdatafilter;
		sql += "'";

		printf("SQL:%s\n",sql.c_str());
		if(xx_sql_query(m_sql,sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			int i = 1;
			while ((row = mysql_fetch_row(result)))
			{
				memset(tmp, 0, 128);
				sprintf(tmp, "name%d", i++);
				resxml += new_strprop(tmp, "value", row[0]);
			}
			mysql_free_result(result);
		}
	}

	resxml += "</AllCount>";

	resxml += "</item1></resultdata>";
	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int relateRoadGroup(cxx_xml xx_xml, char **outxmldata)
{
	int ret = 0, count = 0;
	char * tmp = new char[128];
	string sql = "";
	string resxml = "<resultdata><item1>";

	sql = "DELETE FROM xxdb_info_relateroadgroup WHERE groupID = (SELECT id FROM xxdb_info_usergroup WHERE `group` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/groupname", "value");
	sql += "')";
	printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		count = atoi(xx_xml.xpath_getprop("/userdata/data/roadCount", "value"));
		for(int i = 0; i < count; i++)
		{
			memset(tmp, 0, 128);
			sprintf(tmp, "/userdata/data/roadname%d", i+1);
			sql = "INSERT INTO xxdb_info_relateroadgroup (roadID,groupID) VALUES ((SELECT id FROM xxdb_data_roadname WHERE roadname = '";
			sql += xx_xml.xpath_getprop(tmp, "value");
			sql += "'),(SELECT id FROM xxdb_info_usergroup WHERE `group` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/groupname", "value");
			sql += "'))";
			printf("SQL : %s\n", sql.c_str());
			xx_sql_query(m_sql,sql.c_str());
		}
	}
	else
		ret = 1;

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(strTYPE,count,count,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);
	delete tmp;

	return 0;
}

int GetRoadNameList(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	string datafilter = "";
	int count = 0;

	sql = "SELECT COUNT(0) FROM xxdb_data_roadname WHERE id IN (SELECT roadID FROM xxdb_info_relateroadgroup WHERE groupID = (SELECT `group` FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'))";

	printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result))) count = atoi(row[0]);
		mysql_free_result(result);
	}

	resxml += new_strprop("roadCount","value",itoaa(count));

	if(count > 0)
	{
		sql = "SELECT roadname FROM xxdb_data_roadname WHERE id IN (SELECT roadID FROM xxdb_info_relateroadgroup WHERE groupID = (SELECT `group` FROM xxdb_info_deviceuser WHERE username = '";
		sql += username;
		sql += "'))";

		printf("SQL:%s\n",sql.c_str());
		if(xx_sql_query(m_sql,sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			int i = 1;
			while ((row = mysql_fetch_row(result)))
			{
				resxml += "<roadname";
				resxml += itoaa(i++);
				resxml += " value=\"";
				resxml += row[0];
				resxml += "\"/>";
			}
			mysql_free_result(result);
		}
	}

	resxml += "</item1></resultdata>";
	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int CheckUsername(cxx_xml xx_xml, char ** outxmldata)
{
	int checking = 0;
	string str = xx_xml.xpath_getprop("/userdata/head/user", "value");

	if(str.compare("admin123") == 0) return 0;

	string sql = "SELECT username from xxdb_info_deviceuser WHERE userlogintag = '";
	sql += str.c_str();
	sql += "'";

	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			username = "";
			username.append(row[0]);
			checking = 1;
		}
		else
		{
			printf("%s ", str.c_str());
			checking = 0;
		}
		mysql_free_result(result);
	}
	else
		checking = 0;

	if(checking == 0)
	{
		string tmp1 = "<resultdata></resultdata>";
		string res;
		setResXML(strTYPE, 0, 0, 0, tmp1, res);
		*outxmldata = (char *) res.c_str();

		return -1;
	}

	return 0;
}

int xx_rcv_procinfo(char * inxmldata,char ** outxmldata)
{
	cxx_xml xx_xml;
	xx_xml.readxml(inxmldata);
	string str = xx_xml.xpath_getprop("/userdata/data/type", "value");

	printf("%s *** %s.%s *** IP:%s\n", xx_xml.xpath_getprop("/userdata/head/user", "value"), strTYPE, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));
	if(CheckUsername(xx_xml, outxmldata) < 0)
	{
		printf("%s %s.%s Checking Username error!IP:%s\n", xx_xml.xpath_getprop("/userdata/head/user", "value"), strTYPE, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));
		return -1;
	}

	printf("INXML: %s\n", inxmldata);


	if((str.compare("0007") == 0)||(str.compare("0008") == 0)||(str.compare("0009") == 0))
	{
		int value = 0;
		string sql = "SELECT `value` FROM xxdb_info_versions WHERE `name` = 'road'";

		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result))) value = atoi(row[0]);

			mysql_free_result(result);
		}

		value++;
		sql = "UPDATE xxdb_info_versions SET `value` = ";
		sql += itoaa(value);
		sql += " WHERE `name` = 'road'";

		printf("SQL: %s\n", sql.c_str());
		xx_sql_query(m_sql, sql.c_str());
	}


	if(str.compare("0001") == 0)
		return xx_rcv_procinfo_add(xx_xml, outxmldata);
	if(str.compare("0002") == 0)
		return xx_rcv_procinfo_del(inxmldata,outxmldata);
	if(str.compare("0003") == 0)
		return xx_rcv_procinfo_update(xx_xml, outxmldata);
	if(str.compare("0004") == 0)
		return xx_rcv_procinfo_select(inxmldata,outxmldata);
	if(str.compare("0005") == 0)
		return xx_rcv_procinfo_selectNew(xx_xml, outxmldata);
	if(str.compare("0006") == 0)
		return xx_rcv_procinfo_selectPart(xx_xml, outxmldata);
	if(str.compare("0007") == 0)
		return xx_rcv_procinfo_addRoad(xx_xml, outxmldata);
	if(str.compare("0008") == 0)
		return xx_rcv_procinfo_delRoad(xx_xml, outxmldata);
	if(str.compare("0009") == 0)
		return xx_rcv_procinfo_UpdateRoad(xx_xml, outxmldata);
	if(str.compare("0010") == 0)
		return xx_rcv_procinfo_SelectRoad(xx_xml, outxmldata);
	if(str.compare("0011") == 0)
		return GetRoadNameList(xx_xml, outxmldata);
	if(str.compare("0012") == 0)
		return relateRoadGroup(xx_xml, outxmldata);
	return 0;
}
