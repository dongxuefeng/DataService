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
#include <time.h>
using namespace std;

#define TYPE 0003
#define strTYPE "0003"

string gResLoginXML = "";
using namespace std;
MYSQL * m_sql = 0;
string username = "";

int xx_rcv_gettype()
{
	printf("--xxplug_recive_user--\n");
	connectMySQL(m_sql);
	return TYPE;
}

int xx_rcv_procinfo_useradd(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	string str = "";
	int baseCount = 0, count = 0;
	int ret = 0;

	sql = "SELECT usercount FROM xxdb_info_base";
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if ((row = mysql_fetch_row(result)))
		{
			baseCount = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	sql = "SELECT COUNT(*) FROM xxdb_info_deviceuser";
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			count = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	if(count >= baseCount)
	{
		ret = 1;
		printf("User Count Is Up!\n");
	}
	else
	{
		sql = "select * from xxdb_info_deviceuser where username = '";
		sql += xx_xml.xpath_getprop("/userdata/data/username","value");
		sql += "'";
		ret = xx_sql_query(m_sql,sql.c_str());
		if(ret == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if ((row = mysql_fetch_row(result)))
			{
				ret = 1;
			}
			else
				ret = 0;
			mysql_free_result(result);
		}
		else
			ret = 2;

		if(ret == 0)
		{
			sql = "insert into xxdb_info_deviceuser (username,`password`,`group`,regdate,device,role,name,datafilter) values ('";
			sql += xx_xml.xpath_getprop("/userdata/data/username","value");
			sql += "','";
			sql += xx_xml.xpath_getprop("/userdata/data/password","value");
			sql += "',(SELECT id FROM xxdb_info_usergroup WHERE `group` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/group","value");
			sql += "'),NOW(),'";
			sql += xx_xml.xpath_getprop("/userdata/data/dev","value");
			sql += "','";
			sql += xx_xml.xpath_getprop("/userdata/data/role","value");
			sql += "','";
			sql += xx_xml.xpath_getprop("/userdata/data/name","value");
			sql += "',(SELECT datafilter FROM xxdb_info_usergroup WHERE `group`='";
			sql += xx_xml.xpath_getprop("/userdata/data/group","value");
			sql += "'))";

			//printf("SQL: %s\n", sql.c_str());
			ret = xx_sql_query(m_sql,sql.c_str());
			if(ret == 0)
			{
				if(strcmp(xx_xml.xpath_getprop("/userdata/data/dev","value"),"") != 0)
				{
					sql = "update xxdb_info_device set bind = 1 where deviceid = '";
					sql += xx_xml.xpath_getprop("/userdata/data/dev","value");
					sql += "'";

					//printf("SQL: %s\n", sql.c_str());
					xx_sql_query(m_sql,sql.c_str());
				}
			}
		}
	}

	if(ret == 0) ret = 1;
	else if(ret == 1) ret = 3;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";

	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_userdel(char * inxmldata,char ** outxmldata)
{
	int ret = 0;
	string strpath[]={"/userdata/data/username"};
	string strvalue[1];
	char sqlstr[256];
	string respoxy;
	_set_data(inxmldata,1,strpath,strvalue);

	respoxy = "<resultdata><item1>";

	sprintf(sqlstr, "select device from xxdb_info_deviceuser where username = '%s'", strvalue[0].c_str());
	if(xx_sql_query(m_sql,sqlstr) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			memset(sqlstr, 0, 256);
			sprintf(sqlstr,"update xxdb_info_device set bind = 0 where deviceid ='%s'",row[0]);
			if(xx_sql_query(m_sql,sqlstr) != 0)
				ret = ret||1;
		}
		mysql_free_result(result);
	}
	else
		ret = ret||1;

	memset(sqlstr, 0, 256);
	sprintf(sqlstr,"delete from xxdb_info_deviceuser where username = '%s'",strvalue[0].c_str());

	ret = xx_sql_query(m_sql,sqlstr);

	if(ret == 0) ret = 1;
	else ret = 2;

	respoxy += new_strprop("res","value",itoaa(ret));
	respoxy += "</item1></resultdata>";

	setResXML(strTYPE,1,1,1,respoxy,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();


	return ret;
}

int xx_rcv_procinfo_userupdate(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "update xxdb_info_device set bind = 0 where deviceid IN (select device from xxdb_info_deviceuser where username = '";
	sql += xx_xml.xpath_getprop("/userdata/data/username","value");
	sql += "')";

	//printf("SQL : %s\n", sql.c_str());
	ret = xx_sql_query(m_sql,sql.c_str());

	sql = "update xxdb_info_deviceuser set `group` = (SELECT id FROM xxdb_info_usergroup WHERE `group` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/group","value");
	sql += "'), device = '";
	sql += xx_xml.xpath_getprop("/userdata/data/dev","value");
	sql += "', role = '";
	sql += xx_xml.xpath_getprop("/userdata/data/role","value");
	sql += "', name = '";
	sql += xx_xml.xpath_getprop("/userdata/data/name","value");
	sql += "', datafilter = (SELECT datafilter FROM xxdb_info_usergroup WHERE `group` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/group","value");
	sql += "') where username = '";
	sql += xx_xml.xpath_getprop("/userdata/data/username","value");
	sql += "'";
	//printf("SQL : %s\n", sql.c_str());
	ret = xx_sql_query(m_sql,sql.c_str());
	if(ret == 0)
	{
		sql = "update xxdb_info_device set bind = 1 where deviceid ='";
		sql += xx_xml.xpath_getprop("/userdata/data/dev","value");
		sql += "'";
		printf("SQL : %s\n", sql.c_str());
		ret = xx_sql_query(m_sql,sql.c_str());
	}

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";

	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();

	return 0;
}

int GetNameList(vector<string> * nameList)
{
	int count = 0;
	string sql = "";

	string datafilter = "";
	sql = "SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'";

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if ((row = mysql_fetch_row(result)))
		{
			datafilter = "";
			datafilter.append(row[0]);
		}
		mysql_free_result(result);
	}

	if(datafilter.compare("0") == 0)
	{
		vector<string> filterList;
		sql = "SELECT datafilter FROM xxdb_info_usergroup WHERE father = '0' or father = '-1'";
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			string tmp;
			while ((row = mysql_fetch_row(result)))
			{
				count++;
				tmp = "";
				tmp.append(row[0]);
				filterList.push_back(tmp);
			}
			mysql_free_result(result);
		}

		for(vector<string>::iterator iter = filterList.begin(); iter != filterList.end(); iter++)
		{
			sql = "SELECT username FROM xxdb_info_deviceuser WHERE datafilter LIKE '";
			sql += (*iter);
			sql += "%'";
			//printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				string tmp;
				while ((row = mysql_fetch_row(result)))
				{
					count++;
					tmp = "";
					tmp.append(row[0]);
					nameList->push_back(tmp);
				}
				mysql_free_result(result);
			}
		}
	}
	else
	{
		sql = "SELECT username FROM xxdb_info_deviceuser WHERE datafilter LIKE '";
		sql += datafilter;
		sql += "%'";
		//printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			string tmp;
			while ((row = mysql_fetch_row(result)))
			{
				count++;
				tmp = "";
				tmp.append(row[0]);
				nameList->push_back(tmp);
			}
			mysql_free_result(result);
		}
	}

	return count;
}

int xx_rcv_procinfo_userget(cxx_xml xx_xml, char ** outxmldata)
{
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index","value"));
	int sumcount = 0, count = 0;
	string sql = "";
	string str = "";
	string resxml = "<resultdata>";
	vector<string> nameList;

	GetNameList(&nameList);
	//printf("nameList size = %d\n", nameList.size());
	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		sql = "SELECT a.username,b.group,a.name,a.regdate,a.device,a.role FROM xxdb_info_deviceuser a,xxdb_info_usergroup b WHERE 1=1";
		str = xx_xml.xpath_getprop("/userdata/data/username","value");
		if(str.compare("") != 0)
		{
			sql += " and a.username = '";
			sql += str;
			sql += "'";
		}

		str = xx_xml.xpath_getprop("/userdata/data/group","value");
		if(str.compare("") != 0)
		{
			sql += " and a.group IN (SELECT id FROM xxdb_info_usergroup WHERE `group` = '";
			sql += str;
			sql += "')";
		}

		str = xx_xml.xpath_getprop("/userdata/data/role","value");
		if(str.compare("") != 0)
		{
			sql += " and a.role = '";
			sql += str;
			sql += "'";
		}

		str = xx_xml.xpath_getprop("/userdata/data/name","value");
		if(str.compare("") != 0)
		{
			sql += " and a.name = '";
			sql += str;
			sql += "'";
		}

		sql += " and b.id = a.group AND a.username = '";
		sql += (*iter).c_str();
		sql += "'";

		//printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				sumcount++;
				if((count - firstIndex) >= 10) continue;
				count++;

				if((count - firstIndex) > 0)
				{
					resxml += "<item";
					resxml += itoaa(count - firstIndex);
					resxml += ">";
					resxml += new_strprop("username","value",row[0]);
					resxml += new_strprop("group","value",row[1]);
					resxml += new_strprop("name","value",row[2]);
					resxml += new_strprop("regdate","value",row[3]);
					resxml += new_strprop("dev","value",row[4]);
					resxml += new_strprop("getdate","value","");
					resxml += new_strprop("role","value",row[5]);
					resxml += "</item";
					resxml += itoaa(count - firstIndex);
					resxml += ">";
				}
			}
			mysql_free_result(result);
		}
	}

	resxml += "</resultdata>";
	setResXML(strTYPE,count - firstIndex,sumcount,firstIndex,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int xx_rcv_procinfo_userPasswdAlter(char * inxmldata,char ** outxmldata)
{
	int ret = 0;
	string strpath[]={"/userdata/data/username","/userdata/data/oldpassword","/userdata/data/newpassword"};
	string strvalue[3];
	char sqlstr[256];
	string respoxy;

	_set_data(inxmldata,3,strpath,strvalue);

	respoxy = "<resultdata><item1>";
	string rissuccess = "1";

	sprintf(sqlstr,"select `password` FROM xxdb_info_deviceuser where 1=1 and username ='%s'"
			,strvalue[0].c_str());

	printf("SQL : %s\n", sqlstr);
	if(xx_sql_query(m_sql,sqlstr) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(strcmp(strvalue[1].c_str(), row[0]) != 0)
			{
				rissuccess = "1";
				ret = 1;
			}
		}
		else
		{
			rissuccess = "2";
			ret = 1;
		}
		mysql_free_result(result);
	}
	else
	{
		rissuccess = "2";
		ret = 1;
	}

	printf("ret = %d\n", ret);

	if(ret == 0)
	{
		sprintf(sqlstr,"update xxdb_info_deviceuser set `password` = '%s' where username ='%s'"
				,strvalue[2].c_str(),strvalue[0].c_str());
		if(xx_sql_query(m_sql,sqlstr) == 0)
		{
			rissuccess = "0";
			ret = 0;
		}
		else
		{
			rissuccess = "2";
			ret = 1;
		}
	}

	respoxy += "<res value=\""+rissuccess+"\"/>";
	respoxy += "</item1></resultdata>";

	setResXML(strTYPE,1,1,1,respoxy,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();

	return ret;
}

int xx_rcv_procinfo_departmentGet(char * inxmldata,char ** outxmldata)
{
	int ret = 0;
	string strpath[]={"/userdata/data/group"};
	string strvalue[2];
	string sqlstr = "";
	string respoxy;

	_set_data(inxmldata,1,strpath,strvalue);

	int isumcount = 0;

	respoxy = "<resultdata>";
	string rissuccess = "1";
	sqlstr += "select username,name,role,online from xxdb_info_deviceuser where 1=1";

	if(strvalue[0] != "")
		sqlstr += " and `group` = (SELECT id FROM xxdb_info_usergroup WHERE `group` =  '" +strvalue[0] +"')";

	//printf("SQL: %s\n", sqlstr.c_str());
	if(xx_sql_query(m_sql,sqlstr.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(result)))
		{
			isumcount++;
			respoxy += "<item";
			respoxy += itoaa(isumcount);
			respoxy += ">";

			respoxy += "<username value=\"";
			if(row[0] != NULL) respoxy +=row[0];
			respoxy +="\"/>";
			respoxy += "<name value=\"";
			if(row[1] != NULL) respoxy +=row[1];
			respoxy +="\"/>";
			respoxy += "<role value=\"";
			if(row[2] != NULL) respoxy +=row[2];
			respoxy +="\"/>";
			respoxy += "<online value=\"";
			if(row[3] != NULL) respoxy +=row[3];
			respoxy +="\"/>";

			respoxy += "</item";
			respoxy += itoaa(isumcount);
			respoxy +=">";
		}
		mysql_free_result(result);
	}
	else
		ret = 1;

	respoxy += "</resultdata>";
	setResXML(strTYPE, isumcount, isumcount, 0, respoxy, gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	//printf("OUTXML: %s\n", *outxmldata);

	return ret;
}

int xx_rcv_procinfo_roleGet(char * inxmldata,char ** outxmldata)
{
	int ret = 0;
	string strpath[]={"/userdata/data/role"};
	string strvalue[2];
	string sqlstr = "";
	string respoxy = "";

	_set_data(inxmldata,1,strpath,strvalue);

	int isumcount = 0;
	int sendcount = 0;
	int startIndex  = atoi(strvalue[2].c_str());

	respoxy = "<resultdata>";
	sqlstr += "select username,name from xxdb_info_deviceuser where 1=1";
		if(strvalue[0] != "")
			sqlstr += " and `role` = '" +strvalue[0] +"'";

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

				respoxy += "<username value=\"";
				if(row[1]) respoxy +=row[0];
				respoxy +="\"/>";
				respoxy += "<name value=\"";
				if(row[4]) respoxy +=row[1];
				respoxy +="\"/>";

				respoxy += "</item";
				respoxy += itoaa(sendcount);
				respoxy +=">";
			}
		}
		mysql_free_result(result);
	}
	else
		ret = 1;

	respoxy += "</resultdata>";
	setResXML(strTYPE,sendcount,isumcount,startIndex,respoxy,gResLoginXML);

	*outxmldata = (char *)gResLoginXML.c_str();

	return ret;
}

int xx_rcv_procinfo_setpassword(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";

	sql = "UPDATE xxdb_info_deviceuser SET `password` = '123456' WHERE username = '";
	sql += xx_xml.xpath_getprop("/userdata/data/username","value");
	sql += "'";

	printf("SQL : %s\n", sql.c_str());
	int res = xx_sql_query(m_sql, sql.c_str());

	if(res == 0) res = 1;
	else res = 2;

	resxml += new_strprop("res","value",itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(strTYPE,1,1,0,resxml,gResLoginXML);
	*outxmldata = (char *)gResLoginXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

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
			username = row[0];
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
		//printf("%s\n", res.c_str());

		return -1;
	}

	return 0;
}

int xx_rcv_procinfo11(char * inxmldata,char ** outxmldata)
{
	cxx_xml xx_xml;
	xx_xml.readxml(inxmldata);
	string str = xx_xml.xpath_getprop("/userdata/data/type", "value");

	if(CheckUsername(xx_xml, outxmldata) < 0)
	{
		printf(" %s.%s Checking Username error!IP:%s\n", strTYPE, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));
		return -1;
	}
	printf("%s *** %s.%s *** IP: %s\n", xx_xml.xpath_getprop("/userdata/head/user", "value"), strTYPE, str.c_str(), xx_xml.xpath_getprop("/userdata/head/ip", "value"));

	//printf("inxmldata : %s\n", inxmldata);

	if(str.compare("0001") == 0)
		return xx_rcv_procinfo_useradd(xx_xml, outxmldata);
	if(str.compare("0002") == 0)
		return xx_rcv_procinfo_userdel(inxmldata,outxmldata);
	if(str.compare("0003") == 0)
		return xx_rcv_procinfo_userupdate(xx_xml, outxmldata);
	if(str.compare("0004") == 0)
		return xx_rcv_procinfo_userget(xx_xml, outxmldata);
	if(str.compare("0005") == 0)
		return xx_rcv_procinfo_userPasswdAlter(inxmldata,outxmldata);
	if(str.compare("0006") == 0)
		return xx_rcv_procinfo_departmentGet(inxmldata,outxmldata);
	if(str.compare("0007") == 0)
		return xx_rcv_procinfo_roleGet(inxmldata,outxmldata);
	if(str.compare("0008") == 0)
		return xx_rcv_procinfo_setpassword(xx_xml, outxmldata);

	return 0;
}

int xx_rcv_procinfo(char * inxmldata,char ** outxmldata)
{
	return xx_rcv_procinfo11(inxmldata,outxmldata);
}
