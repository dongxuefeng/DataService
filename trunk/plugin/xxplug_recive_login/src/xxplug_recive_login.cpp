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
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "xxxml.h"
#include <dbmepolice.h>
#include <day.h>
using namespace std;

#define TYPE 7
#define strTYPE "0007"

string gResLoginXML = "";
using namespace std;
MYSQL * m_sql = 0;

int xx_rcv_gettype()
{
	printf("--xxplug_recive_login--\n");
	connectMySQL(m_sql);

	return TYPE;
}

void set5char(char * out, int len)
{
	*out	 = (char)(len%100000/10000+0x30);
	*(out+1) = (char)(len%10000/1000+0x30);
	*(out+1) = (char)(len%1000/100+0x30);
	*(out+2) = (char)(len%100/10+0x30);
	*(out+3) = (char)(len%10+0x30);
}


int GetNameList(vector<string> * nameList, char * username)
{
	string sql = "";

	string datafilter = "";
	sql = "SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'";
	//printf("SQL: %s\n", sql.c_str());
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

	sql = "SELECT username FROM xxdb_info_deviceuser WHERE datafilter LIKE '";
	sql += datafilter;
	sql += "%'";
	//printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		string userstr = "";
		while ((row = mysql_fetch_row(result)))
		{
			userstr = "";
			userstr.append(row[0]);

			nameList->push_back(userstr);
		}
		mysql_free_result(result);
	}

	return nameList->size();
}

int xx_rcv_procinfo_login(char * inxmldata,char ** outxmldata)
{
	cxx_xml xx_xml;
	xx_xml.readxml(inxmldata);

	int ret = 0;
	string sql = "";
	string type = xx_xml.xpath_getprop("/userdata/data/type", "value");
	string username = xx_xml.xpath_getprop("/userdata/data/inputusername","value");
	string password = xx_xml.xpath_getprop("/userdata/data/passwords","value");
	string resxml = "<resultdata><item1>";
	string userlog = "";
	string realname = "";
	int dev = 0;

	printf("*** %s.%s *** user:%s password:%s IP:%s\n", strTYPE, type.c_str(), username.c_str(), password.c_str(), xx_xml.xpath_getprop("/userdata/head/ip","value"));

	//printf("INXML:%s\n",inxmldata);

	if(type.compare("0001") == 0)
	{
		sql = "SELECT id,name FROM xxdb_info_deviceuser WHERE username = '";
		sql += username;
		sql += "' AND PASSWORD = '";
		sql += password;
		sql += "'";

		if(xx_sql_query(m_sql,sql.c_str()) == 0)
		{
			//正常
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
			{
				char tmp[10] = {0};
				srand((unsigned)time(NULL));
				set5char(tmp, rand());
				username += tmp;
				realname = row[1];
				ret = 1;
			}
			else
				ret = 2;
			mysql_free_result(result);
		}
		else
			ret = 3;

		string devtype = xx_xml.xpath_getprop("/userdata/head/dev","value");

		if(devtype.compare("phone") == 0) //手机权限
			dev = 1;
		else if(devtype.compare("pcclient") == 0) //pc客户端 权限
			dev = 2;
		else     //web 权限
			dev = 0;

		if(ret == 1)
		{
			resxml += new_strprop("username", "value", username.c_str());
			resxml += new_strprop("realname", "value", realname.c_str());
			int count = 0;
			char sname[20] = {0};

			vector<string> roleIDList;
			sql = "SELECT roleID FROM xxdb_info_relevanceuserrole WHERE userID IN (SELECT id FROM xxdb_info_deviceuser WHERE username = '";
			sql += xx_xml.xpath_getprop("/userdata/data/inputusername","value");
			sql += "')";
			//printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql,sql.c_str()) == 0)
			{
				//正常
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while((row = mysql_fetch_row(result)))
				{
					string id = row[0];
					roleIDList.push_back(id);
				}
				mysql_free_result(result);
			}

			vector<string> idList;
			for(vector<string>::iterator iter = roleIDList.begin(); iter != roleIDList.end(); iter++)
			{
				sql = "SELECT id FROM xxdb_info_role WHERE id = ";
				sql += (*iter);
				sql += " AND roletype = ";
				sql += itoaa(dev);
				//printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql,sql.c_str()) == 0)
				{
					//正常
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;

					if((row = mysql_fetch_row(result)))
					{
						string id = row[0];
						idList.push_back(id);
					}
					mysql_free_result(result);
				}
			}

			printf("role count = %d\n", roleIDList.size());
            //查询用户绑定的功能选项
			if(roleIDList.size() > 0)
			{
				if(dev != 0)
				{
					for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
					{
						sql = "SELECT COUNT(0) FROM xxdb_info_func WHERE id IN (SELECT funcID FROM xxdb_info_relevancerolefunc WHERE roleID = ";
						sql += (*iter);
						sql += ")";
						//printf("SQL : %s\n", sql.c_str());
						if(xx_sql_query(m_sql,sql.c_str()) == 0)
						{
							//正常
							MYSQL_RES *result = mysql_store_result(m_sql);
							MYSQL_ROW row;

							if((row = mysql_fetch_row(result)))
							{
								count += atoi(row[0]);
								//printf("count = %d\n", count);
							}
							mysql_free_result(result);
						}
					}
					printf("======count = %d\n", count);

					resxml += new_strprop("count", "value", itoaa(count));

					int i = 1;
					if(count > 0)
					{
						resxml += new_strprop("issuccess", "value", itoaa(ret));

						for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
						{
							sql = "SELECT `function` FROM xxdb_info_func WHERE id IN (SELECT funcID FROM xxdb_info_relevancerolefunc WHERE roleID = ";
							sql += (*iter);
							sql += ")";
							printf("SQL : %s\n", sql.c_str());
							if(xx_sql_query(m_sql,sql.c_str()) == 0)
							{
								//正常
								MYSQL_RES *result = mysql_store_result(m_sql);
								MYSQL_ROW row;

								while((row = mysql_fetch_row(result)))
								{
									memset(sname, 0, 20);
									sprintf(sname, "name%d", i);
									resxml += new_strprop(sname, "value", row[0]);
									i++;
								}
								mysql_free_result(result);
							}
						}
					}
					else
					{
						ret = 4;
						resxml += new_strprop("issuccess", "value", itoaa(ret));
					}
				}
				else
				{
					int j = 0;
					resxml += new_strprop("issuccess", "value", itoaa(ret));
					sql = "SELECT COUNT(0) FROM xxdb_info_func WHERE id IN (SELECT father FROM xxdb_info_func WHERE id IN (SELECT funcID FROM xxdb_info_relevancerolefunc WHERE roleID = ";
					for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
					{
						if(j == 0)
						{
							sql += (*iter);
							j++;
						}
						else
						{
							sql += " OR roleID = ";
							sql += (*iter);
						}
					}
					sql += "))";

					if(xx_sql_query(m_sql,sql.c_str()) == 0)
					{
						//正常
						MYSQL_RES *result = mysql_store_result(m_sql);
						MYSQL_ROW row;

						while((row = mysql_fetch_row(result)))
						{
							count += atoi(row[0]);
						}
						mysql_free_result(result);
					}
					printf("======count = %d\n", count);

					resxml += new_strprop("count", "value", itoaa(count));

					int i = 1;
					if(count > 0)
					{
						int first = 0;
						resxml += new_strprop("issuccess", "value", itoaa(ret));
						sql = "SELECT `function` FROM xxdb_info_func WHERE id IN (SELECT father FROM xxdb_info_func WHERE id IN (SELECT funcID FROM xxdb_info_relevancerolefunc WHERE roleID = ";
						for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
						{
							if(first == 0)
							{
								sql += (*iter);
								first++;
							}
							else
							{
								sql += " OR roleID = ";
								sql += (*iter);
							}
						}
						sql += "))";
						printf("SQL : %s\n", sql.c_str());
						if(xx_sql_query(m_sql,sql.c_str()) == 0)
						{
							//正常
							MYSQL_RES *result = mysql_store_result(m_sql);
							MYSQL_ROW row;

							while((row = mysql_fetch_row(result)))
							{
								memset(sname, 0, 20);
								sprintf(sname, "name%d", i);
								resxml += new_strprop(sname, "value", row[0]);
								i++;
							}
							mysql_free_result(result);
						}

						//查询卡口数据过期时间
						int outdate = 0;
						sql = "SELECT outdate FROM xxdb_info_base";
						if(xx_sql_query(m_sql,sql.c_str()) == 0)
						{
							//正常
							MYSQL_RES *result = mysql_store_result(m_sql);
							MYSQL_ROW row;

							if((row = mysql_fetch_row(result)))
							{
								resxml += new_strprop("outdate", "value", row[0]);
								outdate = atoi(row[0]);
							}
							mysql_free_result(result);
						}

						vector<string> nameList;
						GetNameList(&nameList, (char *)xx_xml.xpath_getprop("/userdata/data/inputusername","value"));

						SYSTEMTIME t;
						string now_t = "";
						char befor7_t[15] = {0};

						GetLocalTime(&t);
						now_t = itoaa(t.wYear);
						now_t += "-";
						now_t += itoaa(t.wMonth);
						now_t += "-";
						now_t += itoaa(t.wDay);

						GetDate((char *)now_t.c_str(), befor7_t, outdate*(-1));
						sql = "SELECT COUNT(0) FROM xxdb_data_viodata WHERE DATETIME <= '";
						sql += befor7_t;
						sql += "' AND disposetype = 0 AND download = 0 AND affirm = 1 AND datafrom = 1 ";
						sql += "AND (";
						first = 0;
						for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
						{
							if(first == 0)
							{
								first++;
								sql += "deviceuser = '";
							}
							else
								sql += " OR deviceuser = '";
							sql += (*iter).c_str();
							sql += "'";
						}
						sql += ")";

						printf("SQL : %s\n", sql.c_str());
						if(xx_sql_query(m_sql,sql.c_str()) == 0)
						{
							//正常
							MYSQL_RES *result = mysql_store_result(m_sql);
							MYSQL_ROW row;

							if((row = mysql_fetch_row(result)))
							{
								resxml += new_strprop("befor7data", "value", row[0]);
							}
							mysql_free_result(result);
						}
					}
					else
					{
						ret = 4;
						resxml += new_strprop("issuccess", "value", itoaa(ret));
					}
				}

				sql = "SELECT `value` FROM xxdb_info_versions";
				printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql,sql.c_str()) == 0)
				{
					//正常
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;
					int i = 0;
					while((row = mysql_fetch_row(result)))
					{
						i++;
						switch(i)
						{
							case 1:
								resxml += new_strprop("vioType", "value", row[0]);
								break;
							case 2:
								resxml += new_strprop("road", "value", row[0]);
								break;
							case 3:
								resxml += new_strprop("addressbook", "value", row[0]);
								break;
							default:
								break;
						}

					}
					mysql_free_result(result);
				}
			}
			else
			{
				ret = 4;
				resxml += new_strprop("issuccess", "value", itoaa(ret));
			}
		}
		else
			resxml += new_strprop("issuccess", "value", itoaa(ret));

		resxml += "</item1></resultdata>";

		if(ret == 1)
		{
			sql = "update xxdb_info_deviceuser set userlogintag = '";
			sql += username;
			sql += "' WHERE username = '";
			sql += xx_xml.xpath_getprop("/userdata/data/inputusername","value");
			sql += "'";
			xx_sql_query(m_sql,sql.c_str());
		}

		switch(dev)
		{
			case 0: userlog = "Web "; break;
			case 1: userlog = "Phone "; break;
			case 2: userlog = "PC Client "; break;
			default: break;
		}

		if(ret == 1)
			userlog += "Login successful!";
		else if(ret == 2)
			userlog += "Login Error! User is not Exist!";
		else if(ret == 3)
			userlog += "Login Error! DB Error!";
		else if(ret == 4)
			userlog += "Login Error! User do not have authority!";

		printf("RES:%s\n", userlog.c_str());

		sql = "INSERT xxdb_data_devconnlog (datetime,devid,userid,type) VALUES (NOW(),'";
		sql += xx_xml.xpath_getprop("/userdata/data/type", "value");
		sql += "','";
		sql += username;
		sql += "','";
		sql += userlog;
		sql += "')";
		xx_sql_query(m_sql, sql.c_str());

		setResXML(strTYPE, 1, 1, 1, resxml, gResLoginXML);
		*outxmldata = (char *)gResLoginXML.c_str();

		printf("OUTXML: %s\n", gResLoginXML.c_str());
	}

	return 0;
}

int xx_rcv_procinfo(char * inxmldata,char ** outxmldata)
{
	return xx_rcv_procinfo_login(inxmldata,outxmldata);
}
