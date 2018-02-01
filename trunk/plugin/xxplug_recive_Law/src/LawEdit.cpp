#include "xxplug_recive_Law.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dbmepolice.h>
#include <xxxml.h>

using namespace std;

extern MYSQL * m_sql;
extern string gresXML;
extern string username;

int insertLaw(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int res = 0;

	sql = "SELECT * FROM xxdb_info_lawtext WHERE `key` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "'";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			printf("key %s is exist!\n", xx_xml.xpath_getprop("/userdata/data/key", "value"));
			res = 1;
			goto end;
		}
		mysql_free_result(result);
	}

	sql = "INSERT INTO xxdb_info_lawtext (law,vioid,money,dealtext,`key`) VALUES ('";
	sql += xx_xml.xpath_getprop("/userdata/data/law", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/money", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/dealtext", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "')";

	printf("SQL : %s\n", sql.c_str());
	res = xx_sql_query(m_sql, sql.c_str());

end:
	if(res == 0) res = 1;
	else if(res == 1) res = 2;
	else res = 3;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int deleteLaw(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int res = 0;

	sql = "DELETE FROM xxdb_info_lawtext WHERE `key` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "'";

	printf("SQL : %s\n", sql.c_str());
	res = xx_sql_query(m_sql, sql.c_str());

	if(res == 0) res = 1;
	else res = 2;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}
int updateLaw(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int res = 0;

	sql = "UPDATE xxdb_info_lawtext SET law = '";
	sql += xx_xml.xpath_getprop("/userdata/data/law", "value");
	sql += "', vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "', money = ";
	sql += xx_xml.xpath_getprop("/userdata/data/money", "value");
	sql += ", dealtext = '";
	sql += xx_xml.xpath_getprop("/userdata/data/dealtext", "value");
	sql += "' WHERE `key` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "'";

	printf("SQL : %s\n", sql.c_str());
	res = xx_sql_query(m_sql, sql.c_str());

	if(res == 0) res = 1;
	else res = 2;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int searchLaw(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	string str = "";
	int count = 0, sumcount = 0;
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));

	sql = "SELECT COUNT(0) FROM xxdb_info_lawtext WHERE 1=1";
	str = xx_xml.xpath_getprop("/userdata/data/key", "value");
	if(str.compare("") != 0)
	{
		sql += " AND `key` = '";
		sql += str;
		sql += "'";
	}
	printf("key = %s\n", xx_xml.xpath_getprop("/userdata/data/key", "value"));
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

	if(sumcount > 0)
	{
		sql = "SELECT law,vioid,money,dealtext,`key` FROM xxdb_info_lawtext";
		str = xx_xml.xpath_getprop("/userdata/data/key", "value");
		if(str.compare("") != 0)
		{
			sql += " WHERE `key` = '";
			sql += str;
			sql += "'";
		}
		sql += " LIMIT ";
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
				resxml += new_strprop("law", "value", row[0]);
				resxml += new_strprop("vioid", "value", row[1]);
				resxml += new_strprop("money", "value", row[2]);
				resxml += new_strprop("dealtext", "value", row[3]);
				resxml += new_strprop("key", "value", row[4]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}
			mysql_free_result(result);
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstindex, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int getLaw(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";

	sql = "SELECT law, vioid, money, dealtext FROM xxdb_info_lawtext WHERE `key` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "'";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			resxml += new_strprop("law", "value", row[0]);
			resxml += new_strprop("vioid", "value", row[1]);
			resxml += new_strprop("money", "value", row[2]);
			resxml += new_strprop("dealtext", "value", row[3]);
		}
		mysql_free_result(result);
	}

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int getKeyList(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	int count = 0;

	sql = "SELECT `key` FROM xxdb_info_lawtext";
	string str = xx_xml.xpath_getprop("/userdata/data/key", "value");
	if(str.compare("") != 0)
	{
		sql += " WHERE `key` LIKE '%";
		sql += str;
		sql += "%'";
		sql += " OR vioid LIKE '%";
		sql += str;
		sql += "%'";
	}

	printf("SQL : %s\n", sql.c_str());

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL)
			{
				count++;
				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";
				resxml += new_strprop("key", "value", row[0]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}
		}
		mysql_free_result(result);
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int getVioTypeList(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	int count = 0;

	sql = "SELECT `key`,vioid, `money` FROM xxdb_info_lawtext";
	printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL)
			{
				count++;
				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";
				resxml += "<viotype value=\"";//key
				resxml += row[0];
				resxml += "\" id=\"";
				resxml += row[1];//违法代码
				resxml += "\" money=\"";//罚款金额
				resxml += row[2];
				resxml += "\"/>";
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}
		}
		mysql_free_result(result);
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int getAddPlan(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	char father[5] = {0};
	char tmp[20] = {0};
	int i = 0;
	int count = 0;
	string resxml = "<resultdata><item1>";

	if(getFather(father) == true)
	{
		sql = "SELECT COUNT(0) FROM xxdb_data_addr WHERE father = ";
		sql += father;
		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if ((row = mysql_fetch_row(result)))
			{
				count = atoi(row[0]);
			}
			mysql_free_result(result);
		}

		resxml += "<addCount value=\"";
		resxml += itoaa(count);
		resxml += "\">";

		if(count > 0)
		{
			sql = "SELECT id,NAME FROM xxdb_data_addr WHERE father = ";
			sql += father;
			printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				i = 1;
				while((row = mysql_fetch_row(result)))
				{
					memset(tmp, 0, 20);
					sprintf(tmp, "addID%d", i);
					resxml += new_strprop(tmp, "value", row[0]);
					memset(tmp, 0, 20);
					sprintf(tmp, "add%d", i++);
					resxml += new_strprop(tmp, "value", row[1]);
				}
				mysql_free_result(result);
			}
		}
		resxml += "</addCount>";

		sql = "SELECT COUNT(0) FROM xxdb_data_plan WHERE father = ";
		sql += father;
		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if ((row = mysql_fetch_row(result)))
			{
				count = atoi(row[0]);
			}
			mysql_free_result(result);
		}

		resxml += "<planCount value=\"";
		resxml += itoaa(count);
		resxml += "\">";

		if(count > 0)
		{
			sql = "SELECT id,NAME FROM xxdb_data_plan WHERE father = ";
			sql += father;
			printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				i = 1;
				while((row = mysql_fetch_row(result)))
				{
					memset(tmp, 0, 20);
					sprintf(tmp, "planID%d", i);
					resxml += new_strprop(tmp, "value", row[0]);
					memset(tmp, 0, 20);
					sprintf(tmp, "plan%d", i++);
					resxml += new_strprop(tmp, "value", row[1]);
				}
				mysql_free_result(result);
			}
		}
		resxml += "</planCount>";
	}
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int getAll(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	int stepcount = 0;
	int orgcount = 0;
	char tmp[20] = {0};
	string resxml = "<resultdata><item1>";

	//search planstep count
	sql = "SELECT COUNT(0) FROM xxdb_data_planstep WHERE father = ";
	sql += xx_xml.xpath_getprop("/userdata/data/planID", "value");
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if((row = mysql_fetch_row(result)))
		{
			stepcount = atoi(row[0]);
		}
		mysql_free_result(result);
	}

	resxml += "<planCount value=\"";
	resxml += itoaa(stepcount);
	resxml += "\">";

	if(stepcount > 0)
	{
		for(int i = 1; i <= stepcount; i++)
		{
			int planstepid = 0;
			//search planstep
			sql = "SELECT id,NAME,details FROM xxdb_data_planstep WHERE father = ";
			sql += xx_xml.xpath_getprop("/userdata/data/planID", "value");
			sql += " AND stepcount = ";
			sql += itoaa(i);
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				if((row = mysql_fetch_row(result)))
				{
					memset(tmp, 0, 20);
					sprintf(tmp, "planID%d", i);
					resxml += "<"; resxml += tmp; resxml += " value=\"";
					resxml += row[0];
					resxml += "\">";

					planstepid = atoi(row[0]);
					resxml += new_strprop("pname", "value", row[1]);
					resxml += new_strprop("details", "value", row[2]);
					resxml += new_strprop("xh", "value", itoaa(i));
				}
				mysql_free_result(result);
			}

			//search org count
//			sql = "SELECT COUNT(0) FROM xxdb_data_organization WHERE id IN (SELECT organization FROM xxdb_data_relatesteporganization WHERE planstep = ";
//			sql += itoaa(planstepid);
//			sql += ") AND addrID = ";
//			sql += xx_xml.xpath_getprop("/userdata/data/addID", "value");

			sql = "SELECT COUNT(0) FROM xxdb_data_organization WHERE id IN (SELECT organization FROM xxdb_data_relatesteporganization WHERE planstep = ";
			sql += itoaa(planstepid);
			sql += ")";

			printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				if((row = mysql_fetch_row(result)))
				{
					orgcount = atoi(row[0]);
				}
				mysql_free_result(result);
			}

			resxml += "<orgCount value=\""; resxml += itoaa(orgcount); resxml += "\">";

			//search org
/*			sql = "SELECT NAME,phone1,phone2,phone3,phone4,phone5 FROM xxdb_data_organization WHERE id IN (SELECT organization FROM xxdb_data_relatesteporganization WHERE planstep = ";
			sql += itoaa(planstepid);
			sql += ") AND addrID = ";
			sql += xx_xml.xpath_getprop("/userdata/data/addID", "value");*/

			sql = "SELECT NAME,phone1,phone2,phone3,phone4,phone5 FROM xxdb_data_organization WHERE id IN (SELECT organization FROM xxdb_data_relatesteporganization WHERE planstep = ";
			sql += itoaa(planstepid);
			sql += ")";

			printf("SQL : %s\n", sql.c_str());
			if(xx_sql_query(m_sql, sql.c_str()) == 0)
			{
				MYSQL_RES *result = mysql_store_result(m_sql);
				MYSQL_ROW row;
				char tmp1[20] = {0};
				int j = 1;
				int phonecount = 0;
				while((row = mysql_fetch_row(result)))
				{
					memset(tmp1, 0, 20);
					sprintf(tmp1, "org%d", j);
					resxml += "<"; resxml += tmp1; resxml += ">";

					resxml += new_strprop("oname", "value", row[0]);
					for(int m = 1; m <= 5; m++)
					{
						if(row[m] != NULL) phonecount++;
					}

					resxml += "<phoneCount value=\"";
					resxml += itoaa(phonecount);
					resxml += "\">";

					for(int m = 1; m <= phonecount; m++)
					{
						memset(tmp1, 0, 20);
						sprintf(tmp1, "phone%d", m);
						resxml += new_strprop(tmp1, "value", row[m]);
					}

					resxml += "</phoneCount>";

					memset(tmp1, 0, 20);
					sprintf(tmp1, "org%d", j++);
					resxml += "</"; resxml += tmp1; resxml += ">";
				}
				mysql_free_result(result);
			}

			resxml += "</orgCount>";
			resxml += "</"; resxml += tmp; resxml += ">";
		}
	}
	resxml += "</planCount>";
	resxml += "</item1></resultdata>";

	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int insertVioArticle(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int res = 0;

	sql = "SELECT * FROM xxdb_data_vioarticle WHERE `key` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "'";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			printf("key %s is exist!\n",xx_xml.xpath_getprop("/userdata/data/key", "value"));
			res = 1;
			goto end;
		}
		mysql_free_result(result);
	}

	sql = "INSERT INTO xxdb_data_vioarticle (law,vioid,money,dealtext,`key`) VALUES ('";
	sql += xx_xml.xpath_getprop("/userdata/data/law", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/money", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/dealtext", "value");
	sql += "','";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "')";

	printf("SQL : %s\n", sql.c_str());
	res = xx_sql_query(m_sql, sql.c_str());

end:
	if(res == 0) res = 1;
	else if(res == 1) res = 2;
	else res = 3;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int deleteVioArticle(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int res = 0;

	sql = "DELETE FROM xxdb_data_vioarticle WHERE `key` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "'";

	printf("SQL : %s\n", sql.c_str());
	res = xx_sql_query(m_sql, sql.c_str());

	if(res == 0) res = 1;
	else res = 2;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}
int updateVioArticle(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int res = 0;

	sql = "UPDATE xxdb_data_vioarticle SET law = '";
	sql += xx_xml.xpath_getprop("/userdata/data/law", "value");
	sql += "', vioid = '";
	sql += xx_xml.xpath_getprop("/userdata/data/vioid", "value");
	sql += "', money = '";
	sql += xx_xml.xpath_getprop("/userdata/data/money", "value");
	sql += "', dealtext = '";
	sql += xx_xml.xpath_getprop("/userdata/data/dealtext", "value");
	sql += "' WHERE `key` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "'";

	printf("SQL : %s\n", sql.c_str());
	res = xx_sql_query(m_sql, sql.c_str());

	if(res == 0) res = 1;
	else res = 2;

	resxml += new_strprop("res", "value", itoaa(res));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int searchVioArticle(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	string str = "";
	int count = 0, sumcount = 0;
	int firstindex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));

	sql = "SELECT COUNT(0) FROM xxdb_data_vioarticle WHERE 1=1";
	str = xx_xml.xpath_getprop("/userdata/data/key", "value");
	if(str.compare("") != 0)
	{
		sql += " AND `key` = '";
		sql += str;
		sql += "'";
	}
	printf("key = %s\n", xx_xml.xpath_getprop("/userdata/data/key", "value"));
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

	if(sumcount > 0)
	{
		sql = "SELECT law,vioid,money,dealtext,`key` FROM xxdb_data_vioarticle";
		str = xx_xml.xpath_getprop("/userdata/data/key", "value");
		if(str.compare("") != 0)
		{
			sql += " WHERE `key` = '";
			sql += str;
			sql += "'";
		}
		sql += " LIMIT ";
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
				resxml += new_strprop("law", "value", row[0]);
				resxml += new_strprop("vioid", "value", row[1]);
				resxml += new_strprop("money", "value", row[2]);
				resxml += new_strprop("dealtext", "value", row[3]);
				resxml += new_strprop("key", "value", row[4]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}
			mysql_free_result(result);
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, sumcount, firstindex, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

bool getFather(char * father)
{
	if(father == NULL) return false;

	string sql = "";
	char datafilter[64] = {0};

	sql = "SELECT datafilter FROM xxdb_info_deviceuser WHERE username = '";
	sql += username;
	sql += "'";

	//printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			sprintf(datafilter, row[0]);
		}
		else
			return false;
		mysql_free_result(result);
	}
	else
		return false;

	char * p = strstr(datafilter, ",");

	if(p == NULL)
	{
		sprintf(father, datafilter);
	}
	else
	{
		memcpy(father, datafilter, p-datafilter);
	}

	printf("father = %s\n", father);

	return true;
}

//phone
int getVio(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";

	sql = "SELECT law, vioid, money, dealtext FROM xxdb_data_vioarticle WHERE `key` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/key", "value");
	sql += "'";
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		if ((row = mysql_fetch_row(result)))
		{
			resxml += new_strprop("law", "value", row[0]);
			resxml += new_strprop("vioid", "value", row[1]);
			resxml += new_strprop("money", "value", row[2]);
			resxml += new_strprop("dealtext", "value", row[3]);
		}
		mysql_free_result(result);
	}

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int getVioKeyList(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	int count = 0;

	sql = "SELECT `key` FROM xxdb_data_vioarticle";
	string str = xx_xml.xpath_getprop("/userdata/data/key", "value");
	if(str.compare("") != 0)
	{
		sql += " WHERE `key` LIKE '%";
		sql += str;
		sql += "%'";
	}
	printf("SQL : %s\n", sql.c_str());

	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;

		while((row = mysql_fetch_row(result)))
		{
			if(row[0] != NULL)
			{
				count++;
				resxml += "<item";
				resxml += itoaa(count);
				resxml += ">";
				resxml += new_strprop("key", "value", row[0]);
				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}
		}
		mysql_free_result(result);
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}
//plan
//web
//plan
int insertPlan(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	char father[5] = {0};
	int ret = 0;

	if(getFather(father) == true)
	{
		sql = "INSERT INTO xxdb_data_plan (`name`,father) VALUES ('";
		sql += xx_xml.xpath_getprop("/userdata/data/planName", "value");
		sql += "',";
		sql += father;
		sql += ")";
		printf("SQL : %s\n", sql.c_str());
		ret = xx_sql_query(m_sql, sql.c_str());

		int stepCount = atoi(xx_xml.xpath_getprop("/userdata/data/stepCount", "value"));
		for(int i = 1; i <= stepCount; i++)
		{
			char tmp[60] = {0};
			sprintf(tmp, "/userdata/data/step%d", i);
			sql = "INSERT INTO xxdb_data_planstep (`name`,details,father,stepcount) VALUES ('";
			sql += xx_xml.xpath_getprop(tmp, "value");
			sql += "','";
			sql += xx_xml.xpath_getprop(tmp, "detail");
			sql += "',(SELECT id FROM xxdb_data_plan WHERE `name` = '";
			sql += xx_xml.xpath_getprop("/userdata/data/planName", "value");
			sql += "'AND father = ";
			sql += father;
			sql += "),";
			sql += itoaa(i);
			sql += ")";
			printf("SQL : %s\n", sql.c_str());
			ret = xx_sql_query(m_sql, sql.c_str());

			int orgcount = atoi(xx_xml.xpath_getprop(tmp, "orgCount"));
			printf("orgCount = %d\n", orgcount);
			for(int j = 1; j <= orgcount; j++)
			{
				memset(tmp, 0, 60);
				sprintf(tmp, "/userdata/data/step%d/org%d", i, j);
				printf("tmp = %s\n", tmp);
				sql = "INSERT INTO xxdb_data_relatesteporganization (planstep, organization) VALUES ((SELECT id FROM xxdb_data_planstep WHERE father = (SELECT id FROM xxdb_data_plan WHERE `name` = '";
				sql += xx_xml.xpath_getprop("/userdata/data/planName", "value");
				sql += "'AND father = ";
				sql += father;
				sql += ") AND stepcount = ";
				sql += itoaa(i);
				sql += "), ";
				sql += xx_xml.xpath_getprop(tmp, "value");
				sql += ")";
				printf("SQL : %s\n", sql.c_str());
				ret = xx_sql_query(m_sql, sql.c_str());
			}
		}
	}
	else
		ret = 1;

	if(ret == 0) ret = 1;
	else if(ret == 1) ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int deletePlan(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "DELETE FROM xxdb_data_relatesteporganization WHERE planstep IN (SELECT id FROM xxdb_data_planstep WHERE father = ";
	sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
	sql += ")";
	printf("SQL : %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	sql = "DELETE FROM xxdb_data_planstep WHERE father = ";
	sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
	printf("SQL : %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	sql = "DELETE FROM xxdb_data_plan WHERE id = ";
	sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
	printf("SQL : %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

typedef struct
{
	string name;
	string phone;
	string id;
}org;

typedef struct
{
	string id;
	string name;
	string detail;
	string stepCount;
	vector<org> orgs;
}step;

typedef struct
{
	string id;
	string name;
	vector<step> steps;
}plan;

int searchPlanDetail(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	plan plans;

	sql = "SELECT a.id,a.name,b.name,b.details,b.stepcount,b.id FROM xxdb_data_plan a, xxdb_data_planstep b WHERE a.id = '";
	sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
	sql += "' AND b.father = a.id ORDER BY b.stepcount ASC";

	printf("SQL : %s\n", sql.c_str());
	if(xx_sql_query(m_sql, sql.c_str()) == 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		int old_id = 0;

		while ((row = mysql_fetch_row(result)))
		{
			if(old_id != atoi(row[0]))
			{
				old_id = atoi(row[0]);

				plans.id = row[0];
				plans.name = row[1];

				step vStep;
				vStep.name = row[2];
				vStep.detail = row[3];
				vStep.stepCount = row[4];
				vStep.id = row[5];

				plans.steps.push_back(vStep);
			}
			else
			{
				step vStep;
				vStep.name = row[2];
				vStep.detail = row[3];
				vStep.stepCount = row[4];
				vStep.id = row[5];

				plans.steps.push_back(vStep);
			}
		}
		mysql_free_result(result);
	}

	for(vector<step>::iterator iter_step = plans.steps.begin(); iter_step != plans.steps.end(); iter_step++)
	{
		sql = "SELECT `name`,phone1 FROM xxdb_data_organization WHERE id IN (SELECT organization FROM xxdb_data_relatesteporganization WHERE planstep = ";
		sql += (*iter_step).id;
		sql += ")";

		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			while ((row = mysql_fetch_row(result)))
			{
				org vOrg;

				vOrg.name = row[0];
				vOrg.phone = row[1];

				(*iter_step).orgs.push_back(vOrg);
			}
			mysql_free_result(result);
		}
	}

	int i = 0;

	resxml += "<planName value=\"";
	resxml += plans.name;
	resxml += "\" id=\"";
	resxml += plans.id;
	resxml += "\"/>";

	resxml += new_strprop("stepCount", "value", itoaa(plans.steps.size()));

	for(vector<step>::iterator iter_step = plans.steps.begin(); iter_step != plans.steps.end(); iter_step++)
	{
		int j = 0;
		i++;
		resxml += "<step";
		resxml += itoaa(i);
		resxml += " value=\"";
		resxml += (*iter_step).name;
		resxml += "\" detail=\"";
		resxml += (*iter_step).detail;
		resxml += "\" orgCount=\"";
		resxml += itoaa((*iter_step).orgs.size());
		resxml += "\">";

		for(vector<org>::iterator iter_org = (*iter_step).orgs.begin(); iter_org != (*iter_step).orgs.end(); iter_org++)
		{
			j++;
			resxml += "<org";
			resxml += itoaa(j);
			resxml += " value=\"";
			resxml += (*iter_org).name;
			resxml += "\" phone=\"";
			resxml += (*iter_org).phone;
			resxml += "\"/>";
		}

		resxml += "</step";
		resxml += itoaa(i);
		resxml += ">";
	}

	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int searchPlan(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	string str = "";
	char father[5] = {0};
	int sumcount = 0;
	int count = 0;
	int firstIndex = atoi(xx_xml.xpath_getprop("/userdata/head/Request_Index", "value"));
	vector<plan> plans;

	if(getFather(father) == true)
	{
		sql = "SELECT a.id,a.name,b.name,b.details,b.stepcount FROM xxdb_data_plan a, xxdb_data_planstep b WHERE 1=1 ";
		str = xx_xml.xpath_getprop("/userdata/data/planName", "value");
		if(str.compare("") != 0)
		{
			sql += "AND a.name = '";
			sql += str;
			sql += "' ";
		}
		sql += "AND a.father = ";
		sql += father;
		sql += " AND b.father = a.id";

		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			int old_id = 0;
			while ((row = mysql_fetch_row(result)))
			{
				if(old_id != atoi(row[0]))
				{
					old_id = atoi(row[0]);

					plan vPlan;
					vPlan.id = row[0];
					vPlan.name = row[1];

					step vStep;
					vStep.name = row[2];
					vStep.detail = row[3];
					vStep.stepCount = row[4];
					vStep.id = row[5];

					vPlan.steps.push_back(vStep);
					plans.push_back(vPlan);
				}
				else
				{
					step vStep;
					vStep.name = row[2];
					vStep.detail = row[3];
					vStep.stepCount = row[4];

					for(vector<plan>::iterator iter = plans.begin(); iter != plans.end(); iter++)
					{
						if((*iter).id.compare(row[0]) == 0)
						{
							(*iter).steps.push_back(vStep);
							break;
						}
					}
				}
			}
			mysql_free_result(result);
		}
		sumcount = plans.size();
		for(vector<plan>::iterator iter = plans.begin(); iter != plans.end(); iter++)
		{
			int i = 0;
			if((count - firstIndex) > 10) break;
			count++;
			if(((count - firstIndex) > 0)&&((count - firstIndex) <= 10))
			{
				resxml += "<item";
				resxml += itoaa(count - firstIndex);
				resxml += ">";

				resxml += "<planName value=\"";
				resxml += (*iter).name;
				resxml += "\" id=\"";
				resxml += (*iter).id;
				resxml += "\"/>";

				for(vector<step>::iterator iter_step = (*iter).steps.begin(); iter_step != (*iter).steps.end(); iter_step++)
				{
					i++;
					printf("i = %d\n", i);
					if(i > 3) break;
					resxml += "<step";
					resxml += itoaa(i);
					resxml += " value=\"";
					resxml += (*iter_step).name;
					resxml += "\" detail=\"";
					resxml += (*iter_step).detail;
					resxml += "\"/>";
				}

				resxml += "</item";
				resxml += itoaa(count - firstIndex);
				resxml += ">";
			}
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count - firstIndex, sumcount, firstIndex, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int IsUsePlanName(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	char father[5] = {0};
	int ret = 0;

	if(getFather(father) == true)
	{
		sql = "SELECT `name` FROM xxdb_data_plan WHERE `name` = '";
		sql += xx_xml.xpath_getprop("/userdata/data/planName", "value");
		sql += "' AND father = ";
		sql += father;
		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;

			if ((row = mysql_fetch_row(result)))
			{
				ret = 1;
			}
			else
				ret = 0;
			mysql_free_result(result);
		}
		else
			ret = 2;
	}
	else
		ret = 2;

	if(ret == 0) ret = 1;
	else if(ret == 1) ret = 2;
	else ret = 3;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

//addr
int insertAddr(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	char father[5] = {0};
	int ret = 0;

	if(getFather(father) == true)
	{
		sql = "INSERT INTO xxdb_data_addr (`name`, father) VALUES ('";
		sql += xx_xml.xpath_getprop("/userdata/data/addr", "value");
		sql += "',";
		sql += father;
		sql += ")";

		printf("SQL : %s\n", sql.c_str());
		ret = xx_sql_query(m_sql, sql.c_str());
	}
	else
		ret = 1;

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int deleteAddr(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "DELETE FROM xxdb_data_addr WHERE id = ";
	sql += xx_xml.xpath_getprop("/userdata/data/addr", "value");
	printf("SQL : %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int updateAddr(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "UPDATE xxdb_data_addr SET `name` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/addr", "value");
	sql += "' WHERE id =";
	sql += xx_xml.xpath_getprop("/userdata/data/addr", "id");
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int searchAddr(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	char father[5] = {0};
	int count = 0;

	if(getFather(father) == true)
	{
		sql = "SELECT id,`name` FROM xxdb_data_addr WHERE father = ";
		sql += father;
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

				resxml += "<addr value=\"";
				resxml += row[1];
				resxml += "\" id=\"";
				resxml += row[0];
				resxml += "\"/>";

				resxml += "</item";
				resxml += itoaa(count);
				resxml += ">";
			}
			mysql_free_result(result);
		}
	}

	resxml += "</resultdata>";
	setResXML(TYPESTR, count, count, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

//org
int insertOrg(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "INSERT INTO xxdb_data_organization (`name`,addrID,phone1) VALUES ('";
	sql += xx_xml.xpath_getprop("/userdata/data/org", "value");
	sql += "',";
	sql += xx_xml.xpath_getprop("/userdata/data/addr", "value");
	sql += ",'";
	sql += xx_xml.xpath_getprop("/userdata/data/org", "phone");
	sql += "')";

	printf("SQL : %s\n", sql.c_str());
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int deleteOrg(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "DELETE FROM xxdb_data_organization WHERE id = ";
	sql += xx_xml.xpath_getprop("/userdata/data/id", "value");
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int updateOrg(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	int ret = 0;

	sql = "UPDATE xxdb_data_organization SET `name` = '";
	sql += xx_xml.xpath_getprop("/userdata/data/org", "value");
	sql += "', phone1='";
	sql += xx_xml.xpath_getprop("/userdata/data/org", "phone");
	sql += "' WHERE id = ";
	sql += xx_xml.xpath_getprop("/userdata/data/org", "id");
	ret = xx_sql_query(m_sql, sql.c_str());

	if(ret == 0) ret = 1;
	else ret = 2;

	resxml += new_strprop("res", "value", itoaa(ret));
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}

int searchOrg(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata>";
	int count = 0;

	sql = "SELECT id,`name`,phone1 FROM xxdb_data_organization WHERE addrID = ";
	sql += xx_xml.xpath_getprop("/userdata/data/addr", "value");
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

			resxml += "<org value=\"";
			resxml += row[1];
			resxml += "\" id=\"";
			resxml += row[0];
			resxml += "\" phone=\"";
			resxml += row[2];
			resxml += "\"/>";

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

typedef struct
{
	string id;
	string name;
	vector<org> orgs;
}addr;

int getAllOrg(cxx_xml xx_xml, char **outxmldata)
{
	string sql = "";
	string resxml = "<resultdata><item1>";
	char father[5] = {0};
	vector<addr> addrs;
	int count = 0;

	if(getFather(father) == true)
	{
		sql = "SELECT a.id,a.name,b.name,b.id FROM xxdb_data_addr a,xxdb_data_organization b WHERE a.father = ";
		sql += father;
		sql += " AND b.addrID = a.id";

		printf("SQL : %s\n", sql.c_str());
		if(xx_sql_query(m_sql, sql.c_str()) == 0)
		{
			MYSQL_RES *result = mysql_store_result(m_sql);
			MYSQL_ROW row;
			int old_id = 0;
			while ((row = mysql_fetch_row(result)))
			{
				if(old_id != atoi(row[0]))
				{
					old_id = atoi(row[0]);
					addr vAddr;
					vAddr.id = row[0];
					vAddr.name = row[1];

					org vOrg;
					vOrg.name = row[2];
					vOrg.id = row[3];

					vAddr.orgs.push_back(vOrg);
					addrs.push_back(vAddr);
				}
				else
				{
					org vOrg;
					vOrg.name = row[2];
					vOrg.id = row[3];

					for(vector<addr>::iterator iter = addrs.begin(); iter != addrs.end(); iter++)
					{
						if((*iter).id.compare(row[0]) == 0)
						{
							(*iter).orgs.push_back(vOrg);
							break;
						}
					}
				}
			}
			mysql_free_result(result);
		}

		resxml += new_strprop("addrcount", "value", itoaa(addrs.size()));
		for(vector<addr>::iterator iter = addrs.begin(); iter != addrs.end(); iter++)
		{
			int i = 0;
			count++;
			resxml += "<addr";
			resxml += itoaa(count);
			resxml += " value=\"";
			resxml += (*iter).name;
			resxml += "\" orgcount=\"";
			resxml += itoaa((*iter).orgs.size());
			resxml += "\">";

			for(vector<org>::iterator iter_org = (*iter).orgs.begin(); iter_org != (*iter).orgs.end(); iter_org++)
			{
				i++;
				resxml += "<org";
				resxml += itoaa(i);
				resxml += " value=\"";
				resxml += (*iter_org).name;
				resxml += "\" id=\"";
				resxml += (*iter_org).id;
				resxml += "\"/>";
			}

			resxml += "</addr";
			resxml += itoaa(count);
			resxml += ">";
		}
	}
	resxml += "</item1></resultdata>";
	setResXML(TYPESTR, 1, 1, 0, resxml, gresXML);
	*outxmldata = (char *)gresXML.c_str();
	printf("resXML = %s\n", *outxmldata);

	return 0;
}
