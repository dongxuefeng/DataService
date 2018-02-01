#include "xxplug_recive_Intercept.h"
#include <dbmepolice.h>
#include <xxxml.h>
#include <xxbase64.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <time.h>
using namespace std;

extern MYSQL * m_sql;
extern string gresXML;
extern string username;
extern string picpath;

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
int ReadInterceptPicFile2B64(const char * filename, char * pb64buf)
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

int GetInterceptFileName(const char * dev, const char * datetime, string &subfileurl)
{
	SYSTEMTIME sys;
	char fileurl[256] = {0};

	GetLocalTime(&sys);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "mkdir %s\\intercept\\%d\\%d\\%d", picpath.c_str(), sys.wYear, sys.wMonth, sys.wDay);
	system(fileurl);
	memset(fileurl, 0, 256);
	sprintf(fileurl, "intercept/%d/%d/%d/%s_%s.jpg", sys.wYear, sys.wMonth, sys.wDay, dev, datetime);

	for (char *p = fileurl; *p != '\0'; *p++)
	{
		if(*p == ':'||*p == '-'||*p == ' ')
			*p = '_';
	}

	subfileurl = fileurl;
	return 0;
}

int insertInterceptData(cxx_xml xx_xml, char ** outxmldata)
{
	SYSTEMTIME t;
	string strpic = "";
	string strxml = "<resultdata><item1>";
	string picdir = "";
	char tmp[512] = {0};

	string sql("INSERT INTO xxdb_data_intercept (platenum,platetype,blt,datetime,picurl0,user,device,carcolor,brand,owner,roadname) VALUES ('");
	sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/blt", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");
	sql += "','";

	GetLocalTime(&t);
	sprintf(tmp, "%d_%d_%d_%d_%d_%d_%d",t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	GetInterceptFileName(xx_xml.xpath_getprop("/userdata/head/dev", "value"), tmp, picdir);
	xx_xml.xpath_gettext("/userdata/data/pic1/picdata", strpic);
	SavePicFile(picdir.c_str(), strpic.c_str());

	sql += picdir;
	sql += "','";
	sql += username;
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/head/dev", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/carcolor", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/brand", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/owner", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/roadname", "value");
	sql += "')";

	int ret = xx_sql_query(m_sql, sql.c_str());
	strxml += new_strprop("res", "value", itoaa(ret));
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 1, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

int selectInterceptData(cxx_xml xx_xml, char ** outxmldata)
{
	int ret = 0;
	int count = 0;
	int sumcount = 0;
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	string resxml = "<resultdata>";
	string struser = xx_xml.xpath_getprop("/userdata/data/username", "value");
	string sql = "";

	if(struser.compare("") != 0)
	{
		sql = "SELECT count(0) FROM xxdb_data_intercept a LEFT JOIN xxdb_info_deviceuser b ON a.user = b.username WHERE a.user  = '";
		sql += struser;
		sql += "' AND `datetime` BETWEEN '";
	}
	else
	{
		sql = "SELECT count(0) FROM xxdb_data_intercept a LEFT JOIN xxdb_info_deviceuser b ON a.user = b.username WHERE a.user IN (SELECT username FROM xxdb_info_deviceuser WHERE datafilter LIKE (SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
		sql += username;
		sql += "')) AND `datetime` BETWEEN '";
	}

	sql += xx_xml.xpath_getprop("/userdata/data/stime", "value");
	sql += "' AND '";
	sql += xx_xml.xpath_getprop("/userdata/data/etime", "value");
	sql += "'";

	printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if ((row = mysql_fetch_row(result)))
		{
			sumcount = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	if(struser.compare("") != 0)
	{
		sql = "SELECT a.user,a.device,a.platenum,a.platetype,a.blt,a.datetime,a.roadname,b.name FROM xxdb_data_intercept a LEFT JOIN xxdb_info_deviceuser b ON a.user = b.username WHERE a.user  = '";
		sql += struser;
		sql += "' AND `datetime` BETWEEN '";
	}
	else
	{
		sql = "SELECT a.user,a.device,a.platenum,a.platetype,a.blt,a.datetime,a.roadname,b.name FROM xxdb_data_intercept a LEFT JOIN xxdb_info_deviceuser b ON a.user = b.username WHERE a.user IN (SELECT username FROM xxdb_info_deviceuser WHERE datafilter LIKE (SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
		sql += username;
		sql += "')) AND `datetime` BETWEEN '";
	}

	sql += xx_xml.xpath_getprop("/userdata/data/stime", "value");
	sql += "' AND '";
	sql += xx_xml.xpath_getprop("/userdata/data/etime", "value");
	sql += "' LIMIT ";
	sql += itoaa(firstindex);
	sql += ",10";

	printf("SQL : %s\n", sql.c_str());
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

			resxml += new_strprop("username", "value", row[0]);
			resxml += new_strprop("real", "value", row[7]);
			resxml += new_strprop("devid", "value", row[1]);
			resxml += new_strprop("platenum", "value", row[2]);
			resxml += new_strprop("platetype", "value", row[3]);
			resxml += new_strprop("bltype", "value", "");
			resxml += new_strprop("blt", "value", row[4]);
			resxml += new_strprop("datetime", "value", row[5]);
			resxml += new_strprop("roadname", "value", row[6]);

			resxml += "</item";
			resxml += itoaa(count);
			resxml += ">";
		}
		mysql_free_result(result);
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstindex, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();


	printf("OUTXML: %s\n", gresXML.c_str());
	return ret;
}

char data[1024*1024*5] = {0};
int selectInterceptPic(cxx_xml xx_xml, char ** outxmldata)
{
	int ret = 0;
	string strxml = "<resultdata><item1>";

	string sql = "SELECT picurl0 FROM xxdb_data_intercept WHERE 1=1";
	sql += " AND `user` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/username", "value");
	sql += "'";
	sql += " AND `datetime` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");
	sql += "'";

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if ((row = mysql_fetch_row(result)))
		{
			memset(data, 0, 1024*1024*5);

			int size = ReadInterceptPicFile2B64(row[0], data);

			strxml += new_strprop("picsize", "value", itoaa(size));
			strxml += "<picdata>";
			strxml += data;
			strxml += "</picdata>";
		}
		mysql_free_result(result);
	}

	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return ret;
}

int GetNameList(vector<string> * nameList)
{
//	int count = 0;
	string sql = "";

//	string datafilter = "";
//	sql = "SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
//	sql += username;
//	sql += "'";
//
//	if(xx_sql_query(m_sql, sql.c_str()) == 0)
//	{
//		MYSQL_RES *result = mysql_store_result(m_sql);
//		MYSQL_ROW row;
//		if ((row = mysql_fetch_row(result)))
//		{
//			datafilter = "";
//			datafilter.append(row[0]);
//		}
//		mysql_free_result(result);
//	}
//
//	if(datafilter.compare("0") == 0)
//	{
//		vector<string> filterList;
//		sql = "SELECT datafilter FROM xxdb_info_usergroup WHERE father = '0' or father = '-1'";
//		if(xx_sql_query(m_sql, sql.c_str()) == 0)
//		{
//			MYSQL_RES *result = mysql_store_result(m_sql);
//			MYSQL_ROW row;
//			string tmp;
//			while ((row = mysql_fetch_row(result)))
//			{
//				count++;
//				tmp = "";
//				tmp.append(row[0]);
//				filterList.push_back(tmp);
//			}
//			mysql_free_result(result);
//		}
//
//		for(vector<string>::iterator iter = filterList.begin(); iter != filterList.end(); iter++)
//		{
//			sql = "SELECT username FROM xxdb_info_deviceuser WHERE datafilter LIKE '";
//			sql += (*iter);
//			sql += "%'";
//			//printf("SQL : %s\n", sql.c_str());
//			if(xx_sql_query(m_sql, sql.c_str()) == 0)
//			{
//				MYSQL_RES *result = mysql_store_result(m_sql);
//				MYSQL_ROW row;
//				string tmp;
//				while ((row = mysql_fetch_row(result)))
//				{
//					count++;
//					tmp = "";
//					tmp.append(row[0]);
//					printf("tmp = %s\n", tmp.c_str());
//					nameList->push_back(tmp);
//				}
//				mysql_free_result(result);
//			}
//		}
//	}
//	else
//	{
//		sql = "SELECT username FROM xxdb_info_deviceuser WHERE datafilter LIKE '";
//		sql += datafilter;
//		sql += "%'";
//		//printf("SQL : %s\n", sql.c_str());
//		if(xx_sql_query(m_sql, sql.c_str()) == 0)
//		{
//			MYSQL_RES *result = mysql_store_result(m_sql);
//			MYSQL_ROW row;
//			string tmp;
//			while ((row = mysql_fetch_row(result)))
//			{
//				count++;
//				tmp = "";
//				tmp.append(row[0]);
//				nameList->push_back(tmp);
//			}
//			mysql_free_result(result);
//		}
//	}

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

	sql = "SELECT username FROM xxdb_info_deviceuser WHERE datafilter LIKE '";
	sql += datafilter;
	sql += "%'";
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

int tidyInterceptData(cxx_xml xx_xml, char ** outxmldata)
{
	int sumcount = atoi(xx_xml.xpath_getprop("/userdata/data/usercount", "value"));
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	int count = 0;
	char tmp[100] = {0};
	string resxml = "<resultdata>";
	string sql = "";
	string progxml = "";
	string stime = xx_xml.xpath_getprop("/userdata/data/stime", "value");
	string etime = xx_xml.xpath_getprop("/userdata/data/etime", "value");
	vector<string> nameList;

	if(sumcount == 0)
	{
		GetNameList(&nameList);
		sumcount = nameList.size();
	}
	else
	{
		string user = "";
		for(count = firstindex; count < sumcount; count++)
		{
			memset(tmp, 0, 100);
			sprintf(tmp, "/userdata/data/user%d", count+1);

			user = "";
			user.append(xx_xml.xpath_getprop(tmp, "value"));
			nameList.push_back(user);
		}
	}


	vector<string>::iterator iter = nameList.begin();
	for(count = firstindex; count < sumcount; count++)
	{
		if((count - firstindex) >= 10) break;

		sql = "select name from xxdb_info_deviceuser where username = '";
		sql += (*iter).c_str();
		sql += "'";

		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if ((row = mysql_fetch_row(result)))
			{
				progxml = new_strprop("user", "value", (*iter).c_str());
				progxml += new_strprop("real", "value", row[0]);
			}
			mysql_free_result(result);
		}

		sql = "SELECT COUNT(1) FROM xxdb_data_intercept WHERE user = '"+*iter+"' AND DATETIME >= '"+stime+"' AND DATETIME <= '"+etime+"'";
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			if ((row = mysql_fetch_row(result)))
			{
				if(row[0] != 0)
					progxml += new_strprop("intercept", "value", row[0]);
				else
					progxml += new_strprop("intercept", "value", "0");
			}
			else
			{
				printf("mysql_fetch_row ERROR!\n");
			}

			mysql_free_result(result);
		}

		resxml += setResItem(count-firstindex+1, progxml);

		iter++;
	}
	nameList.clear();

	resxml += "</resultdata>";
	setResXML(TYPESTR, count-firstindex, sumcount, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("OUTXML: %s\n", *outxmldata);

	return 0;
}

void setSQL(string & sql, string user, string stime, string etime, int type)
{
	switch(type)
	{
	case 0:
	{
		sql = "SELECT COUNT(1) FROM xxdb_data_patorl WHERE deviceuser = '"+user+"' AND DATETIME >= '"+stime+"' AND DATETIME <= '"+etime+"'";
	}
	break;
	case 1:
	{
		sql = "SELECT COUNT(1) FROM xxdb_data_accident WHERE deviceuser = '"+user+"' AND DATETIME >= '"+stime+"' AND DATETIME <= '"+etime+"'";
	}
	break;
	case 2:
	{
		sql = "SELECT COUNT(1) FROM xxdb_data_viodata WHERE deviceuser = '"+user+"' AND DATETIME >= '"+stime+"' AND DATETIME <= '"+etime+"'";
	}
	break;
	case 3:
	{
		sql = "SELECT COUNT(1) FROM xxdb_data_intercept WHERE user = '"+user+"' AND DATETIME >= '"+stime+"' AND DATETIME <= '"+etime+"'";
	}
	break;
	case 4:
	{
		sql = "SELECT COUNT(1) FROM xxdb_data_searchlog WHERE user = '"+user+"' AND data >= '"+stime+"' AND data <= '"+etime+"' AND TYPE = '0001'";
	}
	break;
	case 5:
	{
		sql = "SELECT COUNT(1) FROM xxdb_data_searchlog WHERE user = '"+user+"' AND data >= '"+stime+"' AND data <= '"+etime+"' AND TYPE = '0002'";
	}
	break;
	case 6:
	{
		sql = "select count(0) from xxdb_data_qzcs where user = '"+user+"' and datetime between '"+stime+"' and '"+etime+"'";
	}
	break;
	case 7:
	{
		sql = "select count(0) from xxdb_data_sgzrrd where username = '"+user+"' and datetime between '"+stime+"' and '"+etime+"'";
	}
	break;
	default:
		break;
	}
}

int statisticsWork(cxx_xml xx_xml, char ** outxmldata)
{
	int sumcount = atoi(xx_xml.xpath_getprop("/userdata/data/usercount", "value"));
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	int count = 0;
	char tmp[100] = {0};
	string resxml = "<resultdata>";
	string sql = "";
	string progxml = "";
	string stime = xx_xml.xpath_getprop("/userdata/data/stime", "value");
	string etime = xx_xml.xpath_getprop("/userdata/data/etime", "value");
	vector<string> nameList;

	if(sumcount == 0)
	{
		GetNameList(&nameList);
		sumcount = nameList.size();
	}
	else
	{
		string user = "";
		for(int i = 0; i < sumcount; i++)
		{
			memset(tmp, 0, 100);
			sprintf(tmp, "/userdata/data/user%d", i+1);

			user = "";
			user.append(xx_xml.xpath_getprop(tmp, "value"));
			//printf("user = %s\n", user.c_str());
			nameList.push_back(user);
		}
	}


	vector<string>::iterator iter = nameList.begin();
	iter += firstindex;
	for(count = firstindex; count < sumcount; count++)
	{
		if((count - firstindex) >= 10)
		{
			break;
		}

		//printf("sumcount = %d , count = %d\n , firstindex = %d\n", sumcount, count, firstindex);
		//printf("user = %s\n", (*iter).c_str());

		if(iter == nameList.end()) break;
		sql = "select name from xxdb_info_deviceuser where username = '";
		sql += (*iter).c_str();
		sql += "'";

		//printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if ((row = mysql_fetch_row(result)))
			{
				progxml = new_strprop("user", "value", (*iter).c_str());
				progxml += new_strprop("real", "value", row[0]);
			}
			mysql_free_result(result);
		}

		for(int i = 0; i < 8; i++)
		{
			setSQL(sql, *iter, stime, etime, i);
			//printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				if ((row = mysql_fetch_row(result)))
				{
					switch(i)
					{
						case 0:
						{
							if(row[0] != 0)
								progxml += new_strprop("patrol", "value", row[0]);
							else
								progxml += new_strprop("patrol", "value", "0");
						}
						break;
						case 1:
						{
							if(row[0] != 0)
								progxml += new_strprop("accident", "value", row[0]);
							else
								progxml += new_strprop("accident", "value", "0");
						}
						break;
						case 2:
						{
							if(row[0] != 0)
								progxml += new_strprop("vio", "value", row[0]);
							else
								progxml += new_strprop("vio", "value", "0");
						}
						break;
						case 3:
						{
							if(row[0] != 0)
								progxml += new_strprop("intercept", "value", row[0]);
							else
								progxml += new_strprop("intercept", "value", "0");
						}
						break;
						case 4:
						{
							if(row[0] != 0)
								progxml += new_strprop("car", "value", row[0]);
							else
								progxml += new_strprop("car", "value", "0");
						}
						break;
						case 5:
						{
							if(row[0] != 0)
								progxml += new_strprop("driver", "value", row[0]);
							else
								progxml += new_strprop("driver", "value", "0");
						}
						break;
						case 6:
						{
							if(row[0] != 0)
								progxml += new_strprop("qzcs", "value", row[0]);
							else
								progxml += new_strprop("qzcs", "value", "0");
						}
						break;
						case 7:
						{
							if(row[0] != 0)
								progxml += new_strprop("sgzrrd", "value", row[0]);
							else
								progxml += new_strprop("sgzrrd", "value", "0");
						}
						break;
						default: break;
					}
				}
				else
				{
					printf("mysql_fetch_row ERROR!\n");
				}

				mysql_free_result(result);
			}
			else
			{
				printf("SQL ERROR!\n");
			}
		}
		resxml += setResItem(count-firstindex+1, progxml);

		iter++;
	}
	nameList.clear();

	resxml += "</resultdata>";
	setResXML(TYPESTR, count-firstindex, sumcount, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUTXML: %s\n", *outxmldata);

	return 0;
}

int downPicPath(cxx_xml xx_xml, char ** outxmldata)
{
	string resxml = "<resultdata><item1>";;
	string sql = "";
	int picType = atoi(xx_xml.xpath_getprop("/userdata/data/pictype", "value"));

	switch(picType)
	{
		case 0: //违法图片
		{
			sql = "SELECT picurl";
			sql += xx_xml.xpath_getprop("/userdata/data/index", "value");
			sql +=" FROM xxdb_data_viodata WHERE vioid = '";
			sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
			sql += "'";
		}
		break;
		case 1: //勤务图片
		case 2: //事故图片
		{
			sql = "SELECT picurl FROM xxdb_data_accidentpic WHERE ownerid = '";
			sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
			sql += "'";
			sql +=	" limit ";
			sql += xx_xml.xpath_getprop("/userdata/data/index", "value");
			sql += ",1";
		}
		break;
		case 3: //拦截图片
		{
			sql = "SELECT picurl0 FROM xxdb_data_intercept WHERE 1=1";
			sql += " AND `user` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/username", "value");
			sql += "'";
			sql += " AND `datetime` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");
			sql += "'";
		}
		break;
		case 4: //特殊审核图片
		{
			sql = "SELECT picpath FROM xxdb_data_specialviopic WHERE soleid = '";
			sql += xx_xml.xpath_getprop("/userdata/data/soleid", "value");
			sql += "' LIMIT ";
			sql += xx_xml.xpath_getprop("/userdata/data/index", "value");
			sql += ",1";
		}
		break;
		default:
		break;
	}

	//printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql,sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		string tmpprop="";

		if ((row = mysql_fetch_row(result)))
			resxml += new_strprop("picpath", "value", row[0]);

		mysql_free_result(result);
	}

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1,1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("OUTXML: %s\n", *outxmldata);

	return 0;
}
