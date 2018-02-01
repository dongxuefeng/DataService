/*
 * xx_plug_report_test.cpp
 *
 *  Created on: 2011-9-28
 *      Author: Administrator
 */

#include <cxx_xml.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <string>


#include <dbmepolice.h>
#include <xxxml.h>
#include <xxbase64.h>
#include "xxplug_recive_SearchInfo.h"
#include "getinfo/getinfo.h"
#include "chinese_code.h"

extern MYSQL * m_sql;
extern string gresXML;
extern string username;
extern void * web;

char viopath[] = "./";
char strpic[1024*1024*10];

using namespace std;

char * findChildValue(const char * name, const char * xml, int num)
{
	static char val[1024*1024*10];
	char *p, *q;

	memset(val, 0, 1024*1024*10);
	for(int i = 0; i < num; i++)
	{
		if(i == 0)
			p = strstr(xml, name);
		else
		{
			p += strlen(name);
			p = strstr(p, name);
		}
	}
	if(p != NULL)
	{
		p += strlen(name);
		q = strstr(p, "</");
		if(p != q)
			memcpy(val, p, q-p);
	}
	return val;
}

void upstr(string &str)
{
	static char tmp[64] = {0};

	memset(tmp, 0, 64);
	sprintf(tmp, "%s", str.c_str());
	for(unsigned int i = 0; i < str.size(); ++i)
	{
		if(tmp[i] >= 'a' && tmp[i] <= 'z')
		{
			tmp[i] = tmp[i] - 32;
		}
		else
		{
			continue;
		}
	}
	str = tmp;
}

//char jpegbuf[1024*1024*5] = {0};
int getVerify(char * dev, char * picdata, int &size)
{
	/*int readsize = 0;

	string path = "d:/1.jpg";

	sprintf(tagindex, "%s", "01123");
	FILE * f = fopen(path.c_str(), "rb");
	if(f)
	{
		readsize = fread(picdata, 1, 1024 * 512, f);
		size = readsize;
		fclose(f);
		return -1;
	}
	else
		printf("fopen error!\n");
	printf("----0006   strlen = %d\n", strlen(picdata));*/
	return xxGetVerify(web, dev, picdata, size);
}

int getCarInfo(char * dev, char *platenum, char *platetype, char *maskpcode, char *outdata)
{
	string resxml = "";
	resxml += new_strprop("cartype", "value", platetype);
	resxml += new_strprop("platenum", "value", platenum);
	resxml += new_strprop("owner", "value", "老张");
	resxml += new_strprop("usage", "value", "商用");
	resxml += new_strprop("carcode", "value", "123456789");
	resxml += new_strprop("distinguishcode", "value", "0a14883d2ds5");
	resxml += new_strprop("carcolor", "value", "红");
	resxml += new_strprop("brand", "value", "丰田");
	resxml += new_strprop("issueoffice", "value", "陕西省西安市公安局");
	resxml += new_strprop("addr", "value", "陕西省西安市高新路");
	resxml += new_strprop("regdate", "value", "2015-3-3");
	resxml += new_strprop("issuedate", "value", "2013-5-12");
	resxml += new_strprop("filesnum", "value", "33333");
	resxml += new_strprop("farenum", "value", "33355544");
	resxml += new_strprop("weight", "value", "22");
	resxml += new_strprop("equipweight", "value", "335");
	resxml += new_strprop("size", "value", "1345");
	resxml += "</item1></resultdata>";
	sprintf(outdata, "%s", resxml.c_str());
	return 0;
	//return xxGetCarInfo(web, dev, platenum, platetype, maskpcode, outdata);
}


char verifypic[1024 * 512];
char cardata[1024 * 10] = {0};
//查询车辆信息从六合一平台，并返查询结果
int xx_rcv_getcarinfo(cxx_xml xx_xml, char ** outxmldata, int datafrom)
{
	string strxml = "";
	string resxml = "<resultdata><item1>";
	string ixml = "<resultdata><item1>";
	int count = 0;
	string platenum = xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	string platetype = xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	//string devid = xx_xml.xpath_getprop("/userdata/head/dev", "value");
	string devid = username;
	upstr(platenum);

	//从六合一获取信息
	if(datafrom == 0)		//from 6 to 1		2014.7.5 dongxuefeng
	{
		if(getInfo(2, (char *)platetype.c_str(), (char *)platenum.c_str(), NULL, strxml) == 0)
		{
			int sum = atoi(findChildValue("<rownum>", strxml.c_str(), 1));
			if(sum != 0)
			{
				resxml += new_strprop("cartype", "value", findChildValue("<hpzl>", strxml.c_str(), sum));
				resxml += new_strprop("platenum", "value", findChildValue("<hphm>", strxml.c_str(), sum));
				resxml += new_strprop("owner", "value", findChildValue("<jdcsyr>", strxml.c_str(), sum));
				resxml += new_strprop("usage", "value", findChildValue("<syxz>", strxml.c_str(), sum));
				resxml += new_strprop("carcode", "value", "");
				resxml += new_strprop("distinguishcode", "value", findChildValue("<clsbdh>", strxml.c_str(), sum));
				resxml += new_strprop("carcolor", "value", findChildValue("<csys>", strxml.c_str(), sum));
				resxml += new_strprop("brand", "value", findChildValue("<clpp>", strxml.c_str(), sum));
				resxml += new_strprop("issueoffice", "value", findChildValue("<fzjg>", strxml.c_str(), sum));
				resxml += new_strprop("addr", "value", findChildValue("<zsxxdz>", strxml.c_str(), sum));
				resxml += new_strprop("regdate", "value", "");
				resxml += new_strprop("issuedate", "value", "");
				resxml += new_strprop("filesnum", "value", "");
				resxml += new_strprop("farenum", "value", "");
				resxml += new_strprop("weight", "value", "");
				resxml += new_strprop("equipweight", "value", "");
				resxml += new_strprop("size", "value", "");

				count = 1;
			}
			else
				count = 0;
		}
		else
			count = 0;
		resxml += "</item1></resultdata>";
		setResXML(TYPESTR, count, count, 0, resxml, gresXML);
	}
	else if(datafrom == 1)	//from web 从网页拉去车辆信息
	{
		/*memset(verifypic, 0, 1024 * 512);
		int picsize = 0;

		int ret = getVerify((char *)devid.c_str(), verifypic, picsize);
		if(ret == 0)
		{
			ixml += new_strprop("photosize", "value", itoaa(picsize));
			ixml += "<photomask>";
			ixml += verifypic;
			ixml += "</photomask>";
			ixml += "</item1></resultdata>";
			setResXML("0099", 0, 0, 0, ixml,gresXML);
		}
		else if(ret == -1)
		{
				resxml += "</item1></resultdata>";
				setResXML(TYPESTR, 0, 0, 0, resxml,gresXML);
		}*/
		memset(cardata, 0, 1024*10);
		getCarInfo((char *)devid.c_str(), (char *)platenum.c_str(), (char *)platetype.c_str(), (char *)"", cardata);

		resxml += cardata;
		setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	}

	*outxmldata = (char *) gresXML.c_str();
	printf("OUT xml : %s\n", *outxmldata);
	return 0;
}

char b64out[1024 * 10] = {0};
char utf8out[1024*10] = {0};

int sendPIN(cxx_xml xx_xml, char ** outxmldata)
{
	string maskpcode = xx_xml.xpath_getprop("/userdata/data/maskpcode", "value");
	string platenum = xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	string platetype = xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	//string devid = xx_xml.xpath_getprop("/userdata/head/dev", "value");
	string devid = username;
	string gbkplate = "";

	printf("maskpcode = %s\n", maskpcode.c_str());
	printf("platenum = %s\n", platenum.c_str());
	printf("platetype = %s\n", platetype.c_str());

	upstr(platenum);
	memset(cardata, 0, 1024*10);
	int res = getCarInfo((char *)devid.c_str(), (char *)platenum.c_str(), (char *)platetype.c_str(), (char *)maskpcode.c_str(), cardata);

	if(maskpcode.compare("1234") != 0) res = -1;

	if(res == 0)		//success
	{
		//拼接篇数，返回结果
		memset(b64out, 0, 1024*10);
		memset(utf8out, 0, 1024*10);

		b64_decode_string((const unsigned char *)cardata, strlen(cardata), (unsigned char *)b64out);
		string gbkdata = b64out;
		string utfdata = "";
		CChineseCode::GB2312ToUTF_8(utfdata, (char *)gbkdata.c_str(), gbkdata.size());

		printf("b64out : \n%s\n", b64out);
		printf("gbk : \n%s\n", gbkdata.c_str());
		printf("outxml : \n%s\n", utfdata.c_str());
		*outxmldata = (char *)utfdata.c_str();
		printf("outxml : \n%s\n", *outxmldata);

		return 0;
	}
	else if(res == -1)		//返回验证码
	{
		//重新请求验证码
		memset(verifypic, 0, 1024 * 512);
		int picsize = 0;
		int ret = getVerify((char*)devid.c_str(), verifypic, picsize);

		if(ret == 0)
		{
			string resxml = "<resultdata><item1>";
			resxml += new_strprop("photosize", "value", itoaa(picsize));
			resxml += "<photomask>";
			resxml += verifypic;
			resxml += "</photomask>";
			resxml += "</item1></resultdata>";

			setResXML("0099", 0, 0, 0, resxml,gresXML);
			*outxmldata = (char *) gresXML.c_str();
			return 0;
		}
		else
		{
			string resxml = "<resultdata><item1>";
			resxml += "</item1></resultdata>";
			setResXML(TYPESTR, 0, 0, 0, resxml,gresXML);
			*outxmldata = (char *) gresXML.c_str();
			printf("outxml : \n%s\n", *outxmldata);
			return 0;
		}
	}
	if(res == -2)	//网络错误
	{
		string resxml = "<resultdata><item1>";
		resxml += "</item1></resultdata>";
		setResXML(TYPESTR, -1, -1, 0, resxml,gresXML);
		*outxmldata = (char *) gresXML.c_str();
		printf("outxml : \n%s\n", *outxmldata);
		return 0;
	}

	return -1;
}

int getPinPic(cxx_xml xx_xml, char ** outxmldata)
{
	memset(verifypic, 0, 1024 * 512);
	int picsize = 0;
	string platenum = xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	string platetype = xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	//string devid = xx_xml.xpath_getprop("/userdata/head/user", "value");
	string devid = username;

	int ret  = getVerify((char *)devid.c_str(), verifypic, picsize);
	string resxml = "<resultdata><item1>";
	if(ret == 0)
	{
		resxml += new_strprop("photosize", "value", itoaa(picsize));
		resxml += "<photomask>";
		resxml += verifypic;
		resxml += "</photomask>";
	}
	resxml += "</item1></resultdata>";
	setResXML("0001", 0, 0, 0, resxml,gresXML);
	*outxmldata = (char *) gresXML.c_str();
	printf("OUT xml : %s\n", *outxmldata);
	return 0;
}

int CancelSearch(cxx_xml xx_xml, char ** outxmldata)
{
	//string devid = xx_xml.xpath_getprop("/userdata/head/user", "value");
	string devid = username;

	xxCancelSearch(web, (char *)devid.c_str());
	printf("Cancel Search!\n");

	return 0;
}
//获取驾照信息从六合一
int xx_rcv_getdriverinfo(cxx_xml xx_xml, char ** outxmldata, int datafrom)
{
	string strxml = "";
	string resxml = "<resultdata><item1>";
	int count = 0;

#if 1
	if(getInfo(3, NULL, NULL, (char *)xx_xml.xpath_getprop("/userdata/data/driverlicense", "value"), strxml) == 0)
	{
		int sum = atoi(findChildValue("<rownum>", strxml.c_str(), 1));
		if(sum != 0)
		{
			resxml += new_strprop("driverlicense", "value", findChildValue("<sfzmhm>", strxml.c_str(), sum));
			resxml += new_strprop("name", "value", findChildValue("<xm>", strxml.c_str(), sum));
			resxml += new_strprop("sex", "value", findChildValue("<xb>", strxml.c_str(), sum));
			resxml += new_strprop("birthday", "value", findChildValue("<csrq>", strxml.c_str(), sum));
			resxml += new_strprop("addr", "value", findChildValue("<djzsxxdz>", strxml.c_str(), sum));
			resxml += new_strprop("cartype", "value", findChildValue("<zjcx>", strxml.c_str(), sum));
			resxml += new_strprop("surpluspoint", "value", findChildValue("<ljjf>", strxml.c_str(), sum));
			resxml += new_strprop("fgld", "value", findChildValue("<cclzrq>", strxml.c_str(), sum));
			resxml += new_strprop("startday", "value", findChildValue("<yxqs>", strxml.c_str(), sum));
			resxml += new_strprop("endday", "value", findChildValue("<yxqz>", strxml.c_str(), sum));
			resxml += new_strprop("issueoffice", "value", findChildValue("<fzjg>", strxml.c_str(), sum));
			resxml += new_strprop("filesnum", "value", findChildValue("<dabh>", strxml.c_str(), sum));//档案编号
			resxml += new_strprop("picnum", "value", "1");

			count = 1;
		}
		else
			count = 0;
	}
	else
		count = 0;
#else
	resxml += new_strprop("driverlicense", "value", xx_xml.xpath_getprop("/userdata/data/driverlicense", "value"));
	resxml += new_strprop("name", "value", "老张");
	resxml += new_strprop("sex", "value", "男");
	resxml += new_strprop("birthday", "value", "1984-4-4");
	resxml += new_strprop("addr", "value", "陕西省西安市高新路");
	resxml += new_strprop("cartype", "value", "C1");
	resxml += new_strprop("surpluspoint", "value", "123456");
	resxml += new_strprop("fgld", "value", "0123456");
	resxml += new_strprop("startday", "value", "2010-5-5");
	resxml += new_strprop("endday", "value", "2016-5-5");
	resxml += new_strprop("issueoffice", "value", "西安市公安局");
	resxml += new_strprop("picnum", "value", "1");
	count = 1;
#endif
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, count, count, 0, resxml,gresXML);

	*outxmldata = (char *) gresXML.c_str();
	printf("OUT xml : %s\n", *outxmldata);
	return 0;
}
//获取数据库datafilter 1,2,3的最顶端节点，结果为1
void GetFather(char * groupdatafilter)
{
	string sql = "";
	char * mydatafilter = new char[128];

	sql = "SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'";

	if (xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL)
			{
				memset(mydatafilter, 0, 128);
				if(row[0] != NULL)
				{
					sprintf(mydatafilter, row[0]);
					char * p = strstr(mydatafilter, ",");
					if(p != NULL)
						memcpy(groupdatafilter, mydatafilter, p-mydatafilter);
					else
						memcpy(groupdatafilter, mydatafilter, strlen(mydatafilter));
				}
			}
		}
		mysql_free_result(result);
	}

	delete mydatafilter;
	mydatafilter = NULL;
}
//列出xxdb_info_deviceuser符合条件的所有用户列表，加入sql语句中
void AddUserList(char * father, string &sql)
{
	sql += " AND (";
	string str = "SELECT username FROM xxdb_info_deviceuser WHERE datafilter LIKE '";
	str += father;
	str += "%'";

	if(xx_sql_query(m_sql, str.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		int first = 0;

		while((row = mysql_fetch_row(result)))
		{
			if(first == 0)
			{
				sql += "deviceuser = '";
				first++;
			}
			else
				sql += " OR deviceuser = '";
			sql += row[0];
			sql += "'";
		}

		mysql_free_result(result);
	}

	sql += ")";
}

int selectviodata(cxx_xml xx_xml, char ** outxmldata, int datafrom)
{
	string strxml = "<resultdata>";
	string sql = "";
	string str = "";
	string tmpprop="";
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	int sumcount = 0;
	int icount = 0;

	string getxml = "";
	sumcount = 0;
	string platenum = xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	upstr(platenum);

	//判断从本地还是六合一获取数据
	if(atoi(xx_xml.xpath_getprop("/userdata/data/datefrom", "value")) == 0)//六合一
	{
#if 1
		if(getInfo(2, (char *)xx_xml.xpath_getprop("/userdata/data/platetype", "value"), (char *)platenum.c_str(), NULL, getxml) == 0)
		{
			string tmpprop = "";

			int num = atoi(findChildValue("<rownum>", getxml.c_str(), 1));
			if(num != 0)
			{
				for(int i = 0; i < num; i++)
				{
					if((strcmp(findChildValue("<clbj>", getxml.c_str(), i+1), "0") == 0)||(strcmp(findChildValue("<jkbj>", getxml.c_str(), i+1), "0") == 0))
					{
						sumcount++;
						tmpprop = new_strprop("vioid", "value", findChildValue("<xh>", getxml.c_str(), i+1));
						tmpprop += new_strprop("time", "value", findChildValue("<wfsj>", getxml.c_str(), i+1));
						tmpprop += new_strprop("viotype", "value", findChildValue("<wfxw>", getxml.c_str(), i+1));
						tmpprop += new_strprop("vioplace", "value", findChildValue("<wfdz>", getxml.c_str(), i+1));
						tmpprop += new_strprop("viostate", "value", findChildValue("<clbj>", getxml.c_str(), i+1));
						tmpprop += new_strprop("platenum", "value", findChildValue("<hphm>", getxml.c_str(), i+1));
						tmpprop += new_strprop("platetype", "value", findChildValue("<hpzl>", getxml.c_str(), i+1));
						tmpprop += new_strprop("picnum", "value", "3");
						strxml += setResItem(sumcount, tmpprop);
					}
				}
				icount = sumcount;
			}
			else
			{
				sumcount = 0;
				icount = 0;
			}
		}
		else
		{
			sumcount = 0;
			icount = 0;
		}
#else //模拟数据
		icount = 1;
		sumcount = 1;

		strxml += "<item";
		strxml += itoaa(icount);
		strxml += ">";

		time_t t;
		time(&t);

		strxml += new_strprop("vioid", "value", itoaa(t));
		strxml += new_strprop("time", "value", "2014-6-6");
		strxml += new_strprop("viotype", "value", "1043");
		strxml += new_strprop("vioplace", "value", "陕西省西安市碑林区边家村水文巷");
		strxml += new_strprop("viostate", "value", "0");
		strxml += new_strprop("platenum", "value", platenum.c_str());
		strxml += new_strprop("platetype", "value", xx_xml.xpath_getprop("/userdata/data/platetype", "value"));
		strxml += new_strprop("picnum", "value", "3");

		strxml += "</item";
		strxml += itoaa(icount);
		strxml += ">";
#endif
	}
	else if(atoi(xx_xml.xpath_getprop("/userdata/data/datefrom", "value")) == 1)//本地
	{
		char father[10] = {0};
		GetFather(father);

		printf("father = %s\n", father);

		sql = "SELECT vioid,DATETIME,datetime_end,viotype,roadname,platenum,platetype,picnum FROM xxdb_data_viodata WHERE platenum = '";
		sql += platenum;
		sql += "' AND platetype = '";
		sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
		sql += "'";
		AddUserList(father, sql);

		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			icount = 0;

			while((row = mysql_fetch_row(result)))
			{
				strxml += "<item";
				strxml += itoaa(++icount);
				strxml += ">";

				strxml += new_strprop("vioid", "value", row[0]);
				strxml += new_strprop("time", "value", row[1]);
				strxml += new_strprop("endtime", "value", row[2]);
				strxml += new_strprop("viotype", "value", row[3]);
				strxml += new_strprop("vioplace", "value", row[4]);
				strxml += new_strprop("viostate", "value", "0");
				strxml += new_strprop("platenum", "value", row[5]);
				strxml += new_strprop("platetype", "value", row[6]);
				strxml += new_strprop("picnum", "value", row[7]);

				strxml += "</item";
				strxml += itoaa(icount);
				strxml += ">";
			}

			mysql_free_result(result);

			sumcount = icount;
		}
	}
	else
	{
		printf("datefrom error! = %d\n", atoi(xx_xml.xpath_getprop("/userdata/data/datefrom", "value")));
	}

	strxml += "</resultdata>";
	setResXML(TYPESTR, icount, sumcount, firstindex, strxml,gresXML);
	*outxmldata = (char *) gresXML.c_str();

	printf(" OUT xml: %s\n", *outxmldata);

	return 0;
}

int selectviodatapic(cxx_xml xx_xml, char ** outxmldata, int datafrom)
{
	int count = 0;
	string path = "";
	string strxml = "<resultdata><item1>";
	string getxml;
	int index = atoi(xx_xml.xpath_getprop("/userdata/data/picindexvalue","value"));

#if 0
	if(getInfo(5, NULL, NULL, (char *)xx_xml.xpath_getprop("/userdata/data/vioid", "value"), getxml) == 0)
	{
		int sum = atoi(findChildValue("<rownum>", getxml.c_str(), 1));
		if(sum != 0)
		{
			strxml += new_strprop("vioid", "value", xx_xml.xpath_getprop("/userdata/data/vioid", "value"));
			strxml += new_strprop("picindexvalue", "value", xx_xml.xpath_getprop("/userdata/data/picindexvalue","value"));
			string pic = findChildValue("<zpstr1>", getxml.c_str(), index+1);
			strxml += new_strprop("picsize", "value", itoaa(pic.size()));
			strxml += "<picdata>";
			strxml += pic;
			strxml += "</picdata>";

			count = 1;
		}
		else
			count = 0;
	}
	else
		count = 0;
#else
	strxml += new_strprop("vioid", "value", xx_xml.xpath_getprop("/userdata/data/vioid", "value"));
	strxml += new_strprop("picindexvalue", "value", xx_xml.xpath_getprop("/userdata/data/picindexvalue","value"));

	char filename[128] = {0};
	sprintf(filename, "%d.base64", index);
	printf("filename : %s/n", filename);
	FILE * f = fopen(filename, "rb");
	if(f)
	{
		memset(strpic, 0, 1024*1024*10);
		fread(strpic, 1, 1024*1024*10, f);
		fclose(f);
	}

	strxml += new_strprop("picsize", "value", itoaa(strlen(strpic)));
	strxml += "<picdata>";
	strxml += strpic;
	strxml += "</picdata>";
	count = 1;
#endif

	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, count, count, 0, strxml,gresXML);
	*outxmldata = (char *)gresXML.c_str();

	return 0;
}

int getcarpic(cxx_xml xx_xml, char ** outxmldata, int datafrom)
{
#if 0
	setResXML(TYPESTR, 0, 0, 0, "",gresXML);
	*outxmldata = (char *) gresXML.c_str();
#else
	string strxml = "<resultdata><item1>";

	FILE * f = fopen("1.base64", "rb");
	if(f)
	{
		memset(strpic, 0, 1024*1024*10);
		fread(strpic, 1, 1024*1024*10, f);
		fclose(f);
	}
	strxml += new_strprop("picsize", "value", itoaa(strlen(strpic)));
	strxml += "<picdata>";
	strxml += strpic;
	strxml += "</picdata>";
	strxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, strxml,gresXML);
	*outxmldata = (char *) gresXML.c_str();
#endif
	return 0;
}

int getdriverpic(cxx_xml xx_xml, char ** outxmldata, int datafrom)
{
	string strxml = "";
	string resxml = "<resultdata><item1>";
	int count = 0;
#if 0
	if(getInfo(4, NULL, NULL, (char *)xx_xml.xpath_getprop("/userdata/data/driverlicense", "value"), strxml) == 0)
	{

	}
	else
		count = 0;
#else
	FILE * f = fopen("4.base64", "rb");
	if(f)
	{
		memset(strpic, 0, 1024*1024*10);
		fread(strpic, 1, 1024*1024*10, f);
		fclose(f);
	}
	resxml += new_strprop("picsize", "value", itoaa(strlen(strpic)));
	resxml += "<picdata>";
	resxml += strpic;
	resxml += "</picdata>";
	count = 1;
#endif
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, count, count, 0, resxml,gresXML);
	*outxmldata = (char *) gresXML.c_str();

	return 0;
}

