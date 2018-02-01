#include "xxplug_recive_Video.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <xxxml.h>
#include <windows.h>
#include "chinese_code.h"
#include <io.h>

#define RTSP_CLI_PORT 5004

extern MYSQL * m_sql;
extern string gresXML;
extern string username;
extern vector<SPServer> svrList;
extern string LocalIP;
extern string PhoneIP;
extern string WebIP;
extern string videopath;
extern string SvrPort;
extern string picdir;

int GetSvrPort()
{
	int port = 9010 + svrList.size();

Rebegin:
	for(vector<SPServer>::iterator iter = svrList.begin(); iter != svrList.end(); iter++)
	{
		if(port == iter->rtspSvrPort)
		{
			port++;
			goto Rebegin;
		}
	}

	return port;
}

int GetSvrUDPPort()
{
	int port = 50000 + svrList.size()*8;

Rebegin:
	for(vector<SPServer>::iterator iter = svrList.begin(); iter != svrList.end(); iter++)
	{
		if(port == iter->rtspCliUDPPort)
		{
			port += 8;
			goto Rebegin;
		}
	}

	return port;
}

int GetSvrPhonePort()
{
	int port = 20000 + svrList.size();

Rebegin:
	for(vector<SPServer>::iterator iter = svrList.begin(); iter != svrList.end(); iter++)
	{
		if(port == iter->rtspPhonePort)
		{
			port++;
			goto Rebegin;
		}
	}

	return port;
}

SPServer * GetSvr(const char * user)
{
	static SPServer * svr = NULL;

	printf("-----------------\n");
	printf("user = %s\n", user);
	printf("svrList.size = %d\n", svrList.size());
	svr = NULL;
	for(vector<SPServer>::iterator iter = svrList.begin(); iter != svrList.end(); iter++)
	{
		printf("iter->dstuser = %s\n", iter->dstuser);
		svr = NULL;
		if(strcmp(user, iter->dstuser) == 0)
		{
			svr = &*iter;
			break;
		}
	}

	return svr;
}

int StartSvr(SPServer * pSvr)
{
	int res = 0;
	string str = "";

	//启动视频服务器
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=TRUE;

	char path[256] = {0};
	SYSTEMTIME t;
	GetLocalTime(&t);
	sprintf(path, "%s\\%d_%d_%d_%d_%d_%d_%d.h264", videopath.c_str(), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	//printf("path = %s\n", path);

	char cc[128] = {0};
	sprintf(cc, "live555ProxyServer.exe -w %s -s %d -f %s -c %d -V -p %d rtsp://%s:5004/1 %s", SvrPort.c_str(), pSvr->rtspPhonePort, path, pSvr->rtspCliUDPPort, pSvr->rtspSvrPort, pSvr->serverIP, pSvr->dstuser);
	//sprintf(cc, "live555ProxyServer.exe -s %d -f %s -c %d -V -p %d rtsp://%s:5004/1 %s", pSvr->rtspPhonePort, path, pSvr->rtspCliUDPPort, pSvr->rtspSvrPort, pSvr->serverIP, pSvr->dstuser);
	printf("cc : %s\n", cc);

	PROCESS_INFORMATION pi;//必备参数设置结束

	//启动服务
	if(!CreateProcess(TEXT(".\\live555ProxyServer.exe"),
	cc,
	NULL,
	NULL,
	FALSE,
	CREATE_NEW_CONSOLE,
	NULL,
	NULL,
	&si,
	&pi
	))
	{//启动服务失败
		//printf("zhuanfa CreateFail!\n");
		res = 1;
	}
	else
	{//启动服务成功
		//printf("zhuanfa start success!\n");
		pSvr->serverhandle = pi.hProcess;
		pSvr->serverID = pi.dwProcessId;

		//printf("ProcessID = %d\n", pi.dwProcessId);

		STARTUPINFO sj;
		sj.cb = sizeof(STARTUPINFO);
		sj.dwFlags=STARTF_USESHOWWINDOW;
		sj.wShowWindow=TRUE;

		string str;
		str += "rtsp://";
		str += LocalIP.c_str();
		str += ":";
		str += itoaa(pSvr->rtspSvrPort);
		str += "/";
		str += pSvr->dstuser;
	}

	return res;
}

int PhoneLogin(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";

	int res = 0;
	SPServer * pSvr = NULL;
	pSvr = GetSvr(username.c_str());

	if(pSvr != NULL)
	{
		printf(" phone is exist! \n");
		if(strcmp(pSvr->serverIP, xx_xml.xpath_getprop("/userdata/head/ip", "value")) != 0)
		{
			char tmp[128] = {0};
			sprintf(tmp, "tskill %d", pSvr->serverID);
			system(tmp);
			pSvr->serverID = 0;
			pSvr->serverhandle = NULL;
			pSvr->clientcount = 0;

			memset(pSvr->serverIP, 0, 21);
			sprintf(pSvr->serverIP, xx_xml.xpath_getprop("/userdata/head/ip", "value"));

			res = StartSvr(pSvr);
			if(res == 0) pSvr->serverStart = 1;
		}
		resxml += new_strprop("res", "value", itoaa(res));
		resxml += new_strprop("port", "value", itoaa(pSvr->rtspPhonePort));
	}
	else
	{
		printf(" phone is new! \n");
		SPServer svr;

		memset(&svr, 0, sizeof(SPServer));
		sprintf(svr.serverIP, xx_xml.xpath_getprop("/userdata/head/ip", "value"));
		sprintf(svr.dstuser, username.c_str());
		svr.rtspSvrPort = GetSvrPort();
		svr.rtspCliUDPPort = GetSvrUDPPort();
		svr.rtspPhonePort = GetSvrPhonePort();
		//svr.rtspPhonePort = 9999;
		res = StartSvr(&svr);
		if(res == 0) svr.serverStart = 1;

		svrList.push_back(svr);
		resxml += new_strprop("res", "value", itoaa(res));
		resxml += new_strprop("port", "value", itoaa(svr.rtspPhonePort));
	}


	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

char * new_node(const char * node, const char * username, const char * realname)
{
	static string str;

	str = "<";
	str += node;
	str += " username=\"";
	str += username;
	str += "\" status=\"1\" userinfo=\"\" realname=\"";
	str += realname;
	str += "\" />";

	return (char *)str.c_str();
}

int GetUserList(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string realname = "";
	string sql = "";
	char tmp[10] = {0};
	int i = 1;

	resxml += "<data coun=\"";
	resxml += itoaa(svrList.size());
	resxml += "\">";


	for(vector<SPServer>::iterator iter = svrList.begin(); iter != svrList.end(); iter++)
	{
		memset(tmp, 0, 10);
		sprintf(tmp, "node%d", i);
		sql = "SELECT name FROM xxdb_info_deviceuser WHERE username = '";
		sql += iter->dstuser;
		sql += "'";
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while ((row = mysql_fetch_row(result)))
			{
				realname = "";
				realname.append(row[0]);
			}
			mysql_free_result(result);
		}

		resxml += new_node(tmp, iter->dstuser, realname.c_str());
		i++;
	}

	resxml += "</data>";
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("OUT XML : %s\n", *outxmldata);
	//string gbkxml = "";
	//CChineseCode::UTF_8ToGB2312(gbkxml, (char *)gresXML.c_str(), gresXML.size());
	//printf("gbkxml = %s\n", gbkxml.c_str());

	return 0;
}

int StartVideo(cxx_xml xx_xml, char **outxmldata)
{
	string user = xx_xml.xpath_getprop("/userdata/data/data", "dst_usernmae");
	string resxml = "<resultdata><item1>";
	SPServer * svr = NULL;

	if(user.compare("") == 0)
	{
		printf("username = NULL\n");
		resxml += "<data ok=\"failed\"/>";
	}
	else
	{
		svr = GetSvr(user.c_str());
		if(svr == NULL)
		{
			printf("have no server\n");
			resxml += "<data ok=\"failed\"/>";
		}
		else
		{
			if(svr->serverStart == 1)
			{
				resxml += "<data ok=\"ok\" rtsp_url=\"rtsp://";

				string str = xx_xml.xpath_getprop("/userdata/head/ip", "value");
				if(str == WebIP)
					resxml += LocalIP.c_str();
				else
					resxml += PhoneIP.c_str();

				resxml += ":";
				resxml += itoaa(svr->rtspSvrPort);
				resxml += "/";
				resxml += svr->dstuser;
				resxml += "\"/>";
			}
			else
			{
				resxml += "<data ok=\"failed\"/>";
			}
		}
	}

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int StopVideo(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string user = xx_xml.xpath_getprop("/userdata/data/uername", "value");
	char tmp[512] = {0};

	for(vector<SPServer>::iterator iter = svrList.begin(); iter != svrList.end(); iter++)
	{
		printf("iter->dstuser = %s user = %s\n", iter->dstuser, user.c_str());
		if(user.compare(iter->dstuser) == 0)
		{
			memset(tmp, 0, 512);
			sprintf(tmp, "tskill %d", iter->serverID);
			//printf("====%s\n",tmp);
			system(tmp);

			svrList.erase(iter);
			break;
		}
	}

	resxml += new_strprop("res", "value", "0");
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	//printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int GetVideoFileList(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	_finddata_t find_data;
	char filetmp[256];
	char file[256];
	int count = 0;

	memset(file, 0, 256);
	sprintf(file, "%s/*.h264", picdir.c_str());
	printf("find file: %s\n", file);
	long find_handle = _findfirst(file,&find_data);

	if(find_handle != -1)
	{
		do
		{
			count++;
			memset(filetmp, 0, 256);
			sprintf(filetmp, "%s/%s", picdir.c_str(), find_data.name);
			printf("load file %s\n", filetmp);

			resxml += "<file";
			resxml += itoaa(count);
			resxml +=" value=\"";
			resxml += find_data.name;
			resxml += "\" url=\"rtsp://";
			resxml += LocalIP;
			resxml += "/";
			resxml += filetmp;
			resxml += "\"/>";

		}while(_findnext(find_handle,&find_data)!= -1);
		_findclose(find_handle);
	}

	resxml += new_strprop("filecount", "value", itoaa(count));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}
