/*
 * viodata.cpp
 *
 *  Created on: 2014-3-24
 *      Author: Administrator
 */

///////////

#include "xxplug_recive_UpdateVioData.h"
#include "RWexcel.h"
#include <dbmepolice.h>
#include <xxxml.h>
#include <vector>
#include <xxbase64.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <io.h>
#include <tchar.h>
#include <pthread.h>
#include "chinese_code.h"
#include "makezip.h"
#include <day.h>

using namespace std;

extern string gresXML;
extern MYSQL * m_sql;
extern string username;
extern string picpath;
extern string downloadPath;

extern void * mkzip;

typedef struct
{
	char platenum[20];
	char platetype[3];
	char stime[24];
	char etime[24];
	int  affirm;
	int  download;
	int  disposetype;
	int  datafrom;
}pack;


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


char jpegpic[1024*1024*5] = {0};
int savepicfile(const char * filename, const char * filedata)
{
	char pathname[256]={0};
	sprintf(pathname,"%s/%s", picpath.c_str(), filename);
	memset(jpegpic, 0, 1024*1024*5);
	int datalen = b64_decode_string((const unsigned char *)filedata, strlen(filedata), (unsigned char *)jpegpic);
	FILE * fp = fopen(pathname, "wb");
	if(fp)
	{
		fwrite(jpegpic, 1, datalen, fp);
		fclose(fp);
		return 0;
	}
	return 1;
}

int getviodatafilename(const char * dev, const char * datetime, int index, string &subfileurl)
{
	SYSTEMTIME sys;
	char fileurl[256] = {0};

	GetLocalTime(&sys);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "mkdir %s\\vio\\%d\\%d\\%d", picpath.c_str(), sys.wYear, sys.wMonth, sys.wDay);
	system(fileurl);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "vio/%d/%d/%d/%s_%s_%d.jpg", sys.wYear, sys.wMonth, sys.wDay, dev, datetime, index);
	printf("getviodatafilename filename = %s\n", fileurl);
	for (char *p = fileurl; *p != '\0'; *p++)
	{
		if(*p == ':'||*p == '-'||*p == ' ')
			*p = '_';
	}

	subfileurl = fileurl;
	return 0;
}

int getdisposepicfilename(const char * vioid, const char * datetime, string &subfileurl)
{
	SYSTEMTIME sys;
	char fileurl[256] = {0};

	GetLocalTime(&sys);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "mkdir %s\\viodispose\\%d\\%d\\%d", picpath.c_str(), sys.wYear, sys.wMonth, sys.wDay);
	system(fileurl);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "viodispose/%d/%d/%d/%s_%s.jpg", sys.wYear, sys.wMonth, sys.wDay, vioid, datetime);

	for (char *p = fileurl; *p != '\0'; *p++)
	{
		if(*p == ':'||*p == '-'||*p == ' ')
			*p = '_';
	}

	subfileurl = fileurl;
	return 0;
}

char jpegbuf[1024*1024*5] = {0};
int ReadVioPicFile2B64(const char * filename, char * pb64buf)
{
	int readsize = 0;

	string path = picpath;
	path += "/";
	path += filename;

	FILE * f = fopen(path.c_str(), "rb");
	if(f)
	{
		readsize = fread(jpegbuf, 1, 1024*1024*10, f);
		fclose(f);
		int size = b64_encode_string((const unsigned char *)jpegbuf, readsize, (unsigned char *)pb64buf);
		return size;
	}
	else
		printf("fopen error!\n");

	return 0;
}

int insertViodata(cxx_xml xx_xml, char ** outxmldata)
{
	//录入违法库
	string kakoudate = xx_xml.xpath_getprop("/userdata/data/kakoudate", "value");
	string strxml = "<resultdata><item1>";
	string sql = "";
	int isExist = 0, res = 0;

	sql = "SELECT * FROM xxdb_data_viodata WHERE platenum = '";
	sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	sql += "' AND platetype = '";
	sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	sql += "' AND DATETIME = '";
	sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");
	sql += "'";

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
			isExist = 1;//违法数据已存在
		else
			isExist = 0;//违法数据不存在

		mysql_free_result(result);
	}

	if(isExist == 0)
	{
		printf("This vio is new!\n");
		sql = "	INSERT INTO xxdatabase.xxdb_data_viodata ( DATETIME, datetime_end, roadname, \
				platenum, platetype, picnum, picurl0, picurl1, 	picurl2, picurl3, \
				viotype, deviceuser, device, affirm, flag, download, vioid, disposetype,datafrom)\
				VALUES ";
		sql.append("('");
		sql.append(xx_xml.xpath_getprop("/userdata/data/datetime", "value"));
		sql.append("','");
		sql.append(xx_xml.xpath_getprop("/userdata/data/datetimeEnd", "value"));
		sql.append("','");
		sql.append(xx_xml.xpath_getprop("/userdata/data/roadname", "value"));
		sql.append("','");
		sql.append(xx_xml.xpath_getprop("/userdata/data/platenum", "value"));
		sql.append("','");
		sql.append(xx_xml.xpath_getprop("/userdata/data/platetype", "value"));
		sql.append("','");
		sql.append(xx_xml.xpath_getprop("/userdata/data/picnum", "value"));
		sql.append("','");

		int strpicnum = atoi(xx_xml.xpath_getprop("/userdata/data/picnum","value"));
		string strfilepath="";
		string picdata;

		if(strpicnum > 0)
		{
			getviodatafilename(xx_xml.xpath_getprop("/userdata/head/dev","value"), xx_xml.xpath_getprop("/userdata/data/datetime", "value"), 0, strfilepath);
			sql.append(strfilepath.c_str());
			xx_xml.xpath_gettext("/userdata/data/viopic1/picdata",picdata);
			printf("insertViodata filename = %s\n", strfilepath.c_str());
			savepicfile(strfilepath.c_str(),picdata.c_str());
		}
		sql.append("','");
		if(strpicnum > 1)
		{
			getviodatafilename(xx_xml.xpath_getprop("/userdata/head/dev","value"), xx_xml.xpath_getprop("/userdata/data/datetime", "value"), 1, strfilepath);
			sql.append(strfilepath.c_str());
			xx_xml.xpath_gettext("/userdata/data/viopic2/picdata",picdata);
			savepicfile(strfilepath.c_str(),picdata.c_str());
		}
		sql.append("','");
		if(strpicnum > 2)
		{
			getviodatafilename(xx_xml.xpath_getprop("/userdata/head/dev","value"), xx_xml.xpath_getprop("/userdata/data/datetime", "value"), 2, strfilepath);
			sql.append(strfilepath.c_str());
			xx_xml.xpath_gettext("/userdata/data/viopic3/picdata",picdata);
			savepicfile(strfilepath.c_str(),picdata.c_str());
		}
		sql.append("','");
		if(strpicnum > 3)
		{
			getviodatafilename(xx_xml.xpath_getprop("/userdata/head/dev","value"), xx_xml.xpath_getprop("/userdata/data/datetime", "value"), 3, strfilepath);
			sql.append(strfilepath.c_str());
			xx_xml.xpath_gettext("/userdata/data/viopic4/picdata",picdata);
			savepicfile(strfilepath.c_str(),picdata.c_str());
		}
		sql.append("','");
		sql.append(xx_xml.xpath_getprop("/userdata/data/viotype", "value"));
		sql.append("','");
		sql.append(username.c_str());
		sql.append("','");
		sql.append(xx_xml.xpath_getprop("/userdata/head/dev", "value"));
		sql.append("','");

		//如果为卡口数据则  确认状态为 已确认 即 1  若不为卡口数据  则需要判断是否为 现场处理数据
		if(kakoudate.compare("") == 1)
			sql += "1";
		else
			sql.append(xx_xml.xpath_getprop("/userdata/data/disposetype", "value"));

		sql.append("','");
		sql.append("0");
		sql.append("','");
		sql.append("0");
		sql.append("','");
		sql.append(xx_xml.xpath_getprop("/userdata/data/vioid", "value"));
		sql.append("',");
		sql.append(xx_xml.xpath_getprop("/userdata/data/disposetype", "value"));
		sql.append(",");
		if(kakoudate.compare("") == 1)
			sql.append("1");
		else
			sql.append("0");
		sql.append(")");

		printf("SQL : %s\n", sql.c_str());
		res = xx_sql_query(m_sql, sql.c_str());
	}
	else
	{
		printf("This vio is Exist!\n");
		res = 0;
	}

	strxml += new_strprop("res", "value", itoaa(res));
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	//若为卡口数据 则直接录入黑名单库
	if((kakoudate.compare("") == 1) && (isExist == 0))
	{
		int timestamp;
		sql = "SELECT MAX(TIMESTAMP) FROM xxdb_data_blacklist";
		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
			{
				timestamp = atoi(row[0]);
			}
			else
				timestamp = 0;
			mysql_free_result(result);
		}

		sql = "SELECT COUNT(0) FROM xxdb_data_blacklist WHERE `timestamp` = ";
		sql += itoaa(timestamp);
		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
			{
				if(atoi(row[0]) >= 50) timestamp++;
			}
			mysql_free_result(result);
		}
         //插入黑名单，添加vioid字段
		sql = "INSERT INTO xxdb_data_blacklist (datetime,platenum, platetype, `timestamp`, altertype, blacklisttype, blt,vioid) VALUES (NOW(),'";
		sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
		sql += "','";
		sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
		sql += "',";
		sql += itoaa(timestamp);
		sql += ",1,1,'本地违法','";
		sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
		sql +="')";
		printf("SQL : %s\n", sql.c_str());
		xx_sql_query(m_sql, sql.c_str());
	}
	return 0;
}

int affirmVioData(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";

	string sql = "UPDATE xxdb_data_viodata SET affirm = ";
	sql += xx_xml.xpath_getprop("/userdata/data/affirm", "value");
	if(atoi(xx_xml.xpath_getprop("/userdata/data/affirm", "value")) == 1)
	{
		sql += ", platenum = \"";
		sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
		sql += "\", platetype = \"";
		sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
		sql += "\", roadname = \"";
		sql += xx_xml.xpath_getprop("/userdata/data/vioplace", "value");
		sql += "\", viotype = ";
		sql += xx_xml.xpath_getprop("/userdata/data/viotype", "value");
		sql += ", datetime = \"";
		sql += xx_xml.xpath_getprop("/userdata/data/time", "value");
		sql += "\"";
	}
	sql += " WHERE vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";

	int res = xx_sql_query(m_sql, sql.c_str());
	if(res == 0) res = 1;
	else res = 2;
	strxml += new_strprop("res", "value", itoaa(res));
	strxml += "</item1></resultdata>";

	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	//确认后的数据了录入黑名单库
	int timestamp;
	sql = "SELECT MAX(TIMESTAMP) FROM xxdb_data_blacklist";
	printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			timestamp = atoi(row[0]);
		}
		else
			timestamp = 0;
		mysql_free_result(result);
	}

	//web确认手机上传的违法数据，录入黑名单
	sql = "INSERT INTO xxdb_data_blacklist (datetime,platenum, platetype, `timestamp`, altertype, blacklisttype, blt,vioid) VALUES (NOW(),'";
	sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	sql += "',";
	sql += itoaa(++timestamp);

	sql += ",1,0,'本地违法','";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql +="')";

	printf("SQL : %s\n", sql.c_str());
	xx_sql_query(m_sql, sql.c_str());

	return 0;
}

int insertSpotDispose(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	string sql = "INSERT INTO xxdb_data_viodispose (vioid, disposetype, money) VALUES ('";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "',";
	sql += xx_xml.xpath_getprop("/userdata/data/disposetype", "value");
	sql += ",";
	sql += xx_xml.xpath_getprop("/userdata/data/money", "value");
	sql += ")";

	int res = xx_sql_query(m_sql, sql.c_str());
	strxml += new_strprop("res", "value", itoaa(res));
	strxml += "</item1></resultdata>";

	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

int insertDisposePic(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	string strfilepath = "";
	string strpic = "";
	SYSTEMTIME t;
	char tmp[256] = {0};

	GetLocalTime(&t);
	sprintf(tmp, "%d_%d_%d_%d_%d_%d_%d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	getdisposepicfilename(xx_xml.xpath_getprop("/userdata/data/vioid","value"), tmp, strfilepath);
	xx_xml.xpath_gettext("/userdata/data/pic/picdata", strpic);
	savepicfile(strfilepath.c_str(), strpic.c_str());

	string sql = "INSERT INTO xxdb_data_viodisposepic (vioid, picpath) VALUES ('";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "','";
	sql += strfilepath;
	sql += "')";

	int res = xx_sql_query(m_sql, sql.c_str());
	strxml += new_strprop("res", "value", itoaa(res));
	strxml += "</item1></resultdata>";

	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

int selectLocalViodate(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata>";
	string str = "";
	int count = 0, sumcount = 0;

	string sql = "SELECT COUNT(1) FROM xxdb_data_viodata WHERE 1=1";
	str = xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	if(str.compare("") != 0)
	{
		sql += " AND platenum = '";
		sql += str;
		sql += "'";
	}
	str = xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	if(str.compare("") != 0)
	{
		sql += " AND platetype = '";
		sql += str;
		sql += "'";
	}
	str = xx_xml.xpath_getprop("/userdata/data/affirm", "value");
	if(str.compare("") != 0)
	{
		sql += " AND affirm = ";
		sql += str;
	}
	str = xx_xml.xpath_getprop("/userdata/data/stime", "value");
	if(str.compare("") != 0)
	{
		sql += " AND datetime >= '";
		sql += str;
		sql += "'";
	}
	str = xx_xml.xpath_getprop("/userdata/data/etime", "value");
	if(str.compare("") != 0)
	{
		sql += " AND datetime <= '";
		sql += str;
		sql += "'";
	}
	str = xx_xml.xpath_getprop("/userdata/data/download", "value");
	if(str.compare("") != 0)
	{
		sql += " AND download = '";
		sql += str;
		sql += "'";
	}
	str = xx_xml.xpath_getprop("/userdata/data/disposetype", "value");
	if(str.compare("") != 0)
	{
		sql += " AND disposetype = '";
		sql += str;
		sql += "'";
	}

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			sumcount = atoi(row[0]);
		}
		else
			sumcount = 0;
		mysql_free_result(result);
	}

	if(sumcount != 0)
	{
		sql = "SELECT vioid,datetime,viotype,roadname,affirm,picnum,platenum,platetype, disposetype FROM xxdb_data_viodata WHERE 1=1";
		str = xx_xml.xpath_getprop("/userdata/data/platenum", "value");
		if(str.compare("") != 0)
		{
			sql += " AND platenum = '";
			sql += str;
			sql += "'";
		}
		str = xx_xml.xpath_getprop("/userdata/data/platetype", "value");
		if(str.compare("") != 0)
		{
			sql += " AND platetype = '";
			sql += str;
			sql += "'";
		}
		str = xx_xml.xpath_getprop("/userdata/data/affirm", "value");
		if(str.compare("") != 0)
		{
			sql += " AND affirm = ";
			sql += str;
		}
		str = xx_xml.xpath_getprop("/userdata/data/stime", "value");
		if(str.compare("") != 0)
		{
			sql += " AND datetime >= '";
			sql += str;
			sql += "'";
		}
		str = xx_xml.xpath_getprop("/userdata/data/etime", "value");
		if(str.compare("") != 0)
		{
			sql += " AND datetime <= '";
			sql += str;
			sql += "'";
		}
		str = xx_xml.xpath_getprop("/userdata/data/download", "value");
		if(str.compare("") != 0)
		{
			sql += " AND download = '";
			sql += str;
			sql += "'";
		}
		str = xx_xml.xpath_getprop("/userdata/data/disposetype", "value");
		if(str.compare("") != 0)
		{
			sql += " AND disposetype = '";
			sql += str;
			sql += "'";
		}

		sql += " LIMIT ";
		sql += xx_xml.xpath_getprop("/userdata/head/Request_Index", "value");
		sql += ",10";
		printf("sql = %s \n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			string strtmp;
			while((row = mysql_fetch_row(result)))
			{
				count++;
				strtmp = new_strprop("vioid", "value", row[0]);
				strtmp += new_strprop("time", "value", row[1]);
				strtmp += new_strprop("viotype", "value", row[2]);
				strtmp += new_strprop("vioplace", "value", row[3]);
				strtmp += new_strprop("viostate", "value", row[4]);
				strtmp += new_strprop("picnum", "value", row[5]);
				strtmp += new_strprop("platenum", "value", row[6]);
				strtmp += new_strprop("platetype", "value", row[7]);
				strtmp += new_strprop("disposetype", "value", row[8]);
				strxml += setResItem(count, strtmp);
			}
			mysql_free_result(result);
		}
	}

	strxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value")), strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("outxml = %s\n", *outxmldata);

	string gbkstr = "";
	CChineseCode::UTF_8ToGB2312(gbkstr, (char *)gresXML.c_str(), gresXML.size());
	printf("GBK : %s\n", gbkstr.c_str());

	return 0;
}

char b64buf[1024*1024*5];
int selectLocalViopic(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	string filename = "";
	int index = atoi(xx_xml.xpath_getprop("/userdata/data/picindexvalue", "value"));
	string sql = "SELECT picurl0,picurl1,picurl2,picurl3 FROM xxdb_data_viodata WHERE vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			switch(index)
			{
			case 0: filename.append(row[0]); break;
			case 1: filename.append(row[1]); break;
			case 2: filename.append(row[2]); break;
			case 3: filename.append(row[3]); break;
			default:
				break;
			}
			memset(b64buf,0,1024*1024*5);
			int size = ReadVioPicFile2B64(filename.c_str(), b64buf);
			strxml += new_strprop("picsize", "value", itoaa(size));
			strxml += "<picdata>";
			if(size != 0) strxml += b64buf;
			strxml += "</picdata>";
		}
		mysql_free_result(result);
	}

	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

int selectUnaffirmData(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	string sql = "";
	int sumcount = 0, data = 0;
	vector<string> nameList;

	GetNameList(&nameList);

	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		sql = "SELECT COUNT(1) from xxdb_data_viodata WHERE affirm = 0 AND deviceuser = '";
		sql += (*iter).c_str();
		sql += "'";

		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
				sumcount += atoi(row[0]);

			mysql_free_result(result);
		}

		printf("sumcount = %d\n", sumcount);
		if((sumcount > 0) && (data == 0))
		{
			printf("-------------------------\n");
			data = 1;

			sql = "SELECT vioid,datetime,viotype,roadname,affirm,picnum,platenum,platetype,disposetype FROM xxdb_data_viodata WHERE affirm = 0 AND deviceuser = '";
			sql += (*iter).c_str();
			sql += "' ORDER BY DATETIME ASC LIMIT 0,1 ";
			printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if((row = mysql_fetch_row(result)))
				{
					strxml += new_strprop("vioid", "value", row[0]);
					strxml += new_strprop("time", "value", row[1]);
					strxml += new_strprop("viotype", "value", row[2]);
					strxml += new_strprop("vioplace", "value", row[3]);
					strxml += new_strprop("viostate", "value", row[4]);
					strxml += new_strprop("picnum", "value", row[5]);
					strxml += new_strprop("platenum", "value", row[6]);
					strxml += new_strprop("platetype", "value", row[7]);
					strxml += new_strprop("disposetype", "value", row[8]);
				}
				mysql_free_result(result);
			}
		}
	}
	strxml += "</item1></resultdata>";

	setResXML(TYPESTR, data, sumcount, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUTXML: %s\n", *outxmldata);

	return 0;
}

int selectAffirmData(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	int picnum = 0;

	string sql = "SELECT COUNT(1) FROM xxdb_data_viodisposepic WHERE vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			picnum = atoi(row[0]);
		}
		else
			picnum = 0;
		mysql_free_result(result);
	}

	sql = "SELECT disposetype,money FROM xxdb_data_viodispose WHERE vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			strxml += new_strprop("disposetype", "value", row[0]);
			strxml += new_strprop("money", "value", row[1]);
			strxml += new_strprop("picnum", "value", itoaa(picnum));
		}
		mysql_free_result(result);
	}

	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUTXML : %s\n", gresXML.c_str());
	return 0;
}

char b64buf2[1024*1024*5];
int selectAffirmPic(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";

	string sql = "SELECT picpath FROM xxdb_data_viodisposepic WHERE vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "' LIMIT ";
	sql += xx_xml.xpath_getprop("/userdata/data/picindexvalue", "value");
	sql += ",1";

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			memset(b64buf2,0,1024*1024*5);
			int size = ReadVioPicFile2B64(row[0], b64buf2);
			strxml += new_strprop("picsize", "value", itoaa(size));
			strxml += "<picdata>";
			if(size != 0) strxml += b64buf2;
			strxml += "</picdata>";
		}
		mysql_free_result(result);
	}

	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUTXML : %s\n", gresXML.c_str());
	return 0;
}

int MCopyFile(char * sourceFile, char * newFile)
{
	ifstream in;
	ofstream out;

	//printf("sourceFile = %s\n", sourceFile);
	//printf("newFile = %s\n", newFile);
	in.open(sourceFile, ios::binary);
	if(in.fail())
	{
		printf("open file error\n");
		in.close();
		out.close();
		return -1;
	}
	out.open(newFile, ios::binary);
	if(out.fail())
	{
		printf("create new file error\n");
		in.close();
		out.close();
		return -2;
	}else
	{
		out << in.rdbuf();
		out.close();
		in.close();
		return 0;
	}
}

char * getPicName(char *url)
{
	char *t = url;
	char *name;
	while(1)
	{
		name = strstr(t, "/");
		if(name == NULL)
			break;
		t = name + 1;
	}

	return t;
}

void changSymbol(char * src)
{
	char * p = src;
	int len = strlen(src);

	for(int i = 0; i < len; i++)
	{
		if(p[i] == '/') p[i] = '\\';
	}
}

void xlsHeadInit(rwExcel * xls)
{
	char tmp[20][256];
	char * p[20];

	for(int i = 0; i < 20; i++)
		p[i] = tmp[i];
	//初始化excel 表格 文件头
	printf("------------excel head------------\n");
	memset(tmp, 0, 20*256);
	sprintf(tmp[0], "序号");
	sprintf(tmp[1], "号牌号码");
	sprintf(tmp[2], "号牌种类");
	sprintf(tmp[3], "违法时间");
	sprintf(tmp[4], "车辆类型");
	sprintf(tmp[5], "违法地点");
	sprintf(tmp[6], "违法类型");
	sprintf(tmp[7], "行驶方向");
	sprintf(tmp[8], "车道");
	sprintf(tmp[9], "设备编号");
	sprintf(tmp[10], "超速类型");
	sprintf(tmp[11], "开始时间");
	sprintf(tmp[12], "结束时间");
	sprintf(tmp[13], "行驶速度");
	sprintf(tmp[14], "IMG1");
	sprintf(tmp[15], "IMG2");
	sprintf(tmp[16], "IMG2");
	xls->NewSheet("new sheet", p, 17);
	printf("------------end head------------\n");
}

void packData(rwExcel * xls, char * platenum, char * platetype, char * datetime, char * roadname, char * viotype, char * pic0, char * pic1, char * pic2, int &count,char * downPicPath, char * downpicDir)
{
	char tmp[20][256];
	char * p[20];
	string srcPicPath = "";

	for(int i = 0; i < 20; i++)
		p[i] = tmp[i];

	memset(tmp, 0, 20*256);
	sprintf(tmp[0], "%d", count);
	sprintf(tmp[1], platenum);
	sprintf(tmp[2], platetype);
	sprintf(tmp[3], datetime);
	sprintf(tmp[5], roadname);
	switch(atoi(viotype))
	{
	case 0://违停
		sprintf(tmp[6], "1039");
		break;
	case 1://逆行
		sprintf(tmp[6], "1301");
		break;
	case 2://遮挡号牌
		sprintf(tmp[6], "1614");
		break;
	case 3://变更号牌
		sprintf(tmp[6], "1614");
		break;
	case 4://超员
		sprintf(tmp[6], "10480");
		break;
	case 5://占用专用车道
		sprintf(tmp[6], "1019");
		break;
	default:
		break;
	}


	char picname[128] = {0};
	char xlspicname[128] = {0};

	//first pic
	if(pic0 != NULL)
	if(strlen(pic0) != 0)
	{
		//printf("row[5] = %s\n", row[5]);
		srcPicPath = picpath + "/" + pic0;
		//printf("srcPicPath = %s\n", srcPicPath.c_str());

		memset(picname, 0, 128);
		memset(xlspicname, 0, 128);
		sprintf(picname, "%s", getPicName(pic0));
		//printf("picname = %s\n", picname);
		sprintf(xlspicname, "%s/%s", downPicPath, picname);

		//if(MCopyFile((char *)srcPicPath.c_str(), xlspicname) == 0)
		if(CopyFile((char *)srcPicPath.c_str(), xlspicname, true))
		{
			memset(xlspicname, 0, 128);
			sprintf(xlspicname, "%s/%s", downpicDir, picname);
			sprintf(tmp[14], picname);
		}

		//printf("packpic1 = %s\n", xlspicname);
	}
	//second pic
	if(pic1 != NULL)
	if(strlen(pic1) != 0)
	{
		//printf("row[6] = %s\n", row[6]);
		srcPicPath = picpath + "/" + pic1;
		//printf("picfilename = %s\n", srcPicPath.c_str());

		memset(picname, 0, 128);
		memset(xlspicname, 0, 128);
		sprintf(picname, "%s", getPicName(pic1));
		sprintf(xlspicname, "%s/%s", downPicPath, picname);

		//if(MCopyFile((char *)srcPicPath.c_str(), xlspicname) == 0)
		if(CopyFile((char *)srcPicPath.c_str(), xlspicname, true))
		{
			memset(xlspicname, 0, 128);
			sprintf(xlspicname, "%s/%s", downpicDir, picname);
			sprintf(tmp[15], picname);
		}

		//printf("packpic2 = %s\n", xlspicname);
	}

	//third pic
	if(pic2 != NULL)
	if(strlen(pic2) != 0)
	{
		//printf("row[7] = %s\n", row[7]);
		srcPicPath = picpath + "/" + pic2;
		//printf("picfilename = %s\n", srcPicPath.c_str());

		memset(picname, 0, 128);
		memset(xlspicname, 0, 128);
		sprintf(picname, "%s", getPicName(pic2));
		sprintf(xlspicname, "%s/%s", downPicPath, picname);

		//if(MCopyFile((char *)srcPicPath.c_str(), xlspicname) == 0)
		if(CopyFile((char *)srcPicPath.c_str(), xlspicname, true))
		{
			memset(xlspicname, 0, 128);
			sprintf(xlspicname, "%s/%s", downpicDir, picname);
			sprintf(tmp[16], picname);
		}
	}
	xls->AddLine((char *)"new sheet", p, 17);
	count++;

}

/*删除目录及目录中的所有内容*/
BOOL DeleteFolder(char * pstrFolder)
{
    if (NULL == pstrFolder)
    {
        return FALSE;
    }

    char file[256] = {0};
    sprintf(file, "%s/*", pstrFolder);

	_finddata_t find_data;

	printf("DeleteFolder:%s\n", file);
	long find_handle = _findfirst(file,&find_data);

	if(find_handle != -1)
	{
		do
		{
			memset(file, 0, 256);
			sprintf(file, "%s/%s", pstrFolder, find_data.name);
			printf("DeleteFile:%s\n", file);
			DeleteFile(file);
		}while(_findnext(find_handle,&find_data)!= -1);
		_findclose(find_handle);
	}

    /*删除空目录*/
    return RemoveDirectory(pstrFolder);
}

void * packDataThread(void * param)
{
	pack * pPack = (pack *) param;
	string sql = "";
	rwExcel * xls = new rwExcel;
	int count = 1;
	char * xlsfilename = new char[512];
	char * downPicPath = new char[512];
	char * downpicDir = new char[512];
	SYSTEMTIME t;
	vector<string> idList;

	//初始化图片存储路径
	GetLocalTime(&t);
	memset(downPicPath, 0, 512);
	memset(downpicDir, 0, 512);
	sprintf(downPicPath, "%s/download%d_%d_%d_%d_%d_%d_%d", downloadPath.c_str(), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	printf("downPicPath = %s\n", downPicPath);
	if(access(downPicPath, F_OK) != 0)
		mkdir(downPicPath);
	sprintf(downpicDir, "download%d_%d_%d_%d_%d_%d_%d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	//初始化excel 表格 文件头
	xlsHeadInit(xls);

	vector<string> nameList;
	GetNameList(&nameList);

	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		sql = "SELECT platenum,platetype,datetime,roadname,viotype,picurl0,picurl1,picurl2,id FROM xxdb_data_viodata WHERE 1=1 AND affirm = 1";
		if(strlen(pPack->platenum) != 0)
		{
			sql += " AND platenum = '";
			sql += pPack->platenum;
			sql += "'";
		}
		if(strlen(pPack->platetype) != 0)
		{
			sql += " AND platetype = '";
			sql += pPack->platetype;
			sql += "'";
		}
		if(pPack->affirm != -1)
		{
			sql += " AND affirm = ";
			sql += itoaa(pPack->affirm);
		}
		if(strlen(pPack->stime) != 0)
		{
			sql += " AND datetime >= '";
			sql += pPack->stime;
			sql += "'";
		}
		if(strlen(pPack->etime) != 0)
		{
			sql += " AND datetime <= '";
			sql += pPack->etime;
			sql += "'";
		}
		if(pPack->download != -1)
		{
			sql += " AND download = ";
			sql += itoaa(pPack->download);
		}
		if(pPack->disposetype != -1)
		{
			sql += " AND disposetype = ";
			sql += itoaa(pPack->disposetype);
		}
		if(pPack->datafrom != -1)
		{
			sql += " AND datafrom = ";
			sql += itoaa(pPack->datafrom);
		}

		sql += " AND deviceuser = '";
		sql += (*iter).c_str();
		sql += "'";

		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			string id;

			while((row = mysql_fetch_row(result)))
			{
				packData(xls, row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7], count, downPicPath, downpicDir);
				id = row[8];
				idList.push_back(id);
			}

			mysql_free_result(result);
		}
	}

	sprintf(xlsfilename, "%s/%d_%d_%d_%d_%d_%d_%d.xls", downPicPath, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	xls->SaveAs(xlsfilename);
	printf("xls = %s\n", xlsfilename);

	delete xls;

	int first = 0;
	sql = "UPDATE xxdb_data_viodata SET download = 1 WHERE 1=1";
	for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
	{
		if(first == 0)
		{
			first++;
			sql += " AND ( id = ";
		}
		else
			sql += " OR id = ";
		sql += *iter;
	}
	if(idList.size() > 0) sql += ")";
	printf("SQL: %s\n", sql.c_str());
	xx_sql_query(m_sql, sql.c_str());

	char pathfiles[256] = {0};
	char zipfile[256] = {0};
	sprintf(pathfiles, "%s/*", downPicPath);
	sprintf(zipfile, "%s.zip", downPicPath);

	changSymbol(pathfiles);
	changSymbol(zipfile);
	printf("----------\n%s\n%s\n", pathfiles, zipfile);

	//makezip(mkzip, "D:\\download\\1\\*", "D:\\download\\1.zip");
	int r = makezip(mkzip, pathfiles, zipfile);
	printf("r = %d\n", r);

	printf("-----------End Pack------------\n");
	if(DeleteFolder(downPicPath) == FALSE)
	{
		printf("DELETE Folder ERROR!\n");
	}

	//del(mkzip);
	delete pPack;
	delete xlsfilename;
	delete downPicPath;
	delete downpicDir;

	return (void *)0;
}

int packAffirmData(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	string str = "";

	pack * pPack = new pack;
	memset(pPack, 0, sizeof(pack));

	str = xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	if(str.compare("") != 0)
		sprintf(pPack->platenum, str.c_str());

	str = xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	if(str.compare("") != 0)
		sprintf(pPack->platetype, str.c_str());

	str = xx_xml.xpath_getprop("/userdata/data/affirm", "value");
	if(str.compare("") != 0)
		pPack->affirm = atoi(str.c_str());
	else
		pPack->affirm = -1;

	str = xx_xml.xpath_getprop("/userdata/data/stime", "value");
	if(str.compare("") != 0)
		sprintf(pPack->stime, str.c_str());

	str = xx_xml.xpath_getprop("/userdata/data/etime", "value");
	if(str.compare("") != 0)
		sprintf(pPack->etime, str.c_str());

	str = xx_xml.xpath_getprop("/userdata/data/download", "value");
	if(str.compare("") != 0)
		pPack->download = atoi(str.c_str());
	else
		pPack->download = -1;

	str = xx_xml.xpath_getprop("/userdata/data/disposetype", "value");
	if(str.compare("") != 0)
		pPack->disposetype = atoi(str.c_str());
	else
		pPack->disposetype = -1;

	str = xx_xml.xpath_getprop("/userdata/data/datafrom", "value");
	if(str.compare("") != 0)
		pPack->datafrom = atoi(str.c_str());
	else
		pPack->datafrom = -1;
	string sql = "SELECT COUNT(0) FROM xxdb_data_viodata WHERE 1=1";
	if(strlen(pPack->platenum) != 0)
	{
		sql += " AND platenum = '";
		sql += pPack->platenum;
		sql += "'";
	}
	if(strlen(pPack->platetype) != 0)
	{
		sql += " AND platetype = '";
		sql += pPack->platetype;
		sql += "'";
	}
	if(pPack->affirm != -1)
	{
		sql += " AND affirm = ";
		sql += itoaa(pPack->affirm);
	}
	if(strlen(pPack->stime) != 0)
	{
		sql += " AND datetime >= '";
		sql += pPack->stime;
		sql += "'";
	}
	if(strlen(pPack->etime) != 0)
	{
		sql += " AND datetime <= '";
		sql += pPack->etime;
		sql += "'";
	}
	if(pPack->download != -1)
	{
		sql += " AND download = ";
		sql += itoaa(pPack->download);
	}
	if(pPack->disposetype != -1)
	{
		sql += " AND disposetype = ";
		sql += itoaa(pPack->disposetype);
	}
	if(pPack->datafrom != -1)
	{
		sql += " AND datafrom = ";
		sql += itoaa(pPack->datafrom);
	}

	printf("SQL : %s\n", sql.c_str());
	int count = 0;
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			count = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	printf("count = %d\n", count);
	if(count <= 0)
		strxml += new_strprop("res", "value", "2");
	else
	{
		pthread_t packTh;
		pthread_create(&packTh, NULL, packDataThread, (void *)pPack);
		pthread_detach(packTh);
		strxml += new_strprop("res", "value", "1");
	}
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUTXML : %s\n", gresXML.c_str());

	return 0;
}

int selectPackList(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	char tmp[32] = {0};
	int count = 0;
	_finddata_t find_data;

	char file[256];
	sprintf(file, "%s/%s", downloadPath.c_str(), "download*.zip");
	printf("find zip: %s\n", file);

	long find_handle = _findfirst(file,&find_data);

	if(find_handle != -1)
	{
		do
		{
			count++;
			memset(tmp, 0, 32);
			sprintf(tmp, "packname%d", count);
			strxml += new_strprop(tmp, "value", find_data.name);
		}while(_findnext(find_handle,&find_data)!= -1);
		_findclose(find_handle);
	}
	strxml += new_strprop("packcount", "value", itoaa(count));

	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	printf("OUTxml : %s\n", *outxmldata);

	return 0;
}

int deletePack(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	string str = "";
	str = downloadPath.c_str();
	str += "/";
	str += xx_xml.xpath_getprop("/userdata/data/packname", "value");

	printf("deletezip : %s\n", str.c_str());
	if(DeleteFileA(str.c_str()))
		strxml += new_strprop("res", "value", "1");
	else
		strxml += new_strprop("res", "value", "2");

	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUTxml : %s\n", *outxmldata);

	return 0;
}

int setOutDate(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	string sql = "";
	int ret = 0;

	sql = "UPDATE xxdb_info_base SET outdate = ";
	sql += xx_xml.xpath_getprop("/userdata/data/outDate", "value");

	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	strxml += new_strprop("res", "value", itoaa(ret));

	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUTxml : %s\n", *outxmldata);

	return 0;
}

int getOutDateAndCount(cxx_xml xx_xml, char ** outxmldata)
{
	string strxml = "<resultdata><item1>";
	string sql = "";
	int ret = 0, DataCount = 0;

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
			strxml += new_strprop("outdate", "value", row[0]);
			outdate = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	vector<string> nameList;
	GetNameList(&nameList);

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
	sql += "' AND disposetype = 0 AND download = 0 AND affirm = 1 AND datafrom = 0 ";
	sql += "AND (";
	int first = 0;
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
			DataCount = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	strxml += new_strprop("beforNData", "value", itoaa(DataCount));
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUTxml : %s\n", *outxmldata);

	return 0;
}
