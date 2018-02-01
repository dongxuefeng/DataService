/*
 * viodata.cpp
 *
 *  Created on: 2014-3-24
 *      Author: Administrator
 */

///////////

#include "xxplug_recive_UpdateVioData.h"
#include <dbmepolice.h>
#include <xxxml.h>
#include "chinese_code.h"

#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

extern string gresXML;
extern MYSQL * m_sql;

static int new_timestamp = 0;

int BlackIsUpdate(cxx_xml xx_xml, char ** outxmldata)
{
	int count = 0;
	string	sql ("SELECT MAX(TIMESTAMP) FROM xxdb_data_blacklist");

	printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if((row = mysql_fetch_row(result)))
		{
			new_timestamp = atoi(row[0]);
			if(new_timestamp == 0) count = 0;
			else if(atoi(xx_xml.xpath_getprop("/userdata/data/timestamp", "value")) <= new_timestamp)
				count = 1;

			printf("new_timestamp = %d, /userdata/data/timestamp = %s\n", new_timestamp, xx_xml.xpath_getprop("/userdata/data/timestamp", "value"));
		}
		else
		{
			printf("mysql_fetch_row error!\n");
			count = 0;
		}
		mysql_free_result(result);
	}
	string strxml = "<resultdata><item1>";
	strxml += new_strprop("update", "value", itoaa(count));
	strxml += "</item1></resultdata>";

	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("outXML : %s\n", *outxmldata);

	return 0;
}

int GetBlackCount(cxx_xml xx_xml, char ** outxmldata)
{
	int count = 0;
	string	sql ("SELECT COUNT(1) FROM xxdb_data_blacklist WHERE `timestamp` = ");
	sql.append(itoaa(atoi(xx_xml.xpath_getprop("/userdata/data/timestamp", "value"))));

	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		while((row = mysql_fetch_row(result)))
		{
			count = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	string strxml = "<resultdata><item1>";
	strxml += new_strprop("timestamp", "value", itoaa(new_timestamp));
	strxml += new_strprop("count", "value", itoaa(count));
	strxml += "</item1></resultdata>";

	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("outXML : %s\n", *outxmldata);

	return iresut;
}

int GetBlack(cxx_xml xx_xml, char ** outxmldata)
{
	int count = 0;
	string	sql ("SELECT platenum,platetype,blacklisttype,timestamp,blt,carcolor,brand,owner,altertype,id FROM xxdb_data_blacklist WHERE `timestamp` = ");
	sql.append(itoaa(atoi(xx_xml.xpath_getprop("/userdata/data/timestamp", "value"))));
	sql.append(" limit ");
	sql.append(xx_xml.xpath_getprop("/userdata/data/indexid", "value"));
	sql.append(",");
	sql.append(xx_xml.xpath_getprop("/userdata/data/count", "value"));

	string strxml = "<resultdata><item1>";
	string strtmp = "";
	strxml += new_strprop("timestamp", "value", itoaa(atoi(xx_xml.xpath_getprop("/userdata/data/timestamp", "value"))+1));

	printf("SQL : %s\n", sql.c_str());

	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			count++;
			setNumTag("plateinfo", count, &strtmp);
			strtmp += new_strprop("sid", "value", row[9]);
			strtmp += new_strprop("platenum", "value", row[0]);
			strtmp += new_strprop("platetype", "value", row[1]);
			strtmp += new_strprop("blacklisttype", "value", row[2]);
			strtmp += new_strprop("timestamp", "value", row[3]);
			strtmp += new_strprop("blt", "value", row[4]);
			strtmp += new_strprop("carcolor", "value", row[5]);
			strtmp += new_strprop("brand", "value", row[6]);
			strtmp += new_strprop("owner", "value", row[7]);
			strtmp += new_strprop("altertype", "value", row[8]);
			setNumTag("/plateinfo", count, &strtmp);
		}
		mysql_free_result(result);
	}

	strxml += new_strprop("platecount", "value", itoaa(count));
	strxml += strtmp;
	strxml += "</item1></resultdata>";

	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("outXML : %s\n", *outxmldata);

	return iresut;
}

int InsertBlack(cxx_xml xx_xml, char ** outxmldata)
{
	int timestamp = 0;
	string sql = "SELECT timestamp FROM xxdatabase.xxdb_data_blacklist ORDER BY TIMESTAMP DESC";
	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if((row = mysql_fetch_row(result)))
		{
			timestamp = atoi(row[0]);
		}
		else
		{
			printf("mysql_fetch_row error!\n");
			timestamp = 0;
		}
		mysql_free_result(result);
	}

	sql = "INSERT INTO xxdatabase.xxdb_data_blacklist (platenum,platetype,blacklisttype,\
			      timestamp,blt,carcolor,brand,owner,altertype) values ('";
	sql.append(xx_xml.xpath_getprop("/userdata/data/platenum", "value"));
	sql.append("','");
	sql.append(xx_xml.xpath_getprop("/userdata/data/platetype", "value"));
	sql.append("','");
	sql.append(xx_xml.xpath_getprop("/userdata/data/blacklisttype", "value"));
	sql.append("','");
	sql.append(itoaa(timestamp+1));
	sql.append("','");
	sql.append(xx_xml.xpath_getprop("/userdata/data/blt", "value"));
	sql.append("','");
	sql.append(xx_xml.xpath_getprop("/userdata/data/carcolor", "value"));
	sql.append("','");
	sql.append(xx_xml.xpath_getprop("/userdata/data/brand", "value"));
	sql.append("','");
	sql.append(xx_xml.xpath_getprop("/userdata/data/owner", "value"));
	sql.append("',1)");

	iresut = xx_sql_query(m_sql, sql.c_str());

	if(iresut == 0) iresut = 1;
	else	iresut = 2;

	string strtmp = new_strprop("res", "value", itoaa(iresut));
	string strtmp1 = setResItem(1, strtmp);
	string strtmp11 = "<resultdata>";
	strtmp11 += strtmp1;
	strtmp11 += "</resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strtmp11, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("%s\n", *outxmldata);

	return iresut;
}

int DeleteBlack(cxx_xml xx_xml, char ** outxmldata)
{
	int timestamp = 0;
	string sql = "SELECT MAX(TIMESTAMP) FROM xxdb_data_blacklist";
	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if((row = mysql_fetch_row(result)))
		{
			timestamp = atoi(row[0]);
		}
		else
		{
			printf("mysql_fetch_row error!\n");
			timestamp = 0;
		}
		mysql_free_result(result);
	}

	sql = "UPDATE xxdatabase.xxdb_data_blacklist SET ";
	sql.append("timestamp = ");
	sql.append(itoaa(timestamp+1));
	sql.append(",");
	sql.append("altertype = 2");
	sql.append(" WHERE platenum = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/platenum", "value"));
	sql.append("' ");
	sql.append("AND platetype = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/platetype", "value"));

	sql.append("' AND vioid = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/vioid", "value"));
	sql.append("'");
	iresut = xx_sql_query(m_sql, sql.c_str());

	if(iresut == 0) iresut = 1;
	else	iresut = 2;

	string strtmp = new_strprop("res", "value", itoaa(iresut));
	string strtmp1 = setResItem(1, strtmp);
	string strtmp11 = "<resultdata>";
	strtmp11 += strtmp1;
	strtmp11 += "</resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strtmp11, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("%s\n", *outxmldata);

	return iresut;
}

int UpdateBlack(cxx_xml xx_xml, char ** outxmldata)
{
	int timestamp = 0;
	string sql = "SELECT MAX(TIMESTAMP) FROM xxdb_data_blacklist";
	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if((row = mysql_fetch_row(result)))
		{
			timestamp = atoi(row[0]);
		}
		else
		{
			printf("mysql_fetch_row error!\n");
			timestamp = 0;
		}
		mysql_free_result(result);
	}

	sql = "UPDATE xxdatabase.xxdb_data_blacklist SET ";
	sql.append("timestamp = ");
	sql.append(itoaa(timestamp+1));
	sql.append(",");
	sql.append("blt = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/blt", "value"));
	sql.append("',");
	sql.append("carcolor = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/carcolor", "value"));
	sql.append("',");
	sql.append("brand = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/brand", "value"));
	sql.append("',");
	sql.append("owner = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/owner", "value"));
	sql.append("',");
	sql.append("altertype = 3");
	sql.append(" WHERE platenum = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/platenum", "value"));
	sql.append("' ");
	sql.append("AND platetype = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/platetype", "value"));
	sql.append("'");
	sql.append("AND  vioid= '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/vioid", "value"));
	sql.append("'");

	iresut = xx_sql_query(m_sql, sql.c_str());

	if(iresut == 0) iresut = 1;
	else	iresut = 2;

	string strtmp = new_strprop("res", "value", itoaa(iresut));
	string strtmp1 = setResItem(1, strtmp);
	string strtmp11 = "<resultdata>";
	strtmp11 += strtmp1;
	strtmp11 += "</resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strtmp11, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("%s\n", *outxmldata);

	return iresut;
}

int SelectBlack(cxx_xml xx_xml, char ** outxmldata)
{
	int timestamp = 0;
	string sql = "SELECT MAX(TIMESTAMP) FROM xxdb_data_blacklist";
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	//printf("SQL : %s\n", sql.c_str());
	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if((row = mysql_fetch_row(result)))
		{
			timestamp = atoi(row[0]);
		}
		else
		{
			printf("mysql_fetch_row error!\n");
			timestamp = 0;
		}
		mysql_free_result(result);
	}

	int sumcount = 0;
	int count = 0;
	sql = "SELECT platenum,platetype,blacklisttype,timestamp,blt,carcolor,brand,owner,altertype,`vioid`FROM xxdatabase.xxdb_data_blacklist WHERE 1=1 ";

	string strtmp = "";
	string strtmp1 = "";

	xx_xml.xpath_getprop("/userdata/data/platenum", "value", strtmp);
	if(strtmp.compare(""))
	{
		sql.append("AND platenum = '");
		sql += strtmp;
		sql.append("' ");
	}

	xx_xml.xpath_getprop("/userdata/data/platetype", "value", strtmp);
	if(strtmp.compare(""))
	{
		sql.append(" AND platetype = ");
		sql += strtmp;
	}

	sql.append(" AND altertype != 2");

	//printf("SQL : %s\n", sql.c_str());

	iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			sumcount++;
			if((firstindex < sumcount)&&(count < 10))
			{
				strtmp = "";
				count++;
				strtmp += new_strprop("platenum", "value", row[0]);
				strtmp += new_strprop("platetype", "value", row[1]);
				strtmp += new_strprop("blacklisttype", "value", row[2]);
				strtmp += new_strprop("timestamp", "value", row[3]);
				strtmp += new_strprop("blt", "value", row[4]);
				strtmp += new_strprop("carcolor", "value", row[5]);
				strtmp += new_strprop("brand", "value", row[6]);
				strtmp += new_strprop("owner", "value", row[7]);
				strtmp += new_strprop("altertype", "value", row[8]);
				strtmp += new_strprop("vioid", "value", row[9]);//添加vioid
				strtmp1 += setResItem(count, strtmp);
			}

		}
		mysql_free_result(result);
	}

	string strtmp11 = "<resultdata>";
	strtmp11 += strtmp1;
	strtmp11 += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstindex, strtmp11, gresXML);

	*outxmldata = (char *) gresXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return iresut;
}
