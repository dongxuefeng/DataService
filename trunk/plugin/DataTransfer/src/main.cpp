#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include <xxxml.h>
#include <dbmepolice.h>
#include <cxx_xml.h>
#include "ftp_client.h"

using namespace std;

typedef struct
{
	char devcode[30];
	char roadcode[30];
	char dcode[30];
	char dcodeOther;
	char vioType[30];
	char vioTime[30];
	char platenum[10];
	char plateType[3];
	char plateColor[4];
	int  picnum;
	char img_path[5][512];
	char img_dstpath[5][512];
	char vioid[50];
}viodata;

FTP_SERVER_INTERFACE ftp_config;
WSADATA WSAData;

bool g_ftpCon = false;
bool g_tcpCon = false;

char FTP_USER[50] = {0};
char FTP_PWD[50] = {0};
char FTP_IP[16] = {0};
int  FTP_PORT = 21;

MYSQL * m_sql = 0;
char picdir[512] = {0};

char * path;
char * filename;

char TCP_IP[16] = {0};
int  TCP_PORT = 0;
SOCKET TCP_CLI_SOCK;

void ftpDisCon(void);
int GetXML(string &xml, viodata * vio);
int GetViodata(vector<viodata> * vioList);
int ftpUpPic(const char * srcpath, char * dstpath);
int SetViodataUpload(const char * vioid, int upload);

int TcpCliConnect(const char * ip, int port);
int TcpCliDisConnect();
int TcpCliSendN(const char * buf, int len);
int TcpCliRecvN(char * buf, int len);

//
///***************************************************************************
// *   Copyright (C) 2009 by foxsonzhou
//
//环境 Ubuntu linux 8.04
//
// ***************************************************************************/
//
//
//
//#include <iostream>
//#include <cstdlib>
//
//using namespace std;
//
//char* InVisualCharFilter(const char* strCharInput,size_t szInput,size_t& szOutput)
//{
// mbstate_t state_towbs;
// memset(&state_towbs, '\0', sizeof(mbstate_t));
//
// wchar_t tempWChar[szInput];
// memset(tempWChar, '\0',szInput*2);
//
// setlocale( LC_CTYPE, "zh_CN.utf8" );
// size_t ret=mbsrtowcs(tempWChar,&strCharInput, szInput, &state_towbs);
//
// size_t lenTempWChar = wcslen(tempWChar);
//
// wchar_t visualWChar[lenTempWChar];
// size_t k=0;
// for(size_t i=0;i<lenTempWChar;i++)
// {
//  wchar_t wc=tempWChar[i];
//  if (iswprint(wc))
//  {
//   visualWChar[k] =wc;
//   k++;
//  }
// }
// size_t szVisualLen= wcslen(visualWChar);
// visualWChar[szVisualLen] ='\0';
//
// char* strCharOutput=(char*) malloc(szVisualLen+1);
// memset(strCharOutput,'\0',szVisualLen+1);
// wcstombs(strCharOutput,visualWChar,szVisualLen);
//
// szOutput=szVisualLen;
// return strCharOutput;
//}
//
//int main(int argc, char *argv[])
//{
// //const char* testChar="址咖哦错";
// const char* testChar="HASH地址表冲突(机框=0 槽=10 子槽=0 端口=3 VLAN ID=1388 MAC 地址=h-) 3";
// char * buf[1024*10] = {0};
// FILE * f = fopen("d:/xml.xml", "r+");
// if(f)
// {
//	 fread(buf, 1, 1024*10, f);
//	 fclose(f);
// }
//
// size_t szInput=strlen((const char*)buf);
// size_t szOutput;
//// while(1) {
//  char * visualStr=InVisualCharFilter((const char*)buf, szInput,szOutput);
//  cout<<"szOutput="<<szOutput<<" visualStr="<<visualStr<<endl;
//  free(visualStr);
// //}
//
//  FILE * p = fopen("d:/1.xml", "w+");
//  if(p)
//  {
// 	 fwrite(visualStr, 1, strlen(visualStr), p);
// 	 fclose(p);
//  }
//   return EXIT_SUCCESS;
//}
//
//


int main( int argc, char *argv[] )
{
#if 0

	ftp_client_init();
	strcpy(ftp_config.m_FtpAddr, "192.168.10.132");
	ftp_config.m_FtpPort = 21;
	strcpy(ftp_config.m_User, "xiangxun");
	strcpy(ftp_config.m_Psd, "88151312");
	ftp_client_connect( &ftp_config );
	/*int ret = ftp_client_access2("123");
	if(ret == FTP_NOLOGIN)
	{
		printf("NOLOGIN !\n");
	}
	else if(ret == FTP_ACCESS_ERR)
	{
		printf("FTP_ACCESS_ERR !\n");
		int ret = ftp_client_mkdir((char *)"123");
		printf("ret = %d\n", ret);
	}
	else
	{
		printf(" ");
	}*/

	int ret = ftp_client_mkdir((char *)"123");
	printf("ret = %d\n", ret);

	ftp_client_putImg("1.jpg", "123/2.jpg");
	ftp_client_close();

	system("pause");
#else

	int dataCount = 0;
	int ret = 0;
	vector<viodata> vioList;
	string xml = "";
	string vioid = "";

	path = new char[512];
	filename = new char[512];

	TCP_CLI_SOCK = INVALID_SOCKET;
	if( WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )//初始化
	{
		printf( "WSAStartup Error!\n" );
		WSACleanup( );
		return -1;
	}

	connectMySQL(m_sql);

	string sql = "SELECT picdirectory,TCPIP,TCPPORT,FTPIP,FTPPORT,FTPUSER,FTPPWD FROM xxdb_info_base WHERE id = 1";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			sprintf(picdir, row[0]);
			sprintf(TCP_IP, row[1]);
			TCP_PORT = atoi(row[2]);
			sprintf(FTP_IP, row[3]);
			FTP_PORT = atoi(row[4]);
			sprintf(FTP_USER, row[5]);
			sprintf(FTP_PWD, row[6]);
		}
		mysql_free_result(result);
	}

	printf("TCP_IP: %s\n", TCP_IP);
	printf("TCP_PORT: %d\n", TCP_PORT);
	printf("FTP_IP: %s\n", FTP_IP);
	printf("FTP_PORT: %d\n", FTP_PORT);
	printf("FTP_USER: %s\n", FTP_USER);
	printf("FTP_PWD: %s\n", FTP_PWD);

	char * p = picdir;
	while(strcmp(p, "") != 0)
	{
		if(*p == '\\') *p = '/';
		p++;
	}

	sprintf(ftp_config.m_FtpAddr, FTP_IP);
	ftp_config.m_FtpPort = FTP_PORT;
	sprintf(ftp_config.m_User, FTP_USER);
	sprintf(ftp_config.m_Psd, FTP_PWD);

	ftp_client_init();
	g_ftpCon = false;
	g_tcpCon = false;

	while(1)
	{
		//ftp断线重连
		while(!g_ftpCon)
		{
			if(ftp_client_connect( &ftp_config ) == FTP_LOGIN_SUCCESS)
			{
				g_ftpCon = true;
			}
			Sleep(3000);
		}

		while(!g_tcpCon)
		{
			if(TcpCliConnect(TCP_IP, TCP_PORT) == 0)
			{
				g_tcpCon = true;
			}
		}

		//获取数据库数据
		dataCount = GetViodata(&vioList);
		if(dataCount == 0)
		{
			printf("Do not have data!\n");
			Sleep(3000);
			continue;
		}
		else
			printf("---------------------------------------\nData Count: %d\n", dataCount);


		int i = 0;
		for(vector<viodata>::iterator data = vioList.begin(); data != vioList.end(); data++)
		{
			printf("=============== Data %d ===============\n", i++);
			vioid = data->vioid;
			//ftp发送图片
			int picnum = data->picnum;
			for(int j = 0; j < picnum; j++)
			{
				ret = ftpUpPic((const char *)data->img_path[i], data->img_dstpath[i]);
				if(ret == -1)
				{
					printf("Ftp Con Error!\n");
					g_ftpCon = false;
					goto END;
				}
				if(ret != 0)
				{
					printf("Ftp Pic Send Error!\n");
					SetViodataUpload(vioid.c_str(), 2);
					goto CONTINUE_SEND;
				}
			}
			printf("Pic Send OK!\n");
			//tcp发送xml
			GetXML(xml, (viodata *)&*data);
			if(TcpCliSendN(xml.c_str(), xml.size()) != xml.size())
			{
				printf("Send Error!\n");
				TcpCliDisConnect();
				g_tcpCon = false;
				break;
			}

			//设置发送标记位
			SetViodataUpload(vioid.c_str(), 1);
			if(ret == 0) printf("SetViodataUpload OK!\n");
			else printf("SetViodataUpload ERROR!\n");
CONTINUE_SEND:
			Sleep(1000);
		}

END:
		vioList.clear();
		printf("***************************\nData Count: %d\n", dataCount);

		Sleep(1000);
	}
#endif

	delete path;
	delete filename;

	WSACleanup( );

	return 0;
}

void ftpDisCon(void)
{
	ftp_client_close();
	Sleep(10);
	ftp_client_init();
	g_ftpCon = false;
}

int SetViodataUpload(const char * vioid, int upload)
{
	string sql = "UPDATE xxdb_data_viodata SET upload = ";
	sql += itoaa(upload);
	sql += " WHERE vioid = '";
	sql += vioid;
	sql += "'";
	printf("SQL : %s\n", sql.c_str());
	return xx_sql_query(m_sql, sql.c_str());
}

int ftpUpPic(const char * srcpath, char * dstpath)
{
	if(strcmp(srcpath, "") == 0) return 0;

	int ret = 0;
	string str = "";

	memset(dstpath, 0, 512);
	memset(path, 0, 512);
	memset(filename, 0, 512);

	char * p = strrchr(srcpath, '/');
	char * q = strchr(srcpath, '/');
	if(p != NULL && q != NULL)
	{
		memcpy(path, q+1, p-q);
		sprintf(filename, p+1);

		//ftp发送图片
		ret = ftp_client_mkdir(path);
		if(ret == -1) return -1;
		sprintf(dstpath, "%s%s", path, filename);
	}
	else
		sprintf(dstpath, filename);

	str = picdir;
	str += '/';
	str += srcpath;
	ret = ftp_client_putImg(str.c_str(), (char *)dstpath);
	if(ret == -1) return -1;
	else if(ret != 1)
	{
		printf("PUT IMAGE ERROR !\n");
		return -2;
	}

	return 0;
}

int GetXML(string &xml, viodata * vio)
{
	char nTime[128] = {0};
	char valdata[128] = {0};
	SYSTEMTIME t;
	GetLocalTime(&t);

	sprintf(nTime, "%d-%d-%d %d:%d:%d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	sprintf(valdata, "V_%d%d%d%d%d%d%d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
	xml += "<rootInfo>";
	xml += "<headInfo>";
	xml += "<devCode>";
	xml += vio->devcode;
	xml += "</devCode>";	//add by jezy 2013-09-12
	xml += "<license>zhuce</license>";	//NOT NULL
	xml += "<comName>mima</comName>";	//NOT NULL
	xml += "<comWord>xuke</comWord>";	//NOT NULL
	xml += "<sendTime>";
	xml += nTime;
	xml += "</sendTime>";	//NOT NULL      //YYYY-MM-DD hh:mm:ss   锟斤拷锟斤拷锟斤拷录锟侥凤拷锟斤拷时锟斤拷
	xml += "<ver>1.2</ver>";	//NOT NULL
	xml += "</headInfo>";
	xml += "<vioInfo>";
	xml += "<roadcode>";
	xml += vio->roadcode;
	xml += "</roadcode>";	//NOT NULL
	xml += "<dcode>";
	xml += vio->dcode;
	xml += "</dcode>";	//NOT NULL
	xml += "<dcodeOther>";
	xml += vio->dcodeOther;
	xml += "</dcodeOther>";	//NOT NULL
	xml += "<lcode></lcode>";	//NOT NULL
	xml += "<ltype></ltype>";	//NOT NULL
	xml += "<vioType>";
	xml += vio->vioType;
	xml += "</vioType>";	//NOT NULL
	xml += "<vioTime>";
	xml += vio->vioTime;
	xml += "</vioTime>";	//NOT NULL
	xml += "<red_end_time></red_end_time>";
	xml += "<red_start_time></red_start_time>";
	xml += "<platenum>";
	xml += vio->platenum;
	xml += "</platenum>";	//NOT NULL 00000000
	xml += "<plateColor>";
	xml += vio->plateColor;
	xml += "</plateColor>";	//NOT NULL 2
	xml += "<plateType>";
	xml += vio->plateType;
	xml += "</plateType>";	//NOT NULL 99
	xml += "<carClass>3</carClass>";	//NOT NULL 3
	xml += "<carColor>Z</carColor>";	//NOT NULL Z
	xml += "<carType>X99</carType>";	//NOT NULL X99
	xml += "<carBrand></carBrand>";	//字符
	xml += "<carLength></carLength>";	//单位 CM
	xml += "<carWidth></carWidth>";	//单位 CM
	xml += "<carWeight></carWeight>";	//单位 kg
	xml += "<speedCar></speedCar>";	//单位km/h
	xml += "<speed_percent></speed_percent>";
	xml += "<speedStd></speedStd>";
	xml += "<speedStd_big></speedStd_big>";	//NOT NULL 0
	xml += "<speedStd_sml></speedStd_sml>";	//NOT NULL 0
	xml += "<img1_path>";
	xml += vio->img_dstpath[0];
	xml += "</img1_path>";	//NOT NULL
	xml += "<img2_path>";
	xml += vio->img_dstpath[1];
	xml += "</img2_path>";
	xml += "<img3_path>";
	xml += vio->img_dstpath[2];
	xml += "</img3_path>";
	xml += "<img4_path>";
	xml += vio->img_dstpath[3];
	xml += "</img4_path>";
	xml += "<img5_path>";
	xml += vio->img_dstpath[4];
	xml += "</img5_path>";
	xml += "<img1_s_path></img1_s_path>";	//NOT NULL
	xml += "<img2_s_path></img2_s_path>";
	xml += "<img3_s_path></img3_s_path>";
	xml += "<img4_s_path></img4_s_path>";
	xml += "<img5_s_path></img5_s_path>";
	xml += "<img1_s_prm></img1_s_prm>";
	xml += "<img2_s_prm></img2_s_prm>";
	xml += "<img3_s_prm></img3_s_prm>";
	xml += "<img4_s_prm></img4_s_prm>";
	xml += "<img5_s_prm></img5_s_prm>";
	xml += "<video_path></video_path>";
	xml += "<img_flag></img_flag>";	//NOT NULL 2-tupian
	xml += "<ftp_flag></ftp_flag>";	//NOT NULL 1-前锟斤拷 2-锟斤拷锟斤拷
	xml += "<valdate>";
	xml += valdata;
	xml += "</valdate>";	//NOT NULL
	xml += "<isUpFile>1</isUpFile>";	//NOT NULL 1-upload modify vData.isUpFile2 to 1
	xml += "</vioInfo>";
	xml += "</rootInfo>\0";

	return 0;
}

void PlatetypeToColor(char *pPlatetype, char *pPlatecolor)
{
	if (!strcmp(pPlatetype ,"23"))
	{
		sprintf(pPlatecolor, "00");
	}
	else if (!strcmp(pPlatetype ,"01"))
	{
		sprintf(pPlatecolor, "01");
	}
	else if (!strcmp(pPlatetype ,"02"))
	{
		sprintf(pPlatecolor, "02");
	}
	else if (!strcmp(pPlatetype ,"06"))
	{
		sprintf(pPlatecolor, "03");
	}
	else
	{
		sprintf(pPlatecolor, "04");
	}
}

int GetViodata(vector<viodata> * vioList)
{
	string sql = "SELECT datetime, viotype, platenum, platetype, vioid, picurl0, picurl1, picurl2, picurl3, picnum FROM xxdb_data_viodata WHERE affirm = 1 AND disposetype = 0 AND upload = 0 LIMIT 0,10";

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			viodata vio;
			memset((void *)&vio, 0, sizeof(viodata));
			sprintf(vio.vioTime, row[0]);
			sprintf(vio.vioType, row[1]);
			sprintf(vio.platenum, row[2]);
			sprintf(vio.plateType, row[3]);
			PlatetypeToColor(vio.plateType, vio.plateColor);
			sprintf(vio.vioid, row[4]);
			printf("vioid = %s\n", row[4]);
			sprintf(vio.img_path[0], row[5]);
			sprintf(vio.img_path[1], row[6]);
			sprintf(vio.img_path[2], row[7]);
			sprintf(vio.img_path[3], row[8]);
			vio.picnum = atoi(row[9]);

			sprintf(vio.devcode, "6100000001");
			sprintf(vio.roadcode, "2135000001");
			sprintf(vio.dcode, "01");
			vio.dcodeOther = '0';

			vioList->push_back(vio);
		}

//		for(vector<viodata>::iterator data = vioList->begin(); data != vioList->end(); data++)
//		{
//			printf("-------------------\n");
//			printf("vioTime = %s\n", data->vioTime);
//			printf("vioType = %s\n", data->vioType);
//			printf("platenum = %s\n", data->platenum);
//			printf("plateType = %s\n", data->plateType);
//			printf("plateColor = %s\n", data->plateColor);
//			printf("vioid = %s\n", data->vioid);
//			printf("picnum = %d\n", data->picnum);
//			for(int i = 0; i < data->picnum; i++)
//				printf("img_path = %s\n", data->img_path[i]);
//		}

		mysql_free_result(result);
	}

	return vioList->size();
}

int TcpCliConnect(const char * ip, int port)
{
	if(strcmp(ip, "") == 0) return -1;
	if(port <= 0) return -1;

	struct sockaddr_in ServerAddr;

	if((TCP_CLI_SOCK = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf( "Socket Error!\n" );
		return -1;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
	ServerAddr.sin_addr.s_addr = inet_addr(ip);
	memset(&(ServerAddr.sin_zero), 0, sizeof(ServerAddr.sin_zero));

	if(connect(TCP_CLI_SOCK, (struct sockaddr *) &ServerAddr, sizeof(struct sockaddr)) == -1)
	{
		printf( "Connect Error!" );
		return -1;
	}

	return 0;
}

int TcpCliDisConnect()
{
	if(TCP_CLI_SOCK != INVALID_SOCKET)
	{
		closesocket(TCP_CLI_SOCK);
		TCP_CLI_SOCK = INVALID_SOCKET;
	}

	return 0;
}

int TcpCliSendN(const char * buf, int len)
{
	int sendlen = len;
	int n = 0;

	while (sendlen > 0)
	{
		n = send(TCP_CLI_SOCK, buf, sendlen, 0);

		if (n < 0)
		{
			printf("[TcpCliSendN] send error!\n");
			return -1;
		}

		if (n == 0)
		{
			return (len - sendlen);
		}

		buf += n;
		sendlen -= n;
	}

	return len;
}
int TcpCliRecvN(char * buf, int len)
{
	int recvlen = len;
	int n = 0;

	while (recvlen > 0)
	{
		n = recv(TCP_CLI_SOCK, buf, recvlen, 0);

		if (n < 0)
		{
			printf("[TcpCliRecvN] recv error!\n");
			return -1;
		}

		if (n == 0)
		{
			return (len - recvlen);
		}

		buf += n;
		recvlen -= n;
	}

	return len;
}
