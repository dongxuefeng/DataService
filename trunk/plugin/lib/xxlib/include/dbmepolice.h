/*
 * dbmepolice.h
 *
 *  Created on: 2014-3-17
 *      Author: Administrator
 */

#ifndef DBMEPOLICE_H_
#define DBMEPOLICE_H_

#include <mysql.h>
#include <stdio.h>
#include <xxWriteLog.h>
#define sqlfile "sqllogtest.log"
#include <xxxml.h>



inline int connectMySQL(MYSQL *&m_sql,char * ip = (char *)"127.0.0.1", int port = 3306, char * usrName = (char *)"root", char * password = (char *)"xx123xx", char * DBname = (char *)"xxdatabase")
{
	char strip[256] = "localhost";
	FILE * fp = fopen("dbconfig.ini","r");
	if(fp)
	{
		fread(strip,1,256,fp);
		fclose(fp);
	}
	ip = strip;

	printf("IP = %s\n", ip);

	if(m_sql != 0)
		return 0;
	m_sql = (MYSQL *)malloc(sizeof(MYSQL));

	//在某些版本中，不需要该初始化工作，可观看mysql.H以及readme
	mysql_init(m_sql);

	if(m_sql == NULL)
	{
		printf("EROR: MySQL ssock init error. \n");
		return -1;
	}
	else
		printf("MySQL init OK. \n");

	mysql_options(m_sql, MYSQL_OPT_RECONNECT, "1");	//设定自动重连
	//连接到指定的数据库
	m_sql = mysql_real_connect(m_sql, ip, usrName, password, NULL
			, port, NULL, CLIENT_INTERACTIVE);
	if(!(m_sql))
	{
		printf("conn fail... \n");
		return -1;
	}
	else
		printf("MySQL Connect Server OK... \n");

	if(mysql_select_db(m_sql, DBname) != 0)
	{
		printf("select db error. \n");
		return -1;
	}
	else
		printf("select db OK. \n");

	return 0;
}

inline void disconnMySQL(MYSQL *m_sql)
{
	if(m_sql != 0)
		mysql_close(m_sql);
	m_sql = 0;
}

inline int xx_sql_real_query(MYSQL *m_sql, const char *q,
		unsigned long length)
{
	char * filename = (char *)sqlfile;
	int iresult =  mysql_real_query(m_sql, q, length);
	if(!iresult)
	{
		char strtmp[1024];
		sprintf(strtmp,"%s\n%s\n\n",
		mysql_error(m_sql),q);
		write_log(filename,strtmp);
	}

	return iresult;
}

inline int	xx_sql_query(MYSQL *m_sql, const char *q)
{
	string strtmp;
	char * filename= (char *)sqlfile;

Retry:
	strtmp = q;
	write_log((const char *)filename,(const char *)strtmp.c_str());
	//printf("SQL : %s\n", q);

	int iresult =  mysql_query(m_sql, q);
	if(iresult)
	{
		strtmp = mysql_error(m_sql);
		strtmp += "\n";
		write_log(filename,strtmp.c_str());

		printf("ERROR: %s\n", mysql_error(m_sql));

		if(strstr(mysql_error(m_sql), "MySQL server has gone away") != NULL)
		{
			if(mysql_ping(m_sql) == 0)
			{
				printf("-----------Reconnect MYSQL!-------------\n");
			}
			else
			{
				printf("-----------Reconnect error!-------------\n");
			}

			Sleep(500);
			goto Retry;
		}
	}

	return iresult;
}


inline int xx_sql_fondrows(MYSQL *m_sql)
{
	int ifound = -1;

	if(xx_sql_query(m_sql,"SELECT FOUND_ROWS()") != 0)
	{
		MYSQL_RES *result = mysql_store_result(m_sql);
		MYSQL_ROW row;
		if((row = mysql_fetch_row(result)))
			 ifound = atoi(row[0]);
		mysql_free_result(result);
	}
	return ifound;
}

#endif /* DBMEPOLICE_H_ */
