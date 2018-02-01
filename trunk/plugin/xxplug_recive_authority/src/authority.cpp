#include "xxplug_recive_authority.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <xxxml.h>
#include <windows.h>
#include "chinese_code.h"

extern MYSQL * m_sql;
extern string gresXML;
extern string username;

int InsertData(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";
	int ret = 0;
	int count = 0;
	int type = atoi(xx_xml.xpath_getprop("/userdata/data/flag", "value"));

	switch(type)
	{
	case 0:
		{
			sql = "SELECT COUNT(0) from xxdb_info_func WHERE `function` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "' AND funType = ";
			sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while ((row = mysql_fetch_row(result)))
				{
					count = atoi(row[0]);
				}
				mysql_free_result(result);
			}
			else ret = 1;

			if(count == 0)
			{
				sql = "INSERT xxdb_info_func (`function`, father, funType) VALUES ('";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "',0,";
				sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
				sql += ")";
			}
		}
		break;
	case 1:
		{
			sql = "SELECT COUNT(0) from xxdb_info_func WHERE `function` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "' AND funType = ";
			sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if ((row = mysql_fetch_row(result)))
				{
					count = atoi(row[0]);
				}
				mysql_free_result(result);
			}
			else ret = 1;

			if(count == 0)
			{
				sql = "INSERT xxdb_info_func (`function`, father, funType) VALUES ('";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "',-1,";
				sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
				sql += ")";
			}
			else
			{
				sql = "SELECT id,`function` from xxdb_info_func WHERE `function` = '";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "'";
				if(xx_sql_query(m_sql, sql.c_str()) == 0)
				{
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;

					if ((row = mysql_fetch_row(result)))
					{
						printf("id = %s\n", row[0]);
						printf("function name = %s\n", row[1]);
					}
					mysql_free_result(result);
				}
			}
		}
		break;
	case 2:
		{
			sql = "SELECT COUNT(0) from xxdb_info_func WHERE `function` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "'";
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while ((row = mysql_fetch_row(result)))
				{
					count = atoi(row[0]);
				}
				mysql_free_result(result);
			}
			else ret = 1;

			if(count == 0)
			{
				sql = "INSERT xxdb_info_role (rolename, roletype) VALUES ('";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "',";
				sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
				sql += ")";
			}
		}
		break;
	default:
		break;
	}

	if(count == 0)
	{
		//printf("SQL: %s\n", sql.c_str());
		ret = xx_sql_query(m_sql, sql.c_str());
	}

	if(ret != 1)
		if(count > 0) ret = 2;

	if(ret == 0) ret = 1;
	else if(ret == 1) ret = 2;
	else ret = 3;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int DeleteData(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";
	int ret = 0;
	int type = atoi(xx_xml.xpath_getprop("/userdata/data/flag", "value"));

	switch(type)
	{
	case 0:
	case 1:
		{
			string id = "";
			sql = "SELECT id FROM xxdb_info_func WHERE `function` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "'";
			printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if ((row = mysql_fetch_row(result)))
				{
					id = row[0];
				}
				mysql_free_result(result);

				sql = "UPDATE xxdb_info_func SET father = -1 WHERE father = ";
				sql += id;
				printf("SQL: %s\n", sql.c_str());
				ret = xx_sql_query(m_sql, sql.c_str());

				sql = "DELETE FROM xxdb_info_func WHERE `function` = '";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "'";
				printf("SQL: %s\n", sql.c_str());
				ret = xx_sql_query(m_sql, sql.c_str());
			}
			else
				ret = 1;
		}
		break;
	case 2:
		{
			sql = "DELETE FROM xxdb_info_relevancerolefunc WHERE roleID = (SELECT id FROM xxdb_info_role WHERE rolename = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "')";
			ret = xx_sql_query(m_sql, sql.c_str());

			sql = "DELETE FROM xxdb_info_role WHERE rolename = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "'";
			printf("SQL: %s\n", sql.c_str());
			ret = xx_sql_query(m_sql, sql.c_str());
		}
		break;
	default:
		break;
	}

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int RelevanceData(cxx_xml xx_xml, char **outxmldata)
{
	int ret = 0;
	string sql = "";
	int count = atoi(xx_xml.xpath_getprop("/userdata/data/Count", "value"));
	int type = atoi(xx_xml.xpath_getprop("/userdata/data/flag", "value"));
	char * SName = (char *)malloc(512);
	char * tmp = (char *)malloc(512);

	switch(type)
	{
	case 0:
		{
			int father = 0;
			sql = "SELECT id FROM xxdb_info_func WHERE `function` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/Name", "value");
			sql += "'";
			printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while ((row = mysql_fetch_row(result)))
				{
					father = atoi(row[0]);
				}
				mysql_free_result(result);
			}

			sql = "UPDATE xxdb_info_func SET father = -1 WHERE father = ";
			sql += itoaa(father);
			ret = xx_sql_query(m_sql, sql.c_str());

			for(int i = 1; i <= count; i++)
			{
				memset(tmp, 0, 512);
				sprintf(tmp, "/userdata/data/SName%d", i);
				memset(SName, 0, 512);
				sprintf(SName, xx_xml.xpath_getprop(tmp, "value"));

				sql = "UPDATE xxdb_info_func SET father = ";
				sql += itoaa(father);
				sql += " WHERE `function` = '";
				sql += SName;
				sql += "'";

				printf("SQL: %s\n", sql.c_str());
				ret = xx_sql_query(m_sql, sql.c_str());
			}
		}
		break;
	case 1:
		{
			sql = "DELETE FROM xxdb_info_relevanceuserrole WHERE userID = (SELECT id FROM xxdb_info_deviceuser WHERE username = '";
			sql += xx_xml.xpath_getprop("/userdata/data/Name", "value");
			sql += "')";
			ret = xx_sql_query(m_sql, sql.c_str());

			for(int i = 1; i <= count; i++)
			{
				memset(tmp, 0, 512);
				sprintf(tmp, "/userdata/data/SName%d", i);
				memset(SName, 0, 512);
				sprintf(SName, xx_xml.xpath_getprop(tmp, "value"));

				sql = "INSERT INTO xxdb_info_relevanceuserrole (userID,roleID) VALUES ((SELECT id FROM xxdb_info_deviceuser WHERE username = '";
				sql += xx_xml.xpath_getprop("/userdata/data/Name", "value");
				sql += "'),(SELECT id FROM xxdb_info_role WHERE rolename = '";
				sql += SName;
				sql += "'))";
				printf("SQL: %s\n", sql.c_str());
				ret = xx_sql_query(m_sql, sql.c_str());
			}
		}
		break;
	case 2:
		{
			sql = "DELETE FROM xxdb_info_relevancerolefunc WHERE roleID = (SELECT id FROM xxdb_info_role WHERE rolename = '";
			sql += xx_xml.xpath_getprop("/userdata/data/Name", "value");
			sql += "')";
			ret = xx_sql_query(m_sql, sql.c_str());

			for(int i = 1; i <= count; i++)
			{
				memset(tmp, 0, 512);
				sprintf(tmp, "/userdata/data/SName%d", i);
				memset(SName, 0, 512);
				sprintf(SName, xx_xml.xpath_getprop(tmp, "value"));

				sql = "INSERT INTO xxdb_info_relevancerolefunc (roleID,funcID) VALUES ((SELECT id FROM xxdb_info_role WHERE rolename = '";
				sql += xx_xml.xpath_getprop("/userdata/data/Name", "value");
				sql += "'),(SELECT id FROM xxdb_info_func WHERE `function` = '";
				sql += SName;
				sql += "' AND funType = (SELECT roletype FROM xxdb_info_role WHERE rolename = '";
				sql += xx_xml.xpath_getprop("/userdata/data/Name", "value");
				sql += "')))";
				printf("SQL: %s\n", sql.c_str());
				ret = xx_sql_query(m_sql, sql.c_str());
			}
		}
		break;
	default:
		break;
	}

	if(ret == 0) ret = 1;
	else ret = 2;

	string resxml = "<resultdata><item1>";
	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	delete SName;
	delete tmp;

	return 0;
}

int GetNameList(vector<string> * nameList)
{
	int count = 0;
	string sql = "";

	string datafilter = "";
	if(username.compare("admin123") != 0)
		{//如果不是超级用户，查询用户的datafilter
		sql = "SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
		sql += username;
		sql += "'";

		printf("SQL: %s\n", sql.c_str());
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
	}
	else
		datafilter = "0";//是超级用户，则数据过滤

	if(datafilter.compare("0") == 0)
	{
		vector<string> filterList;

		filterList.push_back(datafilter);
        //查询用户组的兄弟节点
		sql = "SELECT datafilter FROM xxdb_info_usergroup WHERE father = '0' or father = '-1'";
		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			string tmp;
			while ((row = mysql_fetch_row(result)))
			{
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
			sql += "%' AND username != '";
			sql += username;
			sql += "'";
			printf("SQL : %s\n", sql.c_str());
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
		sql += "%' AND username != '";
		sql += username;
		sql += "'";
		printf("SQL : %s\n", sql.c_str());
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
//获取所有的数据
int GetAllData(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";
	vector<string> nameList;
	int count = 0;
	int type = atoi(xx_xml.xpath_getprop("/userdata/data/flag", "value"));

	printf("flag = %d\n", type);
	switch(type)
	{
	case 0://查询功能模块总数
		{
			sql = "SELECT COUNT(0) FROM xxdb_info_func WHERE father = 0 AND funType = ";
			sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
		}
		break;
	case 1://查询功能选项总数
		{
			sql = "SELECT COUNT(0) FROM xxdb_info_func WHERE father != 0 AND funType = ";
			sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
		}
		break;
	case 2://查询角色总数
		{
			sql = "SELECT COUNT(0) FROM xxdb_info_role";
		}
		break;
	case 3:
		{
			GetNameList(&nameList);
			count = nameList.size();
			goto USER;
		}
		break;
	default:
		break;
	}

	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			count = atoi(row[0]);
		}
		mysql_free_result(result);
	}

USER:

	resxml += new_strprop("count", "value", itoaa(count));

	if(count > 0)
	{
		switch(type)
		{
		case 0://查询功能模块
			{
				sql = "SELECT `function` FROM xxdb_info_func WHERE father = 0 AND funType = ";
				sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
			}
			break;
		case 1://查询功能选项
			{
				sql = "SELECT `function` FROM xxdb_info_func WHERE father != 0 AND funType = ";
				sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
			}
			break;
		case 2://查询角色
			{
				sql = "SELECT rolename FROM xxdb_info_role";
			}
			break;
		case 3:
			{
				char tmp[10] = {0};
				int i = 0;
				for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
				{
					i++;
					memset(tmp, 0, 10);
					sprintf(tmp, "name%d", i);
					sql = "SELECT `name` FROM xxdb_info_deviceuser WHERE username = '";
					sql += (*iter);
					sql += "'";
					if(xx_sql_query(m_sql, sql.c_str()) == 0)
					{
						MYSQL_RES *result = mysql_store_result(m_sql);
						MYSQL_ROW row;
						if ((row = mysql_fetch_row(result)))
						{
							resxml += "<name";
							resxml += itoaa(i);
							resxml += " value=\"";
							resxml += (*iter);
							resxml += "\" realname=\"";
							resxml += row[0];
							resxml += "\"/>";
						}
						mysql_free_result(result);
					}
				}
				goto USEREND;
			}
			break;
		default:
			break;
		}

		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			int i = 0;
			char tmp[10] = {0};
			while ((row = mysql_fetch_row(result)))
			{
				i++;
				memset(tmp, 0, 10);
				sprintf(tmp, "name%d", i);
				resxml += new_strprop(tmp, "value", row[0]);
			}
			mysql_free_result(result);
		}
	}

USEREND:
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

//    string strin,strout;
//    strin =gresXML;
//    CChineseCode::UTF_8ToGB2312(strout,(char *)strin.c_str(),strin.length());
//    printf("OUT XML func ==:%s\n",strout.c_str());
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int GetPartData(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";
	int count = 0;
	int i = 0;
	int type = atoi(xx_xml.xpath_getprop("/userdata/data/flag", "value"));

	switch(type)
	{
	case 0:
		{
			sql = "SELECT COUNT(0) FROM xxdb_info_role WHERE id IN (SELECT roleID FROM xxdb_info_relevanceuserrole WHERE userID IN (SELECT id FROM xxdb_info_deviceuser WHERE username = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "'))";
		}
		break;
	case 1:
		{
			sql = "SELECT COUNT(0) FROM xxdb_info_func WHERE id IN (SELECT funcID FROM xxdb_info_relevancerolefunc WHERE roleID IN (SELECT id FROM xxdb_info_role WHERE rolename = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "'))";
		}
		break;
	case 2:
		{
			sql = "SELECT COUNT(0) FROM xxdb_info_func WHERE father IN (SELECT id FROM xxdb_info_func WHERE `function` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "')";
		}
		break;
	default:
		break;
	}

	//printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			count = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	resxml += "<partCount value=\"";
	resxml += itoaa(count);
	resxml += "\">";

	if(count > 0)
	{
		switch(type)
		{
		case 0:
			{
				sql = "SELECT rolename FROM xxdb_info_role WHERE id IN (SELECT roleID FROM xxdb_info_relevanceuserrole WHERE userID IN (SELECT id FROM xxdb_info_deviceuser WHERE username = '";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "'))";
			}
			break;
		case 1:
			{
				sql = "SELECT `function` FROM xxdb_info_func WHERE id IN (SELECT funcID FROM xxdb_info_relevancerolefunc WHERE roleID IN (SELECT id FROM xxdb_info_role WHERE rolename = '";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "'))";
			}
			break;
		case 2:
			{
				sql = "SELECT `function` FROM xxdb_info_func WHERE father IN (SELECT id FROM xxdb_info_func WHERE `function` = '";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "')";
			}
			break;
		default:
			break;
		}

		//printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			i = 0;
			char tmp[10] = {0};
			while ((row = mysql_fetch_row(result)))
			{
				i++;
				memset(tmp, 0, 10);
				sprintf(tmp, "name%d", i);
				resxml += new_strprop(tmp, "value", row[0]);
			}
			mysql_free_result(result);
		}
	}
	resxml += "</partCount>";

	switch(type)
	{
	case 0:
		{
			sql = "SELECT COUNT(0) FROM xxdb_info_role";
		}
		break;
	case 1:
		{
			sql = "SELECT COUNT(0) FROM xxdb_info_func WHERE father != 0 AND funType IN (SELECT roletype FROM xxdb_info_role WHERE rolename = '";
			sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
			sql += "')";
		}
		break;
	case 2:
		{
			sql = "SELECT COUNT(0) FROM xxdb_info_func WHERE father != 0 AND father = -1 AND funType = ";
			sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
		}
		break;
	default:
		break;
	}

	//printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			count = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	resxml += "<AllCount value=\"";
	resxml += itoaa(count);
	resxml += "\">";

	if(count > 0)
	{
		switch(type)
		{
		case 0:
			{
				sql = "SELECT rolename FROM xxdb_info_role";
			}
			break;
		case 1:
			{
				sql = "SELECT `function` FROM xxdb_info_func WHERE father != 0 AND funType IN (SELECT roletype FROM xxdb_info_role WHERE rolename = '";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "')";
			}
			break;
		case 2:
			{
				sql = "SELECT `function` FROM xxdb_info_func WHERE father != 0 AND father = -1 AND father = -1 AND funType = ";
				sql += xx_xml.xpath_getprop("/userdata/data/DevType", "value");
			}
			break;
		default:
			break;
		}

		//printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			i = 0;
			char tmp[10] = {0};
			while ((row = mysql_fetch_row(result)))
			{
				i++;
				memset(tmp, 0, 10);
				sprintf(tmp, "name%d", i);
				resxml += new_strprop(tmp, "value", row[0]);
			}
			mysql_free_result(result);
		}
	}
	resxml += "</AllCount>";

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int GetModuleData(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";
	int count = 0;
	string gbkstr = "";
	string utf8str = xx_xml.xpath_getprop("/userdata/data/name", "value");
	CChineseCode::UTF_8ToGB2312(gbkstr, (char *)utf8str.c_str(), utf8str.size());
	printf("gbk = %s\n", gbkstr.c_str());

	sql = "SELECT COUNT(0) FROM xxdb_info_func WHERE id IN (SELECT funcID FROM xxdb_info_relevancerolefunc WHERE roleID IN (SELECT roleID FROM xxdb_info_relevanceuserrole WHERE userID IN (SELECT id FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'))) AND father IN (SELECT id FROM xxdb_info_func WHERE `function` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
	sql += "')";
	printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			count = atoi(row[0]);
		}
		mysql_free_result(result);
	}
	resxml += new_strprop("count", "value", itoaa(count));

	if(count > 0)
	{
		char tmp[20] = {0};
		sql = "SELECT `function` FROM xxdb_info_func WHERE id IN (SELECT funcID FROM xxdb_info_relevancerolefunc WHERE roleID IN (SELECT roleID FROM xxdb_info_relevanceuserrole WHERE userID IN (SELECT id FROM xxdb_info_deviceuser WHERE username = '";
		sql += username;
		sql += "'))) AND father IN (SELECT id FROM xxdb_info_func WHERE `function` = '";
		sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
		sql += "')";
		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			int i = 1;
			while ((row = mysql_fetch_row(result)))
			{
				memset(tmp, 0, 20);
				sprintf(tmp, "name%d", i);
				resxml += new_strprop(tmp, "value", row[0]);
				i++;
			}
			mysql_free_result(result);
		}
	}

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}
