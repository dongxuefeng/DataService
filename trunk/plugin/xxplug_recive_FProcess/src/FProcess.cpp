/*
 * FProcess.cpp
 *
 *  Created on: 2014-7-3
 *      Author: XXSP
 */

#include "xxplug_recive_FProcess.h"
#include <xxxml.h>
#include <dbmepolice.h>
#include "chinese_code.h"

extern string username;
extern MYSQL * m_sql;
extern string gresXML;
extern string phone;
extern string addr;
//获取用户所需要的编号
int getticketid(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string id = "";
	string sql = "";
	int count = 0;
	int usertype = atoi(xx_xml.xpath_getprop("/userdata/data/usetype", "value"));

	//printf("usetype = %d\n", usertype);
	switch(usertype)
	{
		case 0:
			{
				sql = "SELECT number FROM xxdb_data_cfjdsnumber WHERE isuse = 0 LIMIT 0,1";
				//printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql, sql.c_str()) == 0)
				{
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;

					if((row = mysql_fetch_row(result)))
					{
						if(row[0] != NULL) id = row[0];
					}
					else
						printf("mysql_fetch_row error!");
					mysql_free_result(result);
				}

				sql = "UPDATE xxdb_data_cfjdsnumber SET isuse = 1 WHERE number = '";
				sql += id;
				sql += "'";
				//printf("SQL : %s\n", sql.c_str());
				xx_sql_query(m_sql, sql.c_str());
			}
			break;
		case 1:
			{
				sql = "SELECT number FROM xxdb_data_qznumber WHERE isuse = 0 LIMIT 0,1";
				//printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql, sql.c_str()) == 0)
				{
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;

					if((row = mysql_fetch_row(result)))
					{
						if(row[0] != NULL) id = row[0];
					}
					else
						printf("mysql_fetch_row error!");
					mysql_free_result(result);
				}

				sql = "UPDATE xxdb_data_qznumber SET isuse = 1 WHERE number = '";
				sql += id;
				sql += "'";
				//printf("SQL : %s\n", sql.c_str());
				xx_sql_query(m_sql, sql.c_str());
			}
			break;
		case 2:
			{
				sql = "SELECT number FROM xxdb_data_sgnumber WHERE isuse = 0 LIMIT 0,1";
				//printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql, sql.c_str()) == 0)
				{
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;

					if((row = mysql_fetch_row(result)))
					{
						if(row[0] != NULL) id = row[0];
					}
					else
						printf("mysql_fetch_row error!");
					mysql_free_result(result);
				}

				sql = "UPDATE xxdb_data_sgnumber SET isuse = 1 WHERE number = '";
				sql += id;
				sql += "'";
				//printf("SQL : %s\n", sql.c_str());
				xx_sql_query(m_sql, sql.c_str());
			}
			break;
		case 3:
			{
				sql = "SELECT number FROM xxdb_data_cfjdsnumber WHERE isuse = 0 LIMIT 0,1";
				//printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql, sql.c_str()) == 0)
				{
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;

					if((row = mysql_fetch_row(result)))
					{
						if(row[0] != NULL) id = row[0];
					}
					else
						printf("mysql_fetch_row error!");
					mysql_free_result(result);
				}

				sql = "UPDATE xxdb_data_cfjdsnumber SET isuse = 1 WHERE number = '";
				sql += id;
				sql += "'";
				//printf("SQL : %s\n", sql.c_str());
				xx_sql_query(m_sql, sql.c_str());
			}
			break;
		default:
			break;
	}

	sql = "SELECT name FROM xxdb_info_deviceuser WHERE username = '";
	sql += xx_xml.xpath_getprop("/userdata/data/user", "value");
	sql += "'";
	//printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			resxml += new_strprop("realname", "value", row[0]);
		}
		else
			printf("mysql_fetch_row error!\n");
		mysql_free_result(result);
	}

	resxml += new_strprop("id", "value", id.c_str());
	resxml += new_strprop("addr", "value", addr.c_str());
	resxml += new_strprop("phone", "value", phone.c_str());
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, count, count, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int cancleticketid(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";
	int res = 0;
	int usertype = atoi(xx_xml.xpath_getprop("/userdata/data/usetype", "value"));

	//printf("usetype = %d\n", usertype);

	switch(usertype)
	{
		case 0:
		case 3:
			{
				sql = "UPDATE xxdb_data_cfjdsnumber SET isuse = 0 WHERE number = '";
				sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
				sql += "'";
				//printf("SQL : %s\n", sql.c_str());
				res = xx_sql_query(m_sql, sql.c_str());
			}
			break;
		case 1:
			{
				sql = "UPDATE xxdb_data_qznumber SET isuse = 0 WHERE number = '";
				sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
				sql += "'";
				//printf("SQL : %s\n", sql.c_str());
				res = xx_sql_query(m_sql, sql.c_str());
			}
			break;
		case 2:
			{
				sql = "UPDATE xxdb_data_sgnumber SET isuse = 0 WHERE number = '";
				sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
				sql += "'";
				//printf("SQL : %s\n", sql.c_str());
				res = xx_sql_query(m_sql, sql.c_str());
			}
			break;
		default:
			break;
	}

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int getIDLeaveLine(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	vector<string> idList;
	string id = "";
	string sql = "";
	int usertype = atoi(xx_xml.xpath_getprop("/userdata/data/usetype", "value"));

	//printf("usetype = %d\n", usertype);
	switch(usertype)
	{
		case 0:
			{
				sql = "SELECT number FROM xxdb_data_cfjdsnumber WHERE isuse = 0 LIMIT 0,100";
				printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql, sql.c_str()) == 0)
				{
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;

					while((row = mysql_fetch_row(result)))
					{
						if(row[0] != NULL)
						{
							id = row[0];
							idList.push_back(id);
						}
					}

					mysql_free_result(result);
				}

				for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
				{
					sql = "UPDATE xxdb_data_cfjdsnumber SET isuse = 1, user = '";
					sql += username;
					sql += "'WHERE number = '";
					sql += *iter;
					sql += "'";
					//printf("SQL : %s\n", sql.c_str());
					xx_sql_query(m_sql, sql.c_str());
				}
			}
			break;
		case 1:
			{
				sql = "SELECT number FROM xxdb_data_qznumber WHERE isuse = 0 LIMIT 0,100";
				//printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql, sql.c_str()) == 0)
				{
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;

					while((row = mysql_fetch_row(result)))
					{
						if(row[0] != NULL)
						{
							id = row[0];
							idList.push_back(id);
						}
					}

					mysql_free_result(result);
				}

				for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
				{
					sql = "UPDATE xxdb_data_qznumber SET isuse = 1, user = '";
					sql += username;
					sql += "'WHERE number = '";
					sql += *iter;
					sql += "'";
					//printf("SQL : %s\n", sql.c_str());
					xx_sql_query(m_sql, sql.c_str());
				}
			}
			break;
		case 2:
			{
				sql = "SELECT number FROM xxdb_data_sgnumber WHERE isuse = 0 LIMIT 0,100";
				//printf("SQL : %s\n", sql.c_str());
				if(xx_sql_query(m_sql, sql.c_str()) == 0)
				{
					MYSQL_RES *result = mysql_store_result(m_sql);
					MYSQL_ROW row;

					while((row = mysql_fetch_row(result)))
					{
						if(row[0] != NULL)
						{
							id = row[0];
							idList.push_back(id);
						}
					}

					mysql_free_result(result);
				}

				for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
				{
					sql = "UPDATE xxdb_data_sgnumber SET isuse = 1, user = '";
					sql += username;
					sql += "'WHERE number = '";
					sql += *iter;
					sql += "'";
					//printf("SQL : %s\n", sql.c_str());
					xx_sql_query(m_sql, sql.c_str());
				}
			}
			break;
		default:
			break;
	}

	int count = 1;
	char tmp[20] = {0};
	if(idList.size() != 0)
	{
		resxml += new_strprop("Success", "value", "0");
		resxml += new_strprop("idCount", "value", itoaa(idList.size()));
		for(vector<string>::iterator iter = idList.begin(); iter != idList.end(); iter++)
		{
			memset(tmp, 0, 20);
			sprintf(tmp, "id%d", count);
			resxml += new_strprop((const char*)tmp, "value", (*iter).c_str());
			count++;
		}
	}
	else
		resxml += new_strprop("Success", "value", "1");

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);

	idList.clear();

	return 0;
}

int putResult(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	int usertype = atoi(xx_xml.xpath_getprop("/userdata/data/usertype", "value"));
	string sql = "";
	int ret = 1;

	switch(usertype)
	{
		case 0://手机上传除服决定书处理结果
			{
				sql = "INSERT xxdb_data_cfjds (number, name, phone, license, record, office1, zjlx, platenum, platetype, office2, message, datetime, vioid, money, user,handusername) VALUES ('";
				sql += xx_xml.xpath_getprop("/userdata/data/id", "value");//处罚决定书编号
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");//被处罚人名字
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/phone", "value");//被处罚人电话
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/license", "value");//被处驾照
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/record", "value");//档案
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/office1", "value");//发证机关
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/zjlx", "value");//准驾类型
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");//车牌号
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");//车辆类型
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/office2", "value");//发证机关
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/message", "value");//违法内容
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");//违法时间
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");//违违法唯一识别id
				sql += "',";
				sql += xx_xml.xpath_getprop("/userdata/data/money", "value");//罚款金额
				sql += ",'";
				sql += username;//上传者用户名
				sql += "','";
				sql += username;//手机上传违法处理结果，xxdb_data_cfjds处罚决定书表的上传人和处理人相同
				sql += "')";
			}
			break;
		case 1:
			{
				sql = "INSERT xxdb_data_qzcs (number, name, phone, license, record, office1, zjlx, platenum, platetype, office2, message, datetime, vioid, user) VALUES ('";
				sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/phone", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/license", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/record", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/office1", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/zjlx", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/office2", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/message", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
				sql += "','";
				sql += username;
				sql += "')";
			}
			break;
		case 2:
			{
				char tmp[256];
				string str = "";

				int count = atoi(xx_xml.xpath_getprop("/userdata/data/personnum", "value"));
				for(int i = 1; i <= count; i++)
				{
					memset(tmp, 0, 256);
					sprintf(tmp, "/userdata/data/person%d", i);

					sql = "INSERT xxdb_data_sgperson (name,sex,traffic,phone,license,addr,platenum,platetype,insurance,liability,vioid) VALUES ('";

					str = tmp;
					str += "/name";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					str = tmp;
					str += "/sex";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					str = tmp;
					str += "/traffic";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					str = tmp;
					str += "/phone";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					str = tmp;
					str += "/license";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					str = tmp;
					str += "/addr";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					str = tmp;
					str += "/platenum";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					str = tmp;
					str += "/platetype";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					str = tmp;
					str += "/insurance";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					str = tmp;
					str += "/liability";
					sql += xx_xml.xpath_getprop(str.c_str(), "value");
					sql += "','";

					sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
					sql += "')";

					printf("SQL : %s\n", sql.c_str());
					ret = xx_sql_query(m_sql, sql.c_str());
					if(ret != 0) goto end;
				}

				sql = "INSERT xxdb_data_sgzrrd (number, datetime, location, weather, personnum, message, result, username, vioid) VALUES ('";
				sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/location", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/weather", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/personnum", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/message", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/result", "value");
				sql += "','";
				sql += username;
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
				sql += "')";
			}
			break;
		case 3:
			{
				sql = "INSERT xxdb_data_cfjds (number, name, phone, license, record, office1, zjlx, platenum, platetype, office2, message, datetime, vioid, user,handusername) VALUES ('";
				sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
				sql += "','";
				//sql += xx_xml.xpath_getprop("/userdata/data/name", "value");
				sql += "','";
				//sql += xx_xml.xpath_getprop("/userdata/data/phone", "value");
				sql += "','";
				//sql += xx_xml.xpath_getprop("/userdata/data/license", "value");
				sql += "','";
				//sql += xx_xml.xpath_getprop("/userdata/data/record", "value");
				sql += "','";
				//sql += xx_xml.xpath_getprop("/userdata/data/office1", "value");
				sql += "','";
				//sql += xx_xml.xpath_getprop("/userdata/data/zjlx", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
				sql += "','";
				//sql += xx_xml.xpath_getprop("/userdata/data/office2", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/message", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/datetime", "value");
				sql += "','";
				sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
				sql += "','";
				sql += username;
				sql += "','";
				sql += username;//上传人和处理人都为当前手机用户名
				sql += "')";
			}
			break;
		default:
			break;
	}

	printf("sql: %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	if((usertype == 0)||(usertype == 3))
	{  //更新违法数据库，修改处理的数据已经被手机现场处理过
		sql = "UPDATE xxdb_data_viodata SET disposetype = 1";
		sql += " WHERE vioid = '";
		sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
		sql += "'";
		printf("SQL: %s\n", sql.c_str());
		xx_sql_query(m_sql, sql.c_str());

		int timestamp = 0;
		sql = "SELECT MAX(TIMESTAMP) FROM xxdb_data_blacklist";
		printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if ((row = mysql_fetch_row(result)))
			{
				timestamp = atoi(row[0]);
			}
			mysql_free_result(result);
		}
        //更新黑名单，标记为已删除，手机更新时，自己删除
		sql = "UPDATE xxdb_data_blacklist SET altertype = 2, TIMESTAMP = ";
		sql += itoaa(++timestamp);
		sql += " WHERE platenum = '";
		sql += xx_xml.xpath_getprop("/userdata/data/platenum", "value");
		sql += "' AND platetype = '";
		sql += xx_xml.xpath_getprop("/userdata/data/platetype", "value");
		sql += "'";
		sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
		sql += "'";
		printf("SQL: %s\n", sql.c_str());
		xx_sql_query(m_sql, sql.c_str());
	}

end:
	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int GetNameList(vector<string> * nameList)
{
	string sql = "";

	string datafilter = "";
	sql = "SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'";
	//printf("SQL: %s\n", sql.c_str());
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
	//printf("SQL: %s\n", sql.c_str());
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

int SearchVio(cxx_xml xx_xml, char **outxmldata)
{


	string resxml = "<resultdata>";
	string sql = "";
	string str = "";
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	int count = 0, sumcount = 0, first = 0;
	//用户查询一次，每个都自加1
	sql = "update xxdb_info_deviceuser SET chkcounter=chkcounter+1 WHERE username='";
	sql += username;
	sql += "'";
	printf("SQL: %s\n", sql.c_str());
	xx_sql_query(m_sql, sql.c_str());

	//获取该组及其下属组的所有成员
	vector<string> nameList;
	GetNameList(&nameList);
	//计算总数量
	sql = "SELECT COUNT(0) FROM xxdb_data_viodata WHERE 1=1";
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

	str = xx_xml.xpath_getprop("/userdata/data/dispose", "value");
	if(str.compare("") != 0)
	{
		if(str == "0")
		{
			sql += " AND disposetype = ";
			sql += str;
		}else
		{

		sql += " AND disposetype in  ";
				sql += "(1,2)";
		}
	}

	str = xx_xml.xpath_getprop("/userdata/data/download", "value");
	if(str.compare("") != 0)
	{
		sql += " AND download = ";
		sql += str;
	}

	str = xx_xml.xpath_getprop("/userdata/data/affirm", "value");
	if(str.compare("") != 0)
	{
		sql += " AND affirm = ";
		sql += str;
	}

	str = xx_xml.xpath_getprop("/userdata/data/datafrom", "value");
	if(str.compare("") != 0)
	{
		sql += " AND datafrom = ";
		sql += str;
	}

	sql += " AND (";
	first = 0;
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

	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			sumcount = atoi(row[0]);
		}
		mysql_free_result(result);
	}


	//具体查找结果
	sql = "SELECT a.datetime, a.viotype, a.platenum, a.platetype, a.disposetype, a.vioid, b.name, a.affirm, a.download, a.datafrom FROM xxdb_data_viodata a left join xxdb_info_deviceuser b on a.deviceuser = b.username where 1=1";
	str = xx_xml.xpath_getprop("/userdata/data/stime", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.datetime >= '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/etime", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.datetime <= '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/platenum", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.platenum = '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/platetype", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.platetype = '";
		sql += str;
		sql += "'";
	}

	str = xx_xml.xpath_getprop("/userdata/data/dispose", "value");
	if(str.compare("") != 0)
	{
		if(str == "0")
		{
			sql += " AND a.disposetype = ";
			sql += str;
		}else
		{
			sql += " AND a.disposetype in  ";
			sql += "(1,2)";
		}
	}

	str = xx_xml.xpath_getprop("/userdata/data/download", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.download = ";
		sql += str;
	}

	str = xx_xml.xpath_getprop("/userdata/data/affirm", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.affirm = ";
		sql += str;
	}

	str = xx_xml.xpath_getprop("/userdata/data/datafrom", "value");
	if(str.compare("") != 0)
	{
		sql += " AND a.datafrom = ";
		sql += str;
	}

	sql += " AND (";
	first = 0;
	for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		if(first == 0)
		{
			first++;
			sql += "a.deviceuser = '";
		}
		else
			sql += " OR a.deviceuser = '";
		sql += (*iter).c_str();
		sql += "'";
	}
	sql += ") ORDER BY datetime DESC LIMIT ";
	sql += itoaa(firstIndex);
	sql += ",10";

	printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		count = 0;

		while((row = mysql_fetch_row(result)))
		{
			resxml += "<item";
			resxml += itoaa(++count);
			resxml += ">";
			resxml += new_strprop("datetime", "value", row[0]);
			resxml += new_strprop("viotype", "value", row[1]);
			resxml += new_strprop("platenum", "value", row[2]);
			resxml += new_strprop("platetype", "value", row[3]);
			resxml += new_strprop("dispose", "value", row[4]);
			resxml += new_strprop("vioid", "value", row[5]);
			resxml += new_strprop("user", "value", row[6]);
			resxml += new_strprop("affirm", "value", row[7]);
			resxml += new_strprop("download", "value", row[8]);
			resxml += new_strprop("datafrom", "value", row[9]);
			resxml += "</item";
			resxml += itoaa(count);
			resxml += ">";
		}
		mysql_free_result(result);
	}


	/*for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
	{
		sql = "SELECT COUNT(0) FROM xxdb_data_viodata WHERE 1=1";
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

		str = xx_xml.xpath_getprop("/userdata/data/dispose", "value");
		if(str.compare("") != 0)
		{
			sql += " AND disposetype = ";
			sql += str;
		}

		str = xx_xml.xpath_getprop("/userdata/data/download", "value");
		if(str.compare("") != 0)
		{
			sql += " AND download = ";
			sql += str;
		}

		str = xx_xml.xpath_getprop("/userdata/data/affirm", "value");
		if(str.compare("") != 0)
		{
			sql += " AND affirm = ";
			sql += str;
		}

		sql += " AND deviceuser = '";
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

			sql = "SELECT a.datetime, a.viotype, a.platenum, a.platetype, a.disposetype, a.vioid, b.name, a.affirm, a.download FROM xxdb_data_viodata a,xxdb_info_deviceuser b WHERE a.deviceuser = b.username";
			str = xx_xml.xpath_getprop("/userdata/data/stime", "value");
			if(str.compare("") != 0)
			{
				sql += " AND a.datetime >= '";
				sql += str;
				sql += "'";
			}

			str = xx_xml.xpath_getprop("/userdata/data/etime", "value");
			if(str.compare("") != 0)
			{
				sql += " AND a.datetime <= '";
				sql += str;
				sql += "'";
			}

			str = xx_xml.xpath_getprop("/userdata/data/platenum", "value");
			if(str.compare("") != 0)
			{
				sql += " AND a.platenum = '";
				sql += str;
				sql += "'";
			}

			str = xx_xml.xpath_getprop("/userdata/data/platetype", "value");
			if(str.compare("") != 0)
			{
				sql += " AND a.platetype = '";
				sql += str;
				sql += "'";
			}

			str = xx_xml.xpath_getprop("/userdata/data/dispose", "value");
			if(str.compare("") != 0)
			{
				sql += " AND a.disposetype = ";
				sql += str;
			}

			str = xx_xml.xpath_getprop("/userdata/data/download", "value");
			if(str.compare("") != 0)
			{
				sql += " AND a.download = ";
				sql += str;
			}

			str = xx_xml.xpath_getprop("/userdata/data/affirm", "value");
			if(str.compare("") != 0)
			{
				sql += " AND affirm = ";
				sql += str;
			}

			sql += " AND a.deviceuser = '";
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
						resxml += new_strprop("datetime", "value", row[0]);
						resxml += new_strprop("viotype", "value", row[1]);
						resxml += new_strprop("platenum", "value", row[2]);
						resxml += new_strprop("platetype", "value", row[3]);
						resxml += new_strprop("dispose", "value", row[4]);
						resxml += new_strprop("vioid", "value", row[5]);
						resxml += new_strprop("user", "value", row[6]);
						resxml += new_strprop("affirm", "value", row[7]);
						resxml += new_strprop("download", "value", row[8]);
						resxml += "</item";
						resxml += itoaa(count - firstIndex);
						resxml += ">";
					}

					if((count - firstIndex) >= 10) break;
				}
				mysql_free_result(result);
			}
		}
	}*/

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstIndex, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int WorkSearchVio(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata>";
	string sql = "";
	string str = "";
	int count = 0, sumcount = 0;

	sql = "SELECT COUNT(0) FROM xxdb_data_viodata WHERE 1=1";
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

	sql += " AND deviceuser = '";
	sql += xx_xml.xpath_getprop("/userdata/data/username", "value");
	sql += "'";

	//printf("SQL: %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
			sumcount = atoi(row[0]);

		mysql_free_result(result);
	}

	if(sumcount > 0)
	{
		sql = "SELECT a.datetime, a.viotype, a.platenum, a.platetype, a.disposetype, a.vioid, b.name, a.affirm, a.download, a.picnum FROM xxdb_data_viodata a,xxdb_info_deviceuser b WHERE a.deviceuser = b.username";
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

		sql += " AND deviceuser = '";
		sql += xx_xml.xpath_getprop("/userdata/data/username", "value");
		sql += "'";

		//printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				count++;

				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";
				resxml += new_strprop("datetime", "value", row[0]);
				resxml += new_strprop("viotype", "value", row[1]);
				resxml += new_strprop("platenum", "value", row[2]);
				resxml += new_strprop("platetype", "value", row[3]);
				resxml += new_strprop("dispose", "value", row[4]);
				resxml += new_strprop("vioid", "value", row[5]);
				resxml += new_strprop("user", "value", row[6]);
				resxml += new_strprop("affirm", "value", row[7]);
				resxml += new_strprop("download", "value", row[8]);
				resxml += new_strprop("picnum", "value", row[9]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}
			mysql_free_result(result);
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}


int DetailVio(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";

	sql = "SELECT datetime,viotype,roadname,affirm,picnum,platenum,platetype,disposetype FROM xxdb_data_viodata WHERE vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";
	////printf("SQL: %s\n", sql.c_str());

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			resxml += new_strprop("time", "value", row[0]);
			resxml += new_strprop("viotype", "value", row[1]);
			resxml += new_strprop("vioplace", "value", row[2]);
			resxml += new_strprop("viostate", "value", row[3]);
			resxml += new_strprop("picnum", "value", row[4]);
			resxml += new_strprop("platenum", "value", row[5]);
			resxml += new_strprop("platetype", "value", row[6]);
			resxml += new_strprop("disposetype", "value", row[7]);
		}

		mysql_free_result(result);
	}

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);
	string gbkxml = "";
	CChineseCode::UTF_8ToGB2312(gbkxml, (char *)gresXML.c_str(), gresXML.size());
	////printf("gbkxml = %s\n", gbkxml.c_str());

	return 0;
}

int DetailVioDispose(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";

	sql = "SELECT a.number,a.name,a.phone,a.license,a.record,a.office1,a.zjlx,a.platenum,a.platetype,a.office2,a.message,a.datetime,a.vioid,a.money,b.name,a.handusername FROM xxdb_data_cfjds a LEFT JOIN xxdb_info_deviceuser b ON a.user = b.username WHERE a.vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";
	printf("SQL: %s\n", sql.c_str());

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			resxml += new_strprop("id", "value", row[0]);
			resxml += new_strprop("name", "value", row[1]);
			resxml += new_strprop("phone", "value", row[2]);
			resxml += new_strprop("license", "value", row[3]);
			resxml += new_strprop("record", "value", row[4]);
			resxml += new_strprop("office1", "value", row[5]);
			resxml += new_strprop("zjlx", "value", row[6]);
			resxml += new_strprop("platenum", "value", row[7]);
			resxml += new_strprop("platetype", "value", row[8]);
			resxml += new_strprop("office2", "value", row[9]);
			resxml += new_strprop("message", "value", row[10]);
			resxml += new_strprop("datetime", "value", row[11]);
			resxml += new_strprop("vioid", "value", row[12]);
			resxml += new_strprop("money", "value", row[13]);
			resxml += new_strprop("user", "value", row[14]);
			resxml += new_strprop("handuser", "value", row[15]);//新加处理者用户名
		}

		mysql_free_result(result);
	}

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int SearchQZCS(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata>";
	string sql = "";
	string userstr = xx_xml.xpath_getprop("/userdata/data/username", "value");
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	int count = 0, sumcount = 0;

	if(userstr.compare("") == 0)
	{
		vector<string> nameList;
		GetNameList(&nameList);

		sql = "SELECT COUNT(0) FROM xxdb_data_qzcs WHERE 1=1";
		for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
			sql += AddOrSelectCondition("user = ", (*iter).c_str(), 1);
		sql += AddSelectCondition("datetime >=", xx_xml.xpath_getprop("/userdata/data/stime", "value"), 1);
		sql += AddSelectCondition("datetime <=", xx_xml.xpath_getprop("/userdata/data/etime", "value"), 1);
		sql += AddSelectCondition("platenum =", xx_xml.xpath_getprop("/userdata/data/platenum", "value"), 1);
		sql += AddSelectCondition("platetype =", xx_xml.xpath_getprop("/userdata/data/platetype", "value"), 1);

		//printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
				sumcount = atoi(row[0]);

			mysql_free_result(result);
		}

		////printf("SUMCOUNT = %d\n", sumcount);
		sql = "SELECT a.number,a.name,a.phone,a.license,a.record,a.office1,a.zjlx,a.platenum,a.platetype,a.office2,a.message,a.datetime,a.vioid,b.name FROM xxdb_data_qzcs a LEFT JOIN xxdb_info_deviceuser b ON a.user = b.username WHERE 1=1";
		for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
			sql += AddOrSelectCondition("user = ", (*iter).c_str(), 1);
		sql += AddSelectCondition("datetime >=", xx_xml.xpath_getprop("/userdata/data/stime", "value"), 1);
		sql += AddSelectCondition("datetime <=", xx_xml.xpath_getprop("/userdata/data/etime", "value"), 1);
		sql += AddSelectCondition("platenum =", xx_xml.xpath_getprop("/userdata/data/platenum", "value"), 1);
		sql += AddSelectCondition("platetype =", xx_xml.xpath_getprop("/userdata/data/platetype", "value"), 1);
		sql += " LIMIT ";
		sql += itoaa(firstindex);
		sql += ",10";

		//printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				count++;
				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";
				resxml += new_strprop("id", "value", row[0]);
				resxml += new_strprop("name", "value", row[1]);
				resxml += new_strprop("phone", "value", row[2]);
				resxml += new_strprop("license", "value", row[3]);
				resxml += new_strprop("record", "value", row[4]);
				resxml += new_strprop("office1", "value", row[5]);
				resxml += new_strprop("zjlx", "value", row[6]);
				resxml += new_strprop("platenum", "value", row[7]);
				resxml += new_strprop("platetype", "value", row[8]);
				resxml += new_strprop("office2", "value", row[9]);
				resxml += new_strprop("message", "value", row[10]);
				resxml += new_strprop("datetime", "value", row[11]);
				resxml += new_strprop("vioid", "value", row[12]);
				resxml += new_strprop("user", "value", row[13]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}

			mysql_free_result(result);
		}

		resxml += "</resultdata>";
		setResXML(TYPESTR, count, sumcount, firstindex, resxml, gresXML);
		*outxmldata = (char *)gresXML.c_str();
		//printf("OUTXML : %s\n", *outxmldata);
	}
	else
	{
		sql = "SELECT a.number,a.name,a.phone,a.license,a.record,a.office1,a.zjlx,a.platenum,a.platetype,a.office2,a.message,a.datetime,a.vioid,b.name FROM xxdb_data_qzcs a LEFT JOIN xxdb_info_deviceuser b ON a.user = b.username WHERE 1=1";
		sql += AddSelectCondition("user = ", userstr.c_str(), 1);
		sql += AddSelectCondition("datetime >=", xx_xml.xpath_getprop("/userdata/data/stime", "value"), 1);
		sql += AddSelectCondition("datetime <=", xx_xml.xpath_getprop("/userdata/data/etime", "value"), 1);
		sql += AddSelectCondition("platenum =", xx_xml.xpath_getprop("/userdata/data/platenum", "value"), 1);
		sql += AddSelectCondition("platetype =", xx_xml.xpath_getprop("/userdata/data/platetype", "value"), 1);

		//printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				count++;
				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";
				resxml += new_strprop("id", "value", row[0]);
				resxml += new_strprop("name", "value", row[1]);
				resxml += new_strprop("phone", "value", row[2]);
				resxml += new_strprop("license", "value", row[3]);
				resxml += new_strprop("record", "value", row[4]);
				resxml += new_strprop("office1", "value", row[5]);
				resxml += new_strprop("zjlx", "value", row[6]);
				resxml += new_strprop("platenum", "value", row[7]);
				resxml += new_strprop("platetype", "value", row[8]);
				resxml += new_strprop("office2", "value", row[9]);
				resxml += new_strprop("message", "value", row[10]);
				resxml += new_strprop("datetime", "value", row[11]);
				resxml += new_strprop("vioid", "value", row[12]);
				resxml += new_strprop("user", "value", row[13]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}

			mysql_free_result(result);
		}

		resxml += "</resultdata>";
		setResXML(TYPESTR, count, count, 0, resxml, gresXML);
		*outxmldata = (char *)gresXML.c_str();
		//printf("OUTXML : %s\n", *outxmldata);
	}

	return 0;
}

int SearchAccidentDuty(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata>";
	string sql = "";
	string userstr = xx_xml.xpath_getprop("/userdata/data/username", "value");
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	int count = 0, sumcount = 0;

	if(userstr.compare("") == 0)
	{
		vector<string> nameList;
		GetNameList(&nameList);

		sql = "SELECT COUNT(0) FROM xxdb_data_sgzrrd WHERE 1=1";
		for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
			sql += AddOrSelectCondition("username = ", (*iter).c_str(), 1);
		sql += AddSelectCondition("datetime >=", xx_xml.xpath_getprop("/userdata/data/stime", "value"), 1);
		sql += AddSelectCondition("datetime <=", xx_xml.xpath_getprop("/userdata/data/etime", "value"), 1);

		//printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if((row = mysql_fetch_row(result)))
				sumcount = atoi(row[0]);

			mysql_free_result(result);
		}

		////printf("SUMCOUNT = %d\n", sumcount);
		sql = "SELECT datetime,username,location,personnum,vioid FROM xxdb_data_sgzrrd WHERE 1=1";
		for(vector<string>::iterator iter = nameList.begin(); iter != nameList.end(); iter++)
			sql += AddOrSelectCondition("username = ", (*iter).c_str(), 1);
		sql += AddSelectCondition("datetime >=", xx_xml.xpath_getprop("/userdata/data/stime", "value"), 1);
		sql += AddSelectCondition("datetime <=", xx_xml.xpath_getprop("/userdata/data/etime", "value"), 1);
		sql += " LIMIT ";
		sql += itoaa(firstindex);
		sql += ",10";

		////printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				count++;
				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";
				resxml += new_strprop("datetime", "value", row[0]);
				resxml += new_strprop("user", "value", row[1]);
				resxml += new_strprop("location", "value", row[2]);
				resxml += new_strprop("personnum", "value", row[3]);
				resxml += new_strprop("vioid", "value", row[4]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}

			mysql_free_result(result);
		}

		resxml += "</resultdata>";
		setResXML(TYPESTR, count, sumcount, firstindex, resxml, gresXML);
		*outxmldata = (char *)gresXML.c_str();
		////printf("OUTXML : %s\n", *outxmldata);
	}
	else
	{
		sql = "SELECT datetime,username,location,personnum,vioid FROM xxdb_data_sgzrrd WHERE 1=1";
		sql += AddSelectCondition("username = ", userstr.c_str(), 1);
		sql += AddSelectCondition("datetime >=", xx_xml.xpath_getprop("/userdata/data/stime", "value"), 1);
		sql += AddSelectCondition("datetime <=", xx_xml.xpath_getprop("/userdata/data/etime", "value"), 1);

		//printf("SQL: %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while((row = mysql_fetch_row(result)))
			{
				count++;
				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";
				resxml += new_strprop("datetime", "value", row[0]);
				resxml += new_strprop("user", "value", row[1]);
				resxml += new_strprop("location", "value", row[2]);
				resxml += new_strprop("personnum", "value", row[3]);
				resxml += new_strprop("vioid", "value", row[4]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}

			mysql_free_result(result);
		}

		resxml += "</resultdata>";
		setResXML(TYPESTR, count, count, 0, resxml, gresXML);
		*outxmldata = (char *)gresXML.c_str();
		//printf("OUTXML : %s\n", *outxmldata);
	}

	return 0;
}

int DetailAccidentDuty(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";

	sql = "SELECT xxdb_data_sgzrrd.number,xxdb_data_sgzrrd.datetime,xxdb_data_sgzrrd.location,xxdb_data_sgzrrd.weather,xxdb_data_sgzrrd.message,xxdb_data_sgzrrd.result,xxdb_info_deviceuser.name,xxdb_data_sgzrrd.vioid,xxdb_data_sgzrrd.personnum FROM xxdb_data_sgzrrd,xxdb_info_deviceuser WHERE xxdb_data_sgzrrd.username = xxdb_info_deviceuser.username AND vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";
	////printf("SQL: %s\n", sql.c_str());

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if((row = mysql_fetch_row(result)))
		{
			resxml += new_strprop("id", "value", row[0]);
			resxml += new_strprop("datetime", "value", row[1]);
			resxml += new_strprop("location", "value", row[2]);
			resxml += new_strprop("weather", "value", row[3]);
			resxml += new_strprop("message", "value", row[4]);
			resxml += new_strprop("result", "value", row[5]);
			resxml += new_strprop("user", "value", row[6]);
			resxml += new_strprop("vioid", "value", row[7]);
			resxml += new_strprop("personnum", "value", row[8]);
		}

		mysql_free_result(result);
	}

	int count = 0;

	sql = "SELECT name,sex,traffic,phone,license,addr,platenum,platetype,insurance,liability FROM xxdb_data_sgperson WHERE vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "'";
	////printf("SQL: %s\n", sql.c_str());

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			count++;
			resxml += "<person";
			resxml += itoaa(count);
			resxml += ">";
			resxml += new_strprop("name", "value", row[0]);
			resxml += new_strprop("sex", "value", row[1]);
			resxml += new_strprop("traffic", "value", row[2]);
			resxml += new_strprop("phone", "value", row[3]);
			resxml += new_strprop("license", "value", row[4]);
			resxml += new_strprop("addr", "value", row[5]);
			resxml += new_strprop("platenum", "value", row[6]);
			resxml += new_strprop("platetype", "value", row[7]);
			resxml += new_strprop("insurance", "value", row[8]);
			resxml += new_strprop("liability", "value", row[8]);
			resxml += "</person";
			resxml += itoaa(count);
			resxml += ">";
		}

		mysql_free_result(result);
	}

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	////printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int AddNumber(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata><item1>";
	string sql = "";
	int idtype = atoi(xx_xml.xpath_getprop("/userdata/data/idtype", "value"));
	int count = atoi(xx_xml.xpath_getprop("/userdata/data/count", "value"));
	int startid = atoi(xx_xml.xpath_getprop("/userdata/data/startid", "value"));
	int ret = 1;

	switch(idtype)
	{
		case 0:
		{
			sql = "SELECT * FROM xxdb_data_cfjdsnumber WHERE number = '";
			sql += xx_xml.xpath_getprop("/userdata/data/startid", "value");
			sql += "'";
			//printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if((row = mysql_fetch_row(result)))
				{
					ret = 1;
					break;
				}
				mysql_free_result(result);
			}

			for(int i = 0; i < count; i++)
			{
				sql = "INSERT xxdb_data_cfjdsnumber (number,isuse,datetime) VALUES ('";
				sql += itoaa(startid+i);
				sql += "',0,NOW())";
				//printf("SQL: %s\n", sql.c_str());
				ret = xx_sql_query(m_sql, sql.c_str());
			}
		}
		break;
		case 1:
		{
			sql = "SELECT * FROM xxdb_data_qznumber WHERE number = '";
			sql += xx_xml.xpath_getprop("/userdata/data/startid", "value");
			sql += "'";
			//printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if((row = mysql_fetch_row(result)))
				{
					ret = 1;
					break;
				}
				mysql_free_result(result);
			}

			for(int i = 0; i < count; i++)
			{
				sql = "INSERT xxdb_data_qznumber (number,isuse,datetime) VALUES ('";
				sql += itoaa(startid+i);
				sql += "',0,NOW())";
				//printf("SQL: %s\n", sql.c_str());
				ret = xx_sql_query(m_sql, sql.c_str());
			}
		}
		break;
		case 2:
		{
			sql = "SELECT * FROM xxdb_data_sgnumber WHERE number = '";
			sql += xx_xml.xpath_getprop("/userdata/data/startid", "value");
			sql += "'";
			//printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if((row = mysql_fetch_row(result)))
				{
					ret = 1;
					break;
				}
				mysql_free_result(result);
			}

			for(int i = 0; i < count; i++)
			{
				sql = "INSERT xxdb_data_sgnumber (number,isuse,datetime) VALUES ('";
				sql += itoaa(startid+i);
				sql += "',0,NOW())";
				//printf("SQL: %s\n", sql.c_str());
				ret = xx_sql_query(m_sql, sql.c_str());
			}
		}
		break;
		default:
			break;
	}

	if(ret == 0) ret = 1;
	else if(ret == 1) ret = 2;
	else ret = 3;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";

	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	//printf("OUTXML : %s\n", *outxmldata);

	return 0;
}

int SearchNumber(cxx_xml xx_xml, char **outxmldata)
{
	string resxml = "<resultdata>";
	string sql = "";
	int idtype = atoi(xx_xml.xpath_getprop("/userdata/data/idtype", "value"));
	int count = 0, sumcount = 0;
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));

	switch(idtype)
	{
		case 0:
		{
			sql = "SELECT COUNT(0) FROM xxdb_data_cfjdsnumber";
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if((row = mysql_fetch_row(result)))
				{
					sumcount = atoi(row[0]);
				}
				mysql_free_result(result);
			}

			sql = "SELECT number,isuse FROM xxdb_data_cfjdsnumber LIMIT ";
			sql += itoaa(firstindex);
			sql += ",10";
			////printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while((row = mysql_fetch_row(result)))
				{
					count++;
					resxml += "<item";
					resxml += itoaa(count);
					resxml += ">";
					resxml += new_strprop("id", "value", row[0]);
					resxml += new_strprop("isuse", "value", row[1]);
					resxml += "</item";
					resxml += itoaa(count);
					resxml += ">";
				}
				mysql_free_result(result);
			}
		}
		break;
		case 1:
		{
			sql = "SELECT COUNT(0) FROM xxdb_data_qznumber";
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if((row = mysql_fetch_row(result)))
				{
					sumcount = atoi(row[0]);
				}
				mysql_free_result(result);
			}

			sql = "SELECT number,isuse FROM xxdb_data_qznumber LIMIT ";
			sql += itoaa(firstindex);
			sql += ",10";
			////printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while((row = mysql_fetch_row(result)))
				{
					count++;
					resxml += "<item";
					resxml += itoaa(count);
					resxml += ">";
					resxml += new_strprop("id", "value", row[0]);
					resxml += new_strprop("isuse", "value", row[1]);
					resxml += "</item";
					resxml += itoaa(count);
					resxml += ">";
				}
				mysql_free_result(result);
			}
		}
		break;
		case 2:
		{
			sql = "SELECT COUNT(0) FROM xxdb_data_sgnumber";
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				if((row = mysql_fetch_row(result)))
				{
					sumcount = atoi(row[0]);
				}
				mysql_free_result(result);
			}

			sql = "SELECT number,isuse FROM xxdb_data_sgnumber LIMIT ";
			sql += itoaa(firstindex);
			sql += ",10";
			////printf("SQL: %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;

				while((row = mysql_fetch_row(result)))
				{
					count++;
					resxml += "<item";
					resxml += itoaa(count);
					resxml += ">";
					resxml += new_strprop("id", "value", row[0]);
					resxml += new_strprop("isuse", "value", row[1]);
					resxml += "</item";
					resxml += itoaa(count);
					resxml += ">";
				}
				mysql_free_result(result);
			}
		}
		break;
		default:
			break;
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstindex, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	////printf("OUTXML : %s\n", *outxmldata);

	return 0;
}
