#include "xxplug_recive_Register.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <xxxml.h>
#include <xxbase64.h>
#include <day.h>
#include "md5.h"
#include "myAes.h"

using namespace std;

extern MYSQL * m_sql;
extern string gresXML;
extern string username;
extern MD5_CTX md5;
myAes * aes;

void aesDecryption(char * instr, int inlen, char * outstr, int outlen)
{
	//验证码为base64 反解
	int buflen = b64_decode_string((const unsigned char *)instr, inlen,(unsigned char *)outstr);
	myBuffer debuf((void*)outstr, buflen + 1);
	//反解aes得到 cpuID 和 注册码
	aes->decryption(debuf);
	myBuffer debuf1((void*)outstr, buflen + 1);
	//反解aes得到 cpuID 和 注册码
	aes->decryption(debuf1);
	memset(outstr, 0, outlen);
	memcpy(outstr, debuf._bytes, debuf._len);
}

void aesEecryption(char * instr, int inlen, char * outstr)
{
	myBuffer testBuf((void*)instr, inlen);
	aes->encryption(testBuf);
	b64_encode_string((const unsigned char *)testBuf._bytes, testBuf._len, (unsigned char*)outstr);
}

void aesInit()
{
	aes = new myAes();
	aes->set128Key("4f3c45hvfd1saf3d");
}

void aesDestroy()
{
	delete aes;
}

int getDay(char * date)
{
	SYSTEMTIME t;
	char * pdate = new char[128];

	GetLocalTime(&t);
	sprintf(pdate, "%d-%d-%d", t.wYear, t.wMonth, t.wDay);

	int days = DaysBetween2Date(date, pdate);

	delete pdate;
	return days;
}

//phone
int PhoneRegister(cxx_xml xx_xml, char ** outxmldata)//手机注册
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	string str = xx_xml.xpath_getprop("/userdata/data/checking", "value");
	int qzzc = atoi(xx_xml.xpath_getprop("/userdata/data/qzzc", "value"));//强制注册 0不强制 1强制
	char * cpuID = (char *)malloc(17);
	char * regcode = (char *)malloc(17);
	char * checking = (char *)malloc(128);
	int IsReg = 0;//是否注册标记 0未注册 1已注册
	int id = 0, Isregture = 0,ret = 0;

	memset(cpuID, 0, 17);
	memset(regcode, 0, 17);
	memset(checking, 0, 128);

	aesInit();

	aesDecryption((char *)str.c_str(), str.size(), checking, 128);

	memcpy(cpuID, checking+16, 16);
	memcpy(regcode, checking, 16);

	sql = "SELECT registerCode,isuse FROM xxdb_info_registercode WHERE registerCode = '";
	sql += regcode;
	sql += "'";
	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(strcmp(row[0], regcode) == 0)
			{
				if(atoi(row[1]) == 0)
				{
					ret = 0;
					Isregture = 1;
				}
				else
					ret = -2;
			}
			else
				ret = -1;
		}
		else
			ret = -1;

		mysql_free_result(result);
	}

	if(Isregture == 1)
	{
		sql = "SELECT id,regdate,indate,isend FROM xxdb_info_registercode WHERE CPUID = '";
		sql += cpuID;
		sql += "'";
		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while ((row = mysql_fetch_row(result)))
			{
				if(atoi(row[3]) == 0) //判断是否作废 0为作废 1已作废
					IsReg = (getDay(row[1]) > atoi(row[2]))? 0 : 1;  //判断是否过期
				else
					IsReg = 0;

				if(IsReg == 1) id = atoi(row[0]); //如果有祖册 标记注册的id
			}
			mysql_free_result(result);
		}

		if((IsReg == 0)||((IsReg == 1)&&(qzzc == 1))) //进入注册  条件：没有祖册过的手机 或者 需要强制注册的手机
		{
			sql = "UPDATE xxdb_info_registercode SET isuse = 1, regdate = NOW(), CPUID = '";
			sql += cpuID;
			sql += "' WHERE registerCode = '";
			sql += regcode;
			sql += "'";
			printf("SQL: %s\n", sql.c_str());
			xx_sql_query(m_sql, sql.c_str());

			sql = "UPDATE xxdb_info_registercode SET isend = 1 WHERE id = '";
			sql += itoaa(id);
			sql += "'";
			printf("SQL: %s\n", sql.c_str());
			xx_sql_query(m_sql, sql.c_str());

			sql = "SELECT regdate,indate FROM xxdb_info_registercode WHERE registerCode = '";
			sql += regcode;
			sql += "'";
			printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				char * outdate = (char *)malloc(512);
				if ((row = mysql_fetch_row(result)))
				{
					memset(outdate, 0, 512);
					GetDate((char *)row[0], outdate, atoi(row[1]));
					printf("outdate == %s ==\n", outdate);
					str = cpuID;
					str += outdate;
					printf("str == %s ==\n", str.c_str());
					memset(outdate, 0, 512);
					aesEecryption((char *)str.c_str(), str.size(), outdate);
					resxml += new_strprop("checking","value", outdate);
					printf("outdate == %s ==\n", outdate);
				}
				mysql_free_result(result);
				free(outdate);
			}
		}
		else
			ret = -3;
	}

	resxml += new_strprop("res","value",itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR,1,1,0,resxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	free(cpuID);
	free(regcode);
	free(checking);
	aesDestroy();

	return 0;
}

bool GetRegCode(int count, vector<string> &reglist)
{
	unsigned char * reg = new unsigned char[128];
	unsigned char * regmd5 = new unsigned char[17];
	SYSTEMTIME t;

	printf("===== %d GetRegCode =====\n", count);
	for(int i = 0; i < count; i++)
	{
		GetLocalTime(&t);
		memset(reg, 0, 32);
		sprintf((char *)reg, "%d-%d-%d %d:%d:%d_%d XX123XX%d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, i);
		memset(regmd5, 0, 17);
		printf("%s-", reg);
		MD5Update(&md5,(unsigned char *)reg,strlen((char *)reg));
		MD5Final((unsigned char *)regmd5,&md5);

		memset(reg, 0, 32);
		sprintf((char *)reg, "%02x%02x%02x%02x%02x%02x%02x%02x", regmd5[8], regmd5[9], regmd5[10], regmd5[11], regmd5[12], regmd5[13], regmd5[14], regmd5[15]&0xff);
		printf("%s\n", reg);

		string regcode = (char *)reg;
		reglist.push_back(regcode);
	}
	printf("===== GetRegCode =====\n");

	delete reg;
	delete regmd5;

	return true;
}

char * xiao2da(char * str)
{
	static string da = "";
	int strcount = strlen(str);

	da = "";
	for(int i = 0; i < strcount; i++)
	{
		if((str[i]>='a')&&(str[i]<='z'))
		{
			da += str[i]-('a'-'A');
		}
		else
			da += str[i];
	}

	return (char *)da.c_str();
}

//client
int GetNewRegisterCode(cxx_xml xx_xml, char ** outxmldata)//获取新验证码
{
	string sql = "";
	string resxml = "<resultdata>";
	string str = "";
	string retReg = "";
	vector<string> reglist;
	int regCount = atoi(xx_xml.xpath_getprop("/userdata/data/number", "value"));//注册码数量
	string indate = xx_xml.xpath_getprop("/userdata/data/datetime", "value");//注册码有效期
	string org = xx_xml.xpath_getprop("/userdata/data/org", "value");//获取注册码单位名称
	int count = 0;

	GetRegCode(regCount, reglist);//获取注册码

	for(vector<string>::iterator iter = reglist.begin(); iter != reglist.end(); iter++)
	{
		str = xiao2da((char *)(*iter).c_str());
		count++;
		sql = "INSERT INTO xxdb_info_registercode (registerCode,indate,createdate,org) VALUES ('";
		sql += str;
		sql += "',";
		sql += indate;
		sql += ",NOW(),'";
		sql += org;
		sql += "')";

		xx_sql_query(m_sql, sql.c_str());

		retReg = str.substr(0,4);
		retReg += "-";
		retReg += str.substr(4,4);
		retReg += "-";
		retReg += str.substr(8,4);
		retReg += "-";
		retReg += str.substr(12,4);

		resxml += "<item"; resxml += itoaa(count); resxml += ">";
		resxml += new_strprop("checknum", "value", retReg.c_str());
		resxml += "</item"; resxml += itoaa(count); resxml += ">";
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR,count,count,0,resxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}

int GetOldRegisterCode(cxx_xml xx_xml, char ** outxmldata)//获取旧验证码的使用情况
{
	string sql = "";
	string resxml = "<resultdata>";
	int count = 0;
	string reg = "";
	string retReg = "";

	sql = "SELECT registerCode,isuse,regdate,indate,org FROM xxdb_info_registercode WHERE 1=1 ";
	string str = xx_xml.xpath_getprop("/userdata/data/org", "value");
	if(str.compare("") != 0)
	{
		sql += " and org = '";
		sql += str;
		sql += "'";
	}

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while ((row = mysql_fetch_row(result)))
		{
			reg = row[0];

			retReg = reg.substr(0,4);
			retReg += "-";
			retReg += reg.substr(4,4);
			retReg += "-";
			retReg += reg.substr(8,4);
			retReg += "-";
			retReg += reg.substr(12,4);

			count++;
			resxml += "<item"; resxml += itoaa(count); resxml += ">";
			resxml += new_strprop("checknum", "value", retReg.c_str());
			resxml += new_strprop("isuse", "value", row[1]);
			resxml += "</item"; resxml += itoaa(count); resxml += ">";
		}
		mysql_free_result(result);
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR,count,count,0,resxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUT XML : %s\n", *outxmldata);

	return 0;
}
