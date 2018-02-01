/*
 * Accident_info.cpp
 *
 *  Created on: 2014-3-23
 *      Author: xiaohua.shan
 */
#include <vector>
#include "xxplug_recive_ViewScene.h"
#include "chinese_code.h"
#include <dbmepolice.h>
#include <xxxml.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <xxbase64.h>
#include "xxlibpushclient.h"

using namespace std;

extern string gresXML;
extern MYSQL * m_sql;
extern string username;
extern string picpath;
extern void * SendMsg;

char jpegpic[1024*1024*5] = {0};
int SavePicFile(const char * filename, const char * filedata)
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

	return -1;
}

char jpegbuf[1024*1024*5] = {0};
int ReadPicFile2B64(const char * filename, char * pb64buf)
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

int GetPatrolFileName(const char * picsole, const char * datetime, string &subfileurl)
{
	SYSTEMTIME sys;
	char fileurl[256] = {0};

	GetLocalTime(&sys);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "mkdir %s\\patrol\\%d\\%d\\%d", picpath.c_str(), sys.wYear, sys.wMonth, sys.wDay);
	system(fileurl);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "patrol/%d/%d/%d/%s_%s.jpg", sys.wYear, sys.wMonth, sys.wDay, picsole, datetime);

	for (char *p = fileurl; *p != '\0'; *p++)
	{
		if(*p == ':'||*p == '-'||*p == ' ')
			*p = '_';
	}

	subfileurl = fileurl;
	return 0;
}

int GetAccidentFileName(const char * picsole, const char * datetime, string &subfileurl)
{
	SYSTEMTIME sys;
	char fileurl[256] = {0};

	GetLocalTime(&sys);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "mkdir %s\\accident\\%d\\%d\\%d", picpath.c_str(), sys.wYear, sys.wMonth, sys.wDay);
	system(fileurl);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "accident\\%d\\%d\\%d\\%s_%s.jpg", sys.wYear, sys.wMonth, sys.wDay, picsole, datetime);

	for (char *p = fileurl; *p != '\0'; *p++)
	{
		if(*p == ':'||*p == '-'||*p == ' ')
			*p = '_';
	}

	subfileurl = fileurl;
	return 0;
}

int insertAccident_process(cxx_xml xx_xml,char ** outxmldata)
{
	string	sql = "INSERT INTO xxdb_data_accident_progress (sole,station,DATETIME,memo,userid,devid) VALUES ('";
	sql += 	xx_xml.xpath_getprop("/userdata/data/accidentid", "value");
	sql += "',";
	sql += 	xx_xml.xpath_getprop("/userdata/data/progress", "value");
	sql += ",'";
	sql += 	xx_xml.xpath_getprop("/userdata/data/datetime", "value");
	sql += "','";
	sql += 	xx_xml.xpath_getprop("/userdata/data/memo", "value");
	sql += "','";
	sql += 	username;
	sql += "','";
	sql += 	xx_xml.xpath_getprop("/userdata/head/dev", "value");
    sql += "');";

    int iresut = xx_sql_query(m_sql, sql.c_str());
	sql = "update xxdb_data_accident set progress =";
	sql += xx_xml.xpath_getprop("/userdata/data/progress", "value");
	sql += " where sole = ";
	sql += 	xx_xml.xpath_getprop("/userdata/data/accidentid", "value");
	sql += " ;";

	iresut += xx_sql_query(m_sql, sql.c_str());

	string strtmp = new_strprop("res", "value", itoaa(iresut));
	string strtmp1 = setResItem(1, strtmp);
	string strtmp11 = "<resultdata>";
	strtmp11 += strtmp1;
	strtmp11 += "</resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strtmp11, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

int GetUsername(string realname, string &username)
{
	string sql = "";

	sql = "SELECT username FROM xxdb_info_deviceuser WHERE name = '";
	sql += realname;
	sql += "'";

	int ret = xx_sql_query(m_sql, sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL) username = row[0];
		}

		mysql_free_result(result);
	}

	return ret;
}

int GetUsernameList(char * realnameList, vector<string> &usernameList)
{
	char * p = NULL, * q = NULL;
	char tmp[20] = {0};
	string realname, username;

	p = realnameList;
	q = realnameList;
	while(1)
	{
		memset(tmp, 0, 10);
		if((q = strchr(p, ',')) != NULL)
		{
			memcpy(tmp, p, q - p);
			realname = tmp;
			GetUsername(realname, username);
			usernameList.push_back(username);
		}
		else
		{
			strcpy(tmp, p);
			realname = tmp;
			GetUsername(realname, username);
			usernameList.push_back(username);
			break;
		}
		p = q+1;
	}

	return 0;
}

int insertAccident(cxx_xml xx_xml, char ** outxmldata)
{
	string realnameList = "";
	string msg = "";
	time_t t;
	time(&t);

	string sql = "SELECT userList,text FROM xxdb_data_sndinfoallocation WHERE 1=1";
	sql += " AND deadstart <= ";
	sql += xx_xml.xpath_getprop("/userdata/data/death", "value");
	sql += " AND deadend >= ";
	sql += xx_xml.xpath_getprop("/userdata/data/death", "value");
	printf("SQL : %s\n", sql.c_str());
	int iresut = xx_sql_query(m_sql, sql.c_str());
	if(iresut==0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		string tmpprop="";

		if((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL) realnameList = row[0];
			if(row[1] != NULL) msg = row[1];
		}

		mysql_free_result(result);
	}

	if(realnameList.compare("") != 0)
	{
		vector<string> nameList;
		GetUsernameList((char *)realnameList.c_str(), nameList);

		string msgxml = "";
		string name = "";
		string str = "";

		for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
		{
			str = "<resultdata><item1>";
			str += new_strprop("msgtype", "value", "1");
			str += "<text>";
			str += msg;
			str += "</text>";
			str += "</item1></resultdata>";
			setResXML("0009", 1, 1, 0, str, msgxml);

			name = "a/";
			name += (*iter);

			printf("Accident Msg : name : %s msg : %s\n", name.c_str(), msgxml.c_str());
			publish(SendMsg, (char *)name.c_str(), (char *)msgxml.c_str(), false);
		}
	}

	sql ="INSERT INTO xxdb_data_accident\
				(datetime,roadname,KM,accidenttype,carnum,death\
				,hurt,progress,memo,deviceuser,devid,flag,sole) VALUES ";
	sql += "('";
	sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");//data->data.datatime;
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/roadname", "value");//data->data.roadname;
	sql += "',";
	sql += xx_xml.xpath_getprop("/userdata/data/KM", "value");//itoaa(data->data.KM);
	sql += ",";
	sql += "0";
	sql += ",";
	sql += xx_xml.xpath_getprop("/userdata/data/carnum", "value");//sql += itoaa(data->data.carnum);
	sql += ",";
	sql += xx_xml.xpath_getprop("/userdata/data/death", "value");//itoaa(data->data.death);
	sql += ",";
	sql += xx_xml.xpath_getprop("/userdata/data/wound", "value");//itoaa(data->data.hurt);
	sql += ",";
	sql += xx_xml.xpath_getprop("/userdata/data/progress", "value");//itoaa(data->data.progress);
	sql += ",'";
	sql += xx_xml.xpath_getprop("/userdata/data/memo", "value");//data->data.memo;
	sql += "','";
	//sql += xx_xml.xpath_getprop("/userdata/head/user", "value");//data->data.deviceuser;
	sql += username;
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/head/dev", "value");//itoaa(data->data.devid);
	sql += "',";
	sql += "0";//itoaa(data->data.flag);
	sql += ",'";
	string sole = xx_xml.xpath_getprop("/userdata/data/picsole", "value");
	sql += sole;//itoaa(data->data.sole);
	sql += "')";

	string carnum  = xx_xml.xpath_getprop("/userdata/data/carnum", "value");
	int ncarnum = atoi(carnum.c_str());

	iresut = xx_sql_query(m_sql, sql.c_str());
	char tmpstr[128];
	for(int i=1;i<(ncarnum+1);i++)
	{
		sprintf(tmpstr,"/userdata/data/carinfo%d/platenum",i);
		sql = "INSERT INTO xxdb_data_accidentcar \
				(platenum,platetype,accidentid)\
				VALUES ('";
		sql += xx_xml.xpath_getprop(tmpstr,"value");
		sql += "','";
		sprintf(tmpstr,"/userdata/data/carinfo%d/platetype",i);
		sql += xx_xml.xpath_getprop(tmpstr,"value");
		sql += "',";
		sql += sole;
		sql += ");";
		iresut += xx_sql_query(m_sql, sql.c_str());
	}

	string strtmp = new_strprop("res", "value", itoaa(iresut));
	string strtmp1 = setResItem(1, strtmp);
	string strtmp11 = "<resultdata>";
	strtmp11 += strtmp1;
	strtmp11 += "</resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strtmp11, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf(*outxmldata);
	return iresut;
}

int updateAccident(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string str = xx_xml.xpath_getprop("/userdata/data/type", "value");

	sql = "UPDATE xxdb_data_accident SET ";
	sql.append("roadname = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/roadname","value"));
	sql.append("',");
	sql.append("KM = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/KM","value"));
	sql.append("',");
	sql.append("datetime = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/datetime","value"));
	sql.append("',");
	sql.append("carnum = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/carnum","value"));
	sql.append("',");
	sql.append("death = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/death","value"));
	sql.append("',");
	sql.append("hurt = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/wound","value"));
	sql.append("',");
	sql.append("progress = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/progress","value"));
	sql.append("',");
	sql.append("memo = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/memo","value"));
	sql.append("'");
	sql.append(" WHERE sole = ");
	sql.append(xx_xml.xpath_getprop("/userdata/data/picsole","value"));

	int iresut = xx_sql_query(m_sql, sql.c_str());

	if (str.compare("0005") != 0)
	{
		if(iresut == 0) iresut = 1;
		else iresut = 2;
	}

	string strtmp = new_strprop("res", "value", itoaa(iresut));
	string strtmp1 = setResItem(1, strtmp);
	string strtmp11 = "<resultdata>";
	strtmp11 += strtmp1;
	strtmp11 += "</resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strtmp11, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return iresut;
}

int deleteAccident(cxx_xml xx_xml, char ** outxmldata)
{
	string sql("DELETE FROM xxdb_data_accident WHERE sole = '");
	sql.append(xx_xml.xpath_getprop("/userdata/data/picsole","value"));
	sql.append("'");
	int iresut = xx_sql_query(m_sql, sql.c_str());

	sql = "DELETE FROM xxdb_data_accidentcar WHERE accidentid = ";
	sql.append(xx_xml.xpath_getprop("/userdata/data/picsole","value"));
	iresut = xx_sql_query(m_sql, sql.c_str());

	sql = "DELETE FROM xxdb_data_accident_progress WHERE sole = ";
	sql.append(xx_xml.xpath_getprop("/userdata/data/picsole","value"));
	iresut = xx_sql_query(m_sql, sql.c_str());

	if(iresut == 0) iresut = 1;
	else iresut = 2;
	string strtmp = new_strprop("res", "value", itoaa(iresut));
	string strtmp1 = setResItem(1, strtmp);
	string strtmp11 = "<resultdata>";
	strtmp11 += strtmp1;
	strtmp11 += "</resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strtmp11, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return iresut;
}

char b64buf[1024*1024*5];
int selectAccidentpic(cxx_xml xx_xml, char ** outxmldata)
{
	string tmpitem = "";
	int isum_count = 0;
	int icount = 0;

	string picindex = xx_xml.xpath_getprop("/userdata/data/picindex", "value");

	string	sql = "select ownerid,pictype,picurl from xxdb_data_accidentpic\
					where ownerid = ";
	sql += "'";
	sql += xx_xml.xpath_getprop("/userdata/data/picsole", "value");
	sql += "' and pictype = ";
	sql += xx_xml.xpath_getprop("/userdata/data/pictype", "value");

	int iresut = xx_sql_query(m_sql,sql.c_str());
	if(iresut==0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		string tmpprop="";

		while((row = mysql_fetch_row(result)))
		{
			isum_count++;
		}

		mysql_free_result(result);
	}

	sql +=	" limit ";
	sql += xx_xml.xpath_getprop("/userdata/data/picindex", "value");
	sql += ",1";

	iresut = xx_sql_query(m_sql,sql.c_str());
	if(iresut==0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		string tmpprop="";

		if ((row = mysql_fetch_row(result)))
		{
			icount++;
			if(row[2] != 0)
			{
				tmpprop = new_strprop("picsole", "value", row[0]);
				tmpprop += new_strprop("pictype", "value", row[1]);
				tmpprop += new_strprop("picindex", "value", picindex.c_str());

				memset(b64buf,0,1024*1024*5);
				int size = ReadPicFile2B64(row[2], b64buf);

				tmpprop += new_strprop("picsize", "value", itoaa(size));
				tmpprop += "<picdata>";
				if(size != 0) tmpprop += b64buf;
				tmpprop += "</picdata>";

				tmpitem += setResItem(1, tmpprop);
			}
		}
		mysql_free_result(result);
	}

	string strxml = "<resultdata>";
	strxml += tmpitem;
	strxml += "</resultdata>";

	setResXML(TYPESTR, icount,isum_count, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	
	return iresut;
}

int insertAccidentpic(cxx_xml xx_xml, char ** outxmldata)
{
	char tmp[512] = {0};
	string strpic = "";
	string picdir = "";
	string strxml = "";
	string sql = "";
	SYSTEMTIME t;
	int ret = 0;

	GetLocalTime(&t);
	sprintf(tmp, "%d_%d_%d_%d_%d_%d_%d",t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	GetPatrolFileName(xx_xml.xpath_getprop("/userdata/data/picsole", "value"), tmp, picdir);
	xx_xml.xpath_gettext("/userdata/data/pic/picdata", strpic);
	SavePicFile(picdir.c_str(), strpic.c_str());

	sql  ="INSERT INTO xxdb_data_accidentpic(picurl,ownerid,infotype,caption,pictype) VALUES ";
	sql += "('";
	sql += picdir;
	sql += "',";
	sql += xx_xml.xpath_getprop("/userdata/data/picsole", "value");
	sql += ",0,'',";
	sql += xx_xml.xpath_getprop("/userdata/data/pictype", "value");
	sql += ")";

	//printf("SQL : %s\n", sql.c_str());

	ret = xx_sql_query(m_sql, sql.c_str());
	strxml = "<resultdata><item1>";
	strxml += new_strprop("res", "value", itoaa(ret));
	strxml += "</item1></resultdata>";

	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	int piccount = 0;
	sql = "SELECT COUNT(1) FROM xxdb_data_accidentpic WHERE ownerid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/picsole", "value");
	sql += "' GROUP BY ownerid";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			piccount = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	if(atoi(xx_xml.xpath_getprop("/userdata/data/pictype", "value")) == 0)
	{
		sql = "UPDATE xxdb_data_patorl SET picnum = ";
		sql += itoaa(piccount);
		sql += " WHERE sole = '";
		sql += xx_xml.xpath_getprop("/userdata/data/picsole", "value");
		sql += "'";
	}
	else if(atoi(xx_xml.xpath_getprop("/userdata/data/pictype", "value")) == 1)
	{
		sql = "UPDATE xxdb_data_accident SET picnum = ";
		sql += itoaa(piccount);
		sql += " WHERE sole = '";
		sql += xx_xml.xpath_getprop("/userdata/data/picsole", "value");
		sql += "'";
	}

	ret = xx_sql_query(m_sql, sql.c_str());

	return 0;
}

int GetGroupNameList(vector<string> * nameList)
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

int selectAccident(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string str = "";
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index","value"));
	int sumcount = 0, count = 0;
	string resxml = "<resultdata>";
	vector<string> nameList;

	GetGroupNameList(&nameList);
	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		sql = "SELECT a.roadname,a.KM,a.datetime,a.carnum,a.death,a.hurt,a.progress,a.memo,a.sole,a.picnum,b.platenum,b.platetype FROM xxdb_data_accident a,xxdb_data_accidentcar b WHERE b.accidentid = a.sole";

		xx_xml.xpath_getprop("/userdata/data/roadname", "value", str);
		if (str.compare("")) {
			sql += " and roadname = '";
			sql += str;
			sql	+= "' ";
		}

		xx_xml.xpath_getprop("/userdata/data/KM","value",str);
		if(str.compare(""))
		{
			sql += " and KM = ";
			sql += str;
		}

		xx_xml.xpath_getprop("/userdata/data/starttime","value",str);
		if(str.compare(""))
		{
			sql += " and DATETIME >= '";
			sql += str;
			sql += "' ";
		}

		xx_xml.xpath_getprop("/userdata/data/endtime","value",str);
		if(str.compare(""))
		{
			sql += " and DATETIME <= '";
			sql += str;
			sql += "' ";
		}

		xx_xml.xpath_getprop("/userdata/data/death","value",str);
		if(str.compare(""))
		{
			sql += " and death = ";
			sql += str;
		}

		xx_xml.xpath_getprop("/userdata/data/wound","value",str);
		if(str.compare(""))
		{
			sql += " and hurt = ";
			sql += str;
		}

		xx_xml.xpath_getprop("/userdata/data/carnum","value",str);
		if(str.compare(""))
		{
			sql += " and carnum = ";
			sql += str;
		}

		xx_xml.xpath_getprop("/userdata/data/progress","value",str);
		if(str.compare(""))
		{
			sql += " and progress = ";
			sql += str;
		}

		xx_xml.xpath_getprop("/userdata/data/name", "value", str);
		if(str.compare(""))
		{
			sql.append(" AND deviceuser = '");
			sql += str;
			sql += "'";
		}

		sql += " AND deviceuser = '";
		sql += (*iter).c_str();
		sql += "'";

		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql,sql.c_str())==0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			int old_sole = 0;
			int car_count = 0;
			int add_item_end = 0;
			while((row = mysql_fetch_row(result)))
			{
				if(old_sole != atoi(row[8]))
				{
					old_sole = atoi(row[8]);

					sumcount++;
					if((count - firstIndex) >= 10) continue;
					if(((count - firstIndex) >= 1)&&(add_item_end == 1))
					{
						//printf("----##---</item%d>-----##-----\n", count-firstIndex);
						resxml += "</item";
						resxml += itoaa(count-firstIndex);
						resxml += ">";
						add_item_end = 0;
					}

					count++;
					if(count <= firstIndex) continue;

					car_count = 1;

					add_item_end = 1;
					resxml += "<item";
					resxml += itoaa(count-firstIndex);
					resxml += ">";

					resxml += new_strprop("roadname","value",row[0]);
					resxml += new_strprop("KM","value",row[1]);
					resxml += new_strprop("datetime","value",row[2]);
					resxml += new_strprop("carnum","value",row[3]);
					resxml += new_strprop("death","value",row[4]);
					resxml += new_strprop("wound","value",row[5]);
					resxml += new_strprop("progress","value",row[6]);
					resxml += new_strprop("memo","value",row[7]);
					resxml += new_strprop("picsole","value",row[8]);
					resxml += new_strprop("picnum","value",row[9]);

					resxml += "<carinfo";
					resxml += itoaa(car_count);
					resxml += ">";
					resxml += new_strprop("platenum","value",row[10]);
					resxml += new_strprop("platetype","value",row[11]);
					resxml += "</carinfo";
					resxml += itoaa(car_count);
					resxml += ">";
				}
				else
				{
					if((count - firstIndex) > 0)
					{
						car_count++;
						resxml += "<carinfo";
						resxml += itoaa(car_count);
						resxml += ">";
						resxml += new_strprop("platenum","value",row[10]);
						resxml += new_strprop("platetype","value",row[11]);
						resxml += "</carinfo";
						resxml += itoaa(car_count);
						resxml += ">";
					}
				}
			}
			mysql_free_result(result);

			if(((count - firstIndex) >= 1)&&(add_item_end == 1))
			{
				//printf("count = %d\n", count);
				//printf("---------</item%d>-------------\n", count-firstIndex);
				resxml += "</item";
				resxml += itoaa(count-firstIndex);
				resxml += ">";
			}
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count - firstIndex, sumcount, firstIndex, resxml, gresXML);

	*outxmldata = (char *) gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int selectAccidentAll(cxx_xml xx_xml, char ** outxmldata)
{
	string sql = "";
	string str = "";
	int sumcount = 0;
	string resxml = "<resultdata>";
	vector<string> nameList;

	GetGroupNameList(&nameList);
	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		sql = "SELECT a.roadname,a.KM,a.datetime,a.carnum,a.death,a.hurt,a.progress,a.memo,a.sole,a.picnum,b.platenum,b.platetype FROM xxdb_data_accident a,xxdb_data_accidentcar b WHERE b.accidentid = a.sole";

		xx_xml.xpath_getprop("/userdata/data/starttime","value",str);
		if(str.compare(""))
		{
			sql += " and DATETIME >= '";
			sql += str;
			sql += "' ";
		}

		xx_xml.xpath_getprop("/userdata/data/endtime","value",str);
		if(str.compare(""))
		{
			sql += " and DATETIME <= '";
			sql += str;
			sql += "' ";
		}

		xx_xml.xpath_getprop("/userdata/data/name", "value", str);
		if(str.compare(""))
		{
			sql.append(" AND deviceuser = '");
			sql += str;
			sql += "'";
		}

		sql += " AND deviceuser = '";
		sql += (*iter).c_str();
		sql += "'";

		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql,sql.c_str())==0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			int old_sole = 0;
			int car_count = 0;
			int add_item_end = 0;
			while((row = mysql_fetch_row(result)))
			{
				if(old_sole != atoi(row[8]))
				{
					old_sole = atoi(row[8]);

					if(add_item_end == 1)
					{
						//printf("----##---</item%d>-----##-----\n", count-firstIndex);
						resxml += "</item";
						resxml += itoaa(sumcount);
						resxml += ">";
						add_item_end = 0;
					}

					sumcount++;
					car_count = 1;

					add_item_end = 1;
					resxml += "<item";
					resxml += itoaa(sumcount);
					resxml += ">";

					resxml += new_strprop("roadname","value",row[0]);
					resxml += new_strprop("KM","value",row[1]);
					resxml += new_strprop("datetime","value",row[2]);
					resxml += new_strprop("carnum","value",row[3]);
					resxml += new_strprop("death","value",row[4]);
					resxml += new_strprop("wound","value",row[5]);
					resxml += new_strprop("progress","value",row[6]);
					resxml += new_strprop("memo","value",row[7]);
					resxml += new_strprop("picsole","value",row[8]);
					resxml += new_strprop("picnum","value",row[9]);

					resxml += "<carinfo";
					resxml += itoaa(car_count);
					resxml += ">";
					resxml += new_strprop("platenum","value",row[10]);
					resxml += new_strprop("platetype","value",row[11]);
					resxml += "</carinfo";
					resxml += itoaa(car_count);
					resxml += ">";
				}
				else
				{
					car_count++;
					resxml += "<carinfo";
					resxml += itoaa(car_count);
					resxml += ">";
					resxml += new_strprop("platenum","value",row[10]);
					resxml += new_strprop("platetype","value",row[11]);
					resxml += "</carinfo";
					resxml += itoaa(car_count);
					resxml += ">";
				}
			}
			mysql_free_result(result);

			if(add_item_end == 1)
			{
				//printf("count = %d\n", count);
				//printf("---------</item%d>-------------\n", count-firstIndex);
				resxml += "</item";
				resxml += itoaa(sumcount);
				resxml += ">";
			}
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, sumcount, sumcount, 0, resxml, gresXML);

	*outxmldata = (char *) gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int selectAccidentProgress(cxx_xml xx_xml, char ** outxmldata)
{
	string strtmp = "<resultdata>";
	int count = 0;
	string sql = "SELECT sole,station,datetime,memo FROM xxdb_data_accident_progress WHERE 1=1 ";
	sql += "AND sole = '";
	sql += xx_xml.xpath_getprop("/userdata/data/accidentid","value");
	sql += "'";

	int ret = xx_sql_query(m_sql,sql.c_str());
	if(ret == 0)
	{
		MYSQL_RES *results = mysql_store_result(m_sql);
		MYSQL_ROW rows;
		string tmp = "";

		while((rows = mysql_fetch_row(results)))
		{
			count++;
			tmp = new_strprop("accidentid", "value", rows[0]);
			tmp += new_strprop("progress", "value", rows[1]);
			tmp += new_strprop("datetime", "value", rows[2]);
			tmp += new_strprop("memo", "value", rows[3]);

			strtmp += setResItem(count, tmp);
		}
		mysql_free_result(results);
	}

	strtmp += "</resultdata>";

	setResXML(TYPESTR, count, count, 0, strtmp, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return ret;
}

int tidyAccident(cxx_xml xx_xml, char ** outxmldata)
{
	int ret = 0;
	vector<string> name;
	string strtmp1;
	string strtmp11;
	int sendcount = 0;
	unsigned int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));

	string sql = "";
	string strtmp = xx_xml.xpath_getprop("/userdata/data/group", "value");
	ret = GetNameList(strtmp, &name);

	if(ret != 0)
	{
		vector<string>::iterator iter = name.begin();
		for(unsigned int i = 0; i < name.size(); i++)
		{
			int count = 0;
			sql = "SELECT * FROM xxdb_data_accident WHERE 1=1";
			sql += " AND `user` = '";
			sql += iter->c_str();
			sql += "'";
			sql += " AND `datetime` >= '";
			sql += xx_xml.xpath_getprop("/userdata/data/stime", "value");
			sql += "'";
			sql += " AND `datetime` <= '";
			sql += xx_xml.xpath_getprop("/userdata/data/etime", "value");
			sql += "'";

			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				while ((row = mysql_fetch_row(result)))
				{
					count++;
				}
				mysql_free_result(result);
			}

			if((sendcount < 10)&&(firstindex <= i))
			{
				sendcount++;
				strtmp = new_strprop("username", "value", iter->c_str());
				strtmp += new_strprop("count", "value", itoaa(count));
				strtmp1 += setResItem(sendcount, strtmp);
			}
			iter++;
		}
	}

	strtmp11 = "<resultdata>";
	strtmp11 += strtmp1;
	strtmp11 += "</resultdata>";
	setResXML(TYPESTR, sendcount, name.size(), firstindex, strtmp11, gresXML);

	return ret;
}
