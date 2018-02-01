#include "xxplug_recive_SpecialVio.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <xxxml.h>
#include <xxbase64.h>
#include <time.h>
#include "xxlibpushclient.h"

using namespace std;

extern MYSQL * m_sql;
extern string gresXML;
extern string username;
extern string picpath;
extern void * SendMsg;

int insertExamine(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string str = "";
	string resxml = "<resultdata><item1>";
	int res = 0;

	sql = "INSERT INTO xxdb_data_specialvio (vioid,vioaddr,viotime,viotype,platenum,platetype,rebuktime,rebuktype1,rebuktype2,rebuktype3,rebuktype4,rebukadr,picnum,isExamine,isPass,examin1,examin2,examin3,examin4,soleid,username ) VALUES ('";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/vioaddr", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/viotime", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/viotype", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/vioidplate", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/vioidplatetype", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/rebuktime", "value");
	sql += "','";

	str = xx_xml.xpath_getprop("/userdata/data/rebuktype1", "value");
	if(str.compare("") != 0)
		sql += "1";
	else
		sql += "0";
	sql += "','";

	str = xx_xml.xpath_getprop("/userdata/data/rebuktype2", "value");
	if(str.compare("") != 0)
		sql += "1";
	else
		sql += "0";
	sql += "','";

	str = xx_xml.xpath_getprop("/userdata/data/rebuktype3", "value");
	if(str.compare("") != 0)
		sql += "1";
	else
		sql += "0";
	sql += "','";

	str = xx_xml.xpath_getprop("/userdata/data/rebuktype4", "value");
	if(str.compare("") != 0)
		sql += str;
	else
		sql += "0";

	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/rebukadr", "value");
	sql += "',0,0,0,'','','','','";
	sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
	sql += "','";
	sql += username;
	sql += "')";

	printf("SQL : %s\n", sql.c_str());
	res = xx_sql_query(m_sql, sql.c_str());

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf("resXML = %s\n", *outxmldata);

	int autoEamine = 0;

	sql = "SELECT autoEamine FROM xxdb_info_usergroup WHERE `group` = (SELECT `group` FROM xxdb_info_usergroup WHERE id = (SELECT `group` FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'))";
	//printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			autoEamine = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	printf("aotoEamine = %d\n", autoEamine);

	if(autoEamine == 0)
	{
		string msgxml = "";
		string name = "a/";
		name += username;

		str = "<resultdata><item1>";
		str += new_strprop("msgtype", "value", "3");
		str += "<text>";
		str += new_strprop("soleid", "value", xx_xml.xpath_getprop("/userdata/data/soleid", "value"));
		str += new_strprop("rebukres", "value", "1");
		str += new_strprop("rebuksug1", "value", "");
		str += new_strprop("rebuksug2", "value", "");
		str += new_strprop("rebuksug3", "value", "");
		str += new_strprop("rebuksug4", "value", "");
		str += "</text>";
		str += "</item1></resultdata>";
		setResXML("0009", 1, 1, 0, str, msgxml);

		publish(SendMsg, (char *)name.c_str(), (char *)msgxml.c_str(), false);

		sql = "UPDATE xxdb_data_specialvio SET isExamine = 1,isPass = 1 WHERE soleid = '";
		sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
		sql += "'";
		xx_sql_query(m_sql, sql.c_str());
	}

	return 0;
}

int webGetAuto(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	string datafilter = "";
	int count = 0;

	sql = "SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'";
	printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			datafilter = row[0];
		}
	}

	sql = "SELECT `group`,autoEamine FROM xxdb_info_usergroup WHERE father != '0' AND datafilter LIKE '";
	sql += datafilter;
	sql += "%'";
	printf("SQL:%s\n",sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			count++;
			resxml += "<item";
			resxml += itoaa(count);
			resxml += ">";
			resxml += new_strprop("name", "value", row[0]);
			resxml += new_strprop("AutoExamine", "value", row[1]);
			resxml += "</item";
			resxml += itoaa(count);
			resxml += ">";
		}
		mysql_free_result(result);
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int webSetAuto(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string str = "";
	string resxml = "<resultdata><item1>";
	int res = 0, count = 0;
	char tmp[256];

	count = atoi(xx_xml.xpath_getprop("/userdata/data/count", "value"));

	for(int i = 1; i <= count; i++)
	{
		memset(tmp, 0, 256);
		sprintf(tmp, "/userdata/data/form%d/", i);

		sql = "UPDATE xxdb_info_usergroup SET autoEamine = ";
		str = tmp;
		str += "AutoExamine";
		sql += xx_xml.xpath_getprop(str.c_str(), "value");
		sql += " WHERE `group` = '";
		str = tmp;
		str += "name";
		sql += xx_xml.xpath_getprop(str.c_str(), "value");
		sql += "'";
		//printf("SQL : %s\n", sql.c_str());
		res = xx_sql_query(m_sql, sql.c_str());
	}

	if(res == 0) res = 1;
	else res = 2;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf("resXML = %s\n", *outxmldata);

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

int webGetExamine(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string str = "";
	string resxml = "<resultdata>";
	int count = 0, sumcount = 0, pcount = 0;
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	vector<string> nameList;

	GetNameList(&nameList);

	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		sql = "SELECT COUNT(0) FROM xxdb_data_specialvio WHERE 1=1";
		str = xx_xml.xpath_getprop("/userdata/data/stime", "value");
		if(str.compare("") != 0)
		{
			sql += " AND rebuktime >= '";
			sql += str;
			sql += "'";
		}

		str = xx_xml.xpath_getprop("/userdata/data/etime", "value");
		if(str.compare("") != 0)
		{
			sql += " AND rebuktime <= '";
			sql += str;
			sql += "'";
		}

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

		str = xx_xml.xpath_getprop("/userdata/data/IsExamine", "value");
		if(str.compare("") != 0)
		{
			sql += " AND isExamine = ";
			sql += str;
		}

		str = xx_xml.xpath_getprop("/userdata/data/IsPass", "value");
		if(str.compare("") != 0)
		{
			sql += " AND isPass = ";
			sql += str;
		}

		sql += " AND username = '";
		sql += (*iter).c_str();
		sql += "'";

		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
			{
				pcount = atoi(row[0]);
				sumcount += pcount;
			}
			mysql_free_result(result);
		}

		printf("sumcount = %d pcount = %d count = %d\n", sumcount, pcount, count);
		if((pcount > 0) && ((count - firstIndex) < 10))
		{

			sql = "SELECT platenum,platetype,vioaddr,viotime,viotype,vioid,soleid,rebuktime,rebukadr,rebuktype1,rebuktype2,rebuktype3,rebuktype4,picnum,isExamine,isPass,examin1,examin2,examin3,examin4 FROM xxdb_data_specialvio WHERE 1=1";
			str = xx_xml.xpath_getprop("/userdata/data/stime", "value");
			if(str.compare("") != 0)
			{
				sql += " AND rebuktime >= '";
				sql += str;
				sql += "'";
			}

			str = xx_xml.xpath_getprop("/userdata/data/etime", "value");
			if(str.compare("") != 0)
			{
				sql += " AND rebuktime <= '";
				sql += str;
				sql += "'";
			}

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

			str = xx_xml.xpath_getprop("/userdata/data/IsExamine", "value");
			if(str.compare("") != 0)
			{
				sql += " AND isExamine = ";
				sql += str;
			}

			str = xx_xml.xpath_getprop("/userdata/data/IsPass", "value");
			if(str.compare("") != 0)
			{
				sql += " AND isPass = ";
				sql += str;
			}

			sql += " AND username = '";
			sql += (*iter).c_str();
			sql += "'";

			printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while((row = mysql_fetch_row(result)))
				{
					count++;

					if((count - firstIndex) > 0)
					{
						resxml += "<item";
						resxml += itoaa(count - firstIndex);
						resxml += ">";
						resxml += new_strprop("platenum", "value", row[0]);
						resxml += new_strprop("platetype", "value", row[1]);
						resxml += new_strprop("vioplace", "value", row[2]);
						resxml += new_strprop("viodate", "value", row[3]);
						resxml += new_strprop("viotype", "value", row[4]);
						resxml += new_strprop("vioid", "value", row[5]);
						resxml += new_strprop("soleid", "value", row[6]);
						resxml += new_strprop("complaintime", "value", row[7]);
						resxml += new_strprop("complainadr", "value", row[8]);
						resxml += new_strprop("complain1", "value", row[9]);
						resxml += new_strprop("complain2", "value", row[10]);
						resxml += new_strprop("complain3", "value", row[11]);
						resxml += new_strprop("complain4", "value", row[12]);
						resxml += new_strprop("picnum", "value", row[13]);
						resxml += new_strprop("IsExamine", "value", row[14]);
						resxml += new_strprop("IsPass", "value", row[15]);
						resxml += new_strprop("examin1", "value", row[16]);
						resxml += new_strprop("examin2", "value", row[17]);
						resxml += new_strprop("examin3", "value", row[18]);
						resxml += new_strprop("examin4", "value", row[19]);
						resxml += "</item";
						resxml += itoaa(count - firstIndex);
						resxml += ">";
					}

					if((count - firstIndex) >= 10) break;
				}
				mysql_free_result(result);
			}
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count - firstIndex, sumcount, firstIndex, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int webSendResult(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string str = "";
	string resxml = "<resultdata><item1>";
	int res = 0;

	sql = "UPDATE xxdb_data_specialvio SET isExamine = 1";
	sql += ", isPass = ";
	sql += xx_xml.xpath_getprop("/userdata/data/IsPass", "value");
	sql += ", examin1 = '";
	sql += xx_xml.xpath_getprop("/userdata/data/examin1", "value");
	sql += "', examin2 = '";
	sql += xx_xml.xpath_getprop("/userdata/data/examin2", "value");
	sql += "', examin3 = '";
	sql += xx_xml.xpath_getprop("/userdata/data/examin3", "value");
	sql += "', examin4 = '";
	sql += xx_xml.xpath_getprop("/userdata/data/examin4", "value");
	sql += "' WHERE soleid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
	sql += "'";

	//printf("SQL : %s\n", sql.c_str());
	res = xx_sql_query(m_sql, sql.c_str());

	if(res == 0) res = 1;
	else res = 2;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf("resXML = %s\n", *outxmldata);

	string msgxml = "";
	string name = "a/";
	name += username;

	str = "<resultdata><item1>";
	str += new_strprop("msgtype", "value", "3");
	str += "<text>";
	str += new_strprop("soleid", "value", xx_xml.xpath_getprop("/userdata/data/soleid", "value"));
	str += new_strprop("rebukres", "value", "1");
	str += new_strprop("rebuksug1", "value", "");
	str += new_strprop("rebuksug2", "value", "");
	str += new_strprop("rebuksug3", "value", "");
	str += new_strprop("rebuksug4", "value", "");
	str += "</text>";
	str += "</item1></resultdata>";
	setResXML("0009", 1, 1, 0, str, msgxml);

	publish(SendMsg, (char *)name.c_str(), (char *)msgxml.c_str(), false);

	sql = "UPDATE xxdb_data_specialvio SET isExamine = 1 WHERE soleid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
	sql += "'";
	xx_sql_query(m_sql, sql.c_str());

	return 0;
}

int getfilename(char * soleid, string &filename)
{
	char tmp[512] = {0};
	char path[512] = {0};
	SYSTEMTIME t;
	GetLocalTime(&t);

	memset(path, 0, 512);
	sprintf(path, "mkdir %s\\specialviopic\\%d\\%d\\%d", picpath.c_str(), t.wYear, t.wMonth, t.wDay);
	system(path);

	sprintf(tmp, "SpecialVioPic_%s_%d_%d_%d_%d_%d_%d_%d.jpg", soleid, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);

	memset(path, 0, 512);
	sprintf(path, "specialviopic/%d/%d/%d/%s", t.wYear, t.wMonth, t.wDay, tmp);

	filename = path;

	return 0;
}


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

int uploadPic(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	string filename = "";
	string picdata = "";
	int picnum = 0;
	int res = 0;

	xx_xml.xpath_gettext("/userdata/data/picdata", picdata);
	getfilename((char *)xx_xml.xpath_getprop("/userdata/data/soleid", "value"), filename);
	SavePicFile(filename.c_str(), picdata.c_str());

	sql = "INSERT INTO xxdb_data_specialviopic (soleid,picpath) VALUES ('";
	sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
	sql += "','";
	sql += filename;
	sql += "')";
	//printf("SQL : %s\n", sql.c_str());
	res = xx_sql_query(m_sql, sql.c_str());

	sql = "SELECT picnum FROM xxdb_data_specialvio WHERE soleid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
	sql += "'";
	//printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			picnum = atoi(row[0])+1;
		}
		mysql_free_result(result);
	}

	sql = "UPDATE xxdb_data_specialvio SET picnum = ";
	sql += itoaa(picnum);
	sql += " WHERE soleid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
	sql += "'";
	//printf("SQL : %s\n", sql.c_str());
	xx_sql_query(m_sql, sql.c_str());

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf("resXML = %s\n", *outxmldata);

	return 0;
}

char picinbuf[1024*1024*10];
char b64outbuf[1024*1024*10];
int getPic(char * path, string &b64data, int &picsize)
{
	memset(picinbuf, 0, 1024*1024*10);
	memset(b64outbuf, 0, 1024*1024*10);

	char filepath[512] = {0};
	sprintf(filepath, "%s/%s", picpath.c_str(), path);

	FILE * f = fopen(filepath, "r+");
	if(f)
	{
		fseek(f, 0, SEEK_END);
		picsize = ftell(f);
		fseek(f, 0, SEEK_SET);
		fread(picinbuf, 1, picsize, f);
		b64_encode_string((const unsigned char *)picinbuf, picsize, (unsigned char *)b64outbuf);
		fclose(f);
	}
	b64data = b64outbuf;

	return 0;
}

int downloadPic(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	string filename = "";
	string picdata = "";
	int picsize = 0;
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/data/picindexvalue", "value"));

	sql = "SELECT picpath FROM xxdb_data_specialviopic WHERE soleid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
	sql += "' LIMIT ";
	sql += itoaa(firstIndex);
	sql += ",1";
	//printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL) filename = row[0];
		}
		mysql_free_result(result);
	}

	getPic((char *)filename.c_str(), picdata, picsize);
	resxml += new_strprop("picsize", "value", itoaa(picsize));
	resxml += "<picdata>";
	resxml += picdata;
	resxml += "</picdata>";
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf("resXML = %s\n", *outxmldata);

	return 0;
}

int callback(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";

	sql = "UPDATE xxdb_data_specialvio SET sendflag = 1 WHERE soleid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
	sql += "'";
	printf("SQl : %s\n", sql.c_str());
	int ret = xx_sql_query(m_sql, sql.c_str());

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();

	return 0;
}
