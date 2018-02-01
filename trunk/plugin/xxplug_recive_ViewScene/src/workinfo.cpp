#include "xxplug_recive_ViewScene.h"
#include "chinese_code.h"
#include <dbmepolice.h>
#include <xxxml.h>
#include <vector>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

extern string gresXML;
extern MYSQL * m_sql;
extern string username;


int insertWork(cxx_xml xx_xml, char ** outxmldata)
{
	string	sql = "INSERT INTO xxdatabase.xxdb_data_patorl (patorltype,roadname,KM,DATETIME,memo,picnum,deviceuser,devid,flag,sole) VALUES (";

	sql.append(xx_xml.xpath_getprop("/userdata/data/patorltype", "value"));
	sql.append(",'");
	sql.append(xx_xml.xpath_getprop("/userdata/data/roadname", "value"));
	sql.append("',");
	sql.append(xx_xml.xpath_getprop("/userdata/data/KM", "value"));
	sql.append(",'");
	sql.append(xx_xml.xpath_getprop("/userdata/data/datetime", "value"));
	sql.append("','");
	sql.append(xx_xml.xpath_getprop("/userdata/data/memo", "value"));
	sql.append("',");
	sql.append(xx_xml.xpath_getprop("/userdata/data/picnum", "value"));
	sql.append(",'");
	sql.append(username.c_str());
	sql.append("','");
	sql.append(xx_xml.xpath_getprop("/userdata/head/dev", "value"));
	sql.append("',");
	sql.append("0,'");
	sql.append(xx_xml.xpath_getprop("/userdata/data/picsole", "value"));
	sql.append("')");

	//printf("SQL : %s\n", sql.c_str());
	int iresut = xx_sql_query(m_sql, sql.c_str());

	string strxml = "<resultdata><item1>";
	strxml += new_strprop("res", "value", itoaa(iresut));
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

int deleteWork(cxx_xml xx_xml, char ** outxmldata)
{
	string sql("DELETE FROM xxdb_data_patorl WHERE sole = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/picsole", "value"));
	sql.append("'");

	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0) iresut = 1;
	else iresut = 2;

	string strxml = "<resultdata><item1>";
	strxml += new_strprop("res", "value", itoaa(iresut));
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

int updateWork(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "UPDATE xxdb_data_patorl SET ";
	sql.append("patorltype = ");
	sql.append(xx_xml.xpath_getprop("/userdata/data/patorltype", "value"));
	sql.append(",");
	sql.append("roadname = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/roadname", "value"));
	sql.append("',");
	sql.append("KM = ");
	sql.append(xx_xml.xpath_getprop("/userdata/data/KM", "value"));
	sql.append(",");
	sql.append("datetime = ");
	sql.append(xx_xml.xpath_getprop("/userdata/data/datetime", "value"));
	sql.append(",");
	sql.append("memo = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/memo", "value"));
	sql.append("',");
	sql.append("picnum = ");
	sql.append(" WHERE sole = ");
	sql.append(xx_xml.xpath_getprop("/userdata/data/picsole", "value"));

	int iresut = xx_sql_query(m_sql, sql.c_str());

	if(iresut == 0) iresut = 1;
	else iresut = 2;

	string strxml = "<resultdata><item1>";
	strxml += new_strprop("res", "value", itoaa(iresut));
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

int selectWork(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string strtmp = "";
	vector<string> nameList;
	int sumcount = 0;
	int count = 0;
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	string strxml = "<resultdata>";

	GetGroupNameList(&nameList);

	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		sql = "SELECT patorltype,roadname,KM,datetime,memo,picnum,sole FROM xxdb_data_patorl WHERE 1=1 ";

		xx_xml.xpath_getprop("/userdata/data/patorltype", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND patorltype = ");
			sql += strtmp;
		}

		xx_xml.xpath_getprop("/userdata/data/roadname", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND roadname = '");
			sql += strtmp;
			sql += "'";
		}

		xx_xml.xpath_getprop("/userdata/data/KM", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND KM = ");
			sql += strtmp;
		}

		xx_xml.xpath_getprop("/userdata/data/starttime", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND datetime >= '");
			sql += strtmp;
			sql += "'";
		}

		xx_xml.xpath_getprop("/userdata/data/endtime", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND datetime <= '");
			sql += strtmp;
			sql += "'";
		}

		xx_xml.xpath_getprop("/userdata/data/memo", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND memo = '");
			sql += strtmp;
			sql += "'";
		}

		xx_xml.xpath_getprop("/userdata/data/name", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND deviceuser = '");
			sql += strtmp;
			sql += "'";
		}

		sql += " AND deviceuser = '";
		sql += (*iter).c_str();
		sql += "'";

		strtmp = "";

		//printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(result)))
			{
				sumcount++;
				if((firstindex < sumcount)&&(count < 10))
				{
					count++;
					strtmp = new_strprop("patorltype", "value", row[0]);
					strtmp += new_strprop("roadname", "value", row[1]);
					strtmp += new_strprop("KM", "value", row[2]);
					strtmp += new_strprop("datetime", "value", row[3]);
					strtmp += new_strprop("memo", "value", row[4]);
					strtmp += new_strprop("picnum", "value", row[5]);
					strtmp += new_strprop("picsole", "value", row[6]);

					strxml += setResItem(count, strtmp);
				}
			}
			mysql_free_result(result);
		}
	}

	strxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstindex, strxml, gresXML);

	//string out = "";
	//CChineseCode::UTF_8ToGB2312(out, (char *)gresXML.c_str(), gresXML.size());

	*outxmldata = (char *) gresXML.c_str();

	//printf("GBK : %s\n", out.c_str());

	return 0;
}

int selectWorkAll(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string strtmp = "";
	vector<string> nameList;
	int sumcount = 0;
	string strxml = "<resultdata>";

	GetGroupNameList(&nameList);

	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		sql = "SELECT patorltype,roadname,KM,datetime,memo,picnum,sole FROM xxdb_data_patorl WHERE 1=1 ";

		xx_xml.xpath_getprop("/userdata/data/starttime", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND datetime >= '");
			sql += strtmp;
			sql += "'";
		}

		xx_xml.xpath_getprop("/userdata/data/endtime", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND datetime <= '");
			sql += strtmp;
			sql += "'";
		}

		xx_xml.xpath_getprop("/userdata/data/name", "value", strtmp);
		if(strtmp.compare(""))
		{
			sql.append(" AND deviceuser = '");
			sql += strtmp;
			sql += "'";
		}

		sql += " AND deviceuser = '";
		sql += (*iter).c_str();
		sql += "'";

		strtmp = "";

		//printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(result)))
			{
				sumcount++;
				strtmp = new_strprop("patorltype", "value", row[0]);
				strtmp += new_strprop("roadname", "value", row[1]);
				strtmp += new_strprop("KM", "value", row[2]);
				strtmp += new_strprop("datetime", "value", row[3]);
				strtmp += new_strprop("memo", "value", row[4]);
				strtmp += new_strprop("picnum", "value", row[5]);
				strtmp += new_strprop("picsole", "value", row[6]);

				strxml += setResItem(sumcount, strtmp);
			}
			mysql_free_result(result);
		}
	}

	strxml += "</resultdata>";
	setResXML(TYPESTR, sumcount, sumcount, 0, strxml, gresXML);

	*outxmldata = (char *) gresXML.c_str();

	//printf("GBK : %s\n", out.c_str());

	return 0;
}

int GetNameList(string group, vector<string> * nameList)
{
	int count = 0;
	vector<string> groupname;

	string sql = "SELECT `group` FROM xxdb_info_usergroup WHERE 1=1";
	if(group.compare(""))
	{
		sql += " AND `group` = '";
		sql += group.c_str();
		sql += "'";
	}
	else
		sql += " AND `father` = '0'";

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(result)))
		{
			count++;
			string tmp;
			tmp = row[0];
			groupname.push_back(tmp);
		}
		mysql_free_result(result);
	}

	if(count == 0) return count;

	vector<string>::iterator iter = groupname.begin();
	for(int i = 0; i < count; i++)
	{
		sql = "SELECT `group` FROM xxdb_info_usergroup WHERE 1=1";
		if(iter->compare(""))
		{
			sql += " AND `father` = '";
			sql += iter->c_str();
			sql += "'";
		}

		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(result)))
			{
				string tmp;
				tmp = row[0];
				groupname.push_back(tmp);
			}
			mysql_free_result(result);
		}
		iter++;
	}

	count = 0;
	iter = groupname.begin();
	for(int i = 0; i < groupname.size(); i++)
	{
		sql = "SELECT `username` FROM xxdb_info_deviceuser WHERE 1=1";
		if(iter->compare(""))
		{
			sql += " AND `group` = '";
			sql += iter->c_str();
			sql += "'";
		}

		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(result)))
			{
				count++;
				string tmp;
				tmp = row[0];
				nameList->push_back(tmp);
			}
			mysql_free_result(result);
		}
		iter++;
	}

	return count;
}

int tidyWork(cxx_xml xx_xml, char ** outxmldata)
{
	int ret = 0;
	vector<string> name;
	string strxml = "<resultdata>";
	int sendcount = 0;
	unsigned int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));

	string sql = "";
	string strtmp = xx_xml.xpath_getprop("/userdata/data/group", "value");
	ret = GetNameList(strtmp, &name);
	//printf("name count = %d\n", name.size());

	if(ret != 0)
	{
		vector<string>::iterator iter = name.begin();
		for(unsigned int i = 0; i < name.size(); i++)
		{
			int count = 0;
			sql = "SELECT COUNT(0) FROM xxdb_data_patorl WHERE 1=1";
			sql += " AND `deviceuser` = '";
			sql += iter->c_str();
			sql += "'";
			sql += " AND `datetime` >= '";
			sql += xx_xml.xpath_getprop("/userdata/data/stime", "value");
			sql += "'";
			sql += " AND `datetime` <= '";
			sql += xx_xml.xpath_getprop("/userdata/data/etime", "value");
			sql += "'";

			//printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				if ((row = mysql_fetch_row(result)))
				{
					count = atoi(row[0]);
					//printf("count = %d\n", count);
				}
				mysql_free_result(result);
			}
			if(count != 0)
			{
				sql = "SELECT name FROM xxdb_info_deviceuser WHERE username = '";
				sql += iter->c_str();
				sql += "'";
				//printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql, sql.c_str()) == 0)
				{
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;
					if ((row = mysql_fetch_row(result)))
					{
						if((sendcount < 10)&&(firstindex <= i))
						{
							sendcount++;
							strtmp = new_strprop("username", "value", iter->c_str());
							strtmp += new_strprop("realname", "value", row[0]);
							strtmp += new_strprop("count", "value", itoaa(count));
							strxml += setResItem(sendcount, strtmp);
						}
					}
					mysql_free_result(result);
				}
			}
			iter++;
		}
	}

	strxml += "</resultdata>";
	setResXML(TYPESTR, sendcount, name.size(), firstindex, strxml, gresXML);

	*outxmldata = (char *) gresXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return ret;
}

int getWorktypeCount(cxx_xml xx_xml, char ** outxmldata)
{
	string strtmp = "";
	string strItem = "<resultdata>";
	int count = 0;
	string sql = "SELECT COUNT(*),patorltype FROM xxdb_data_patorl";
	sql += " WHERE datetime > '";
	sql += xx_xml.xpath_getprop("/userdata/data/stime", "value");
	sql += "'";
	sql += " AND datetime <= '";
	sql += xx_xml.xpath_getprop("/userdata/data/etime", "value");
	sql += "'";
	sql += " GROUP BY patorltype";

	int ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(result)))
		{
			count++;
			strtmp = new_strprop("patorltype", "value", row[1]);
			strtmp += new_strprop("patorltypecount", "value", row[0]);

			strItem += setResItem(count, strtmp);
		}
		mysql_free_result(result);
	}

	strItem += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, strItem, gresXML);

	*outxmldata = (char *) gresXML.c_str();

	return ret;
}

int getManworkCount(cxx_xml xx_xml, char ** outxmldata)
{
	string strtmp = "";
	string strItem = "<resultdata>";
	int count = 0;
	int sumcount = 0;
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	string sql = "SELECT COUNT(*) AS num, xxdb_data_patorl.deviceuser, xxdb_info_deviceuser.name FROM xxdb_data_patorl, xxdb_info_deviceuser \
			WHERE xxdb_data_patorl.deviceuser = xxdb_info_deviceuser.username ";
	sql += " AND xxdb_data_patorl.datetime > '";
	sql += xx_xml.xpath_getprop("/userdata/data/stime", "value");
	sql += "'";
	sql += " AND xxdb_data_patorl.datetime <= '";
	sql += xx_xml.xpath_getprop("/userdata/data/etime", "value");
	sql += "'";
	sql += " AND xxdb_data_patorl.patorltype = ";
	sql += xx_xml.xpath_getprop("/userdata/data/patorltype", "value");
	sql += " GROUP BY deviceuser";

	int ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(result)))
		{
			sumcount++;
			if((firstindex < sumcount) && (count < 1000))
			{
				count++;
				strtmp = new_strprop("name", "value", row[1]);
				strtmp += new_strprop("realname", "value", row[2]);
				strtmp += new_strprop("namecount", "value", row[0]);

				strItem += setResItem(count, strtmp);
			}
		}
		mysql_free_result(result);
	}

	strItem += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstindex, strItem, gresXML);

	*outxmldata = (char *) gresXML.c_str();

	return ret;
}
