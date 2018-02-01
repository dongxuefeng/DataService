#ifndef _XXDATABASE_H_
#define _XXDATABASE_H_

#pragma once

#include <stdio.h>
//#include <my_global.h>
#include <string.h>
#include <string>
#include <mysql.h>
#include "DataType.h"
#include <list>

#define ok				0
#define timeout			-1
#define sqlerror		-2
#define other			-3

using namespace std;

class XXDataBase
{
public:
	XXDataBase();
	~XXDataBase();
	int getLaseError();
	int CreateDataBase();

	int InsertData(pviodata vd);		//insert illegal data
	int InsertData(pgpsdata gd);		//insert GPS data
	int InsertData(pdevlog dl);			//insert log of client device
	int InsertData(pdevconnlog dc);		//insert data of client connected info
	int	InsertData(pintercept intercept);	//insert the car info because they was in blacklist
	int InsertData(pserverlog sl);		//insert this server's log
	int InsertData(pblacklist bl);		//insert the blacklist info
	int InsertData(pblacklisttype blt);	//insert the type of blacklist
	int InsertData(pdeviceInfo di);		//insert the device info
	int InsertData(pdeviceuser du);		//insert the man's info who used the device
	int	InsertData(pdevlogtype dt);		//insert the type of log
	int InsertData(pPlatetype pt);		//insert the type of plate
	int InsertData(puser user);			//insert the users' info
	int InsertData(pusergroup ug);		//insert the group of users
	int InsertData(pviotype vy);		//insert the type of viodata
	/************************************************************************/
	/* 
		�������ݣ����Ϊ��صĽṹ�塣
		�ṹ����ID�ֶα�ʾҪ���µ�����Ψһ��ʶ�������ֶβ�Ϊnull��Ϊ����ʱ��Ч*/
	/************************************************************************/
	int UpDate(pviodata vd);		//update illegal data
	int UpDate(pgpsdata gd);		//update GPS data
	int UpDate(pdevlog dl);			//update log of client device
	int UpDate(pdevconnlog dc);		//update data of client connected info
	int	UpDate(pintercept intercept);	//update the car info because they was in blacklist
	int UpDate(pserverlog sl);		//update this server's log
	int UpDate(pblacklist bl);		//update the blacklist info
	int UpDate(pblacklisttype blt);	//update the type of blacklist
	int UpDate(pdeviceInfo di);		//update the device info
	int UpDate(pdeviceuser du);		//update the man's info who used the device
	int	UpDate(pdevlogtype dt);		//update the type of log
	int UpDate(pPlatetype pt);		//update the type of plate
	int UpDate(puser user);			//update the users' info
	int UpDate(pusergroup ug);		//update the group of users
	int UpDate(pviotype vy);		//update the type of viodata
	/************************************************************************/
	/* 
		��IDɾ�������ݣ����ṩ����ɾ�����ܣ�������Ҫ��������*/
	/************************************************************************/
	int DeleteVioData(int id);
	int DeleteGpsData(int id);
	int DeleteDevLog(int id);
	int DeleteDevconnLog(int id);
	int DeleteIntercept(int id);
	int DeleteServerLog(int id);
	int DeleteBlackList(int id);
	int DeleteBlackListType(int id);
	int DeleteDeviceInfo(int id);
	int DeleteDeviceuser(int id);
	int DeleteDevLogType(int id);
	int DeletePlateType(int id);
	int DeleteUser(int id);
	int DeleteGroup(int id);
	int DeleteVioType(int id);

	//���·��ؽ���ڴ��Ѿ����룬ʹ���������ͷ�

	/*
		��ȡΥ����Ϣ
		id ����0����ID��������
		index, count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		stime,etime ��ʱ�䷶Χ��ѯ����Ϊnull��Ч
		platenum �����Ʋ�ѯ��֧��ģ����ѯ����Ϊnull��Ч
		platetype ���������Ͳ�ѯ���Ǹ�����Ч
		userid,devid���û����豸��ѯ���Ǹ�����Ч
		����viodata����list
	*/
	list<viodata> *GetViodata(int id, int index, int count, char *stime, char *etime, char *platenum, int platetype,
		int user, int devid, int viotype);
	/************************************************************************/
	/* 
		��ȡgps��Ϣ
		id����id�����������Ǹ�����Ч
		index,count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		userid ���û��������Ǹ�����Ч
		devid ���豸�������Ǹ�����Ч
		stime, etime��ʱ�䷶Χ����Ϊnull��Ч
		����gpsdata����list*/
	/************************************************************************/
	list<gpsdata> *GetGpsData(int id, int index, int count, int userid, int devid, char *stime, char *etime);
	/************************************************************************/
	/*	
		��ȡ�豸��־
		id,��id�����������Ǹ�����Ч
		index,count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		logtype������־�����������Ǹ�����Ч
		userid��devid�����û����豸�������Ǹ�����Ч
		stime, etime����ѯʱ�䷶Χ����Ϊnull��Ч*/
	/************************************************************************/
	list<devlog> *GetDevLogData(int id, int index, int count, int logtype, int userid, int devid, char *stime, char *etime);
	/************************************************************************/
	/* 
		��ȡ�豸������־
		id,��id�����������Ǹ�����Ч
		index,count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		userid, devid���û����豸�������Ǹ�����Ч
		stime, etime*/
	/************************************************************************/
	list<devconnlog> *GetDevconnLog(int id, int index, int count, int userid, int devid, char *stime, char *etime);
	/************************************************************************/
	/* 
		��ȡ������Ϣ
		id ����0����ID��������
		index, count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		stime,etime ��ʱ�䷶Χ��ѯ����Ϊnull��Ч
		platenum �����Ʋ�ѯ��֧��ģ����ѯ����Ϊnull��Ч
		platetype ���������Ͳ�ѯ���Ǹ�����Ч
		userid,devid���û����豸��ѯ���Ǹ�����Ч
		bltype,����������������
		����intercept����list*/
	/************************************************************************/
	list<intercept> *GetInterceptDate(int id, int index, int count, char *stime, char *etime, char *platenum, int platetype,
		int user, int devid, int bltype);
	/************************************************************************/
	/* 
		��ȡϵͳ������־
		id ����0����ID��������
		index, count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		stime,etime ��ʱ�䷶Χ��ѯ����Ϊnull��Ч
		logtype����־�����������Ǹ�����Ч*/
	/************************************************************************/
	list<serverlog> *GetServerLog(int id, int index, int count, char *stime, char *etime, int logtype);
	/************************************************************************/
	/* 
		��ȡ������
		id����0��id��������
		index, count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		platenum��������������null��Ч
		platetype������������Ч���Ǹ�����Ч
		blacklisttype�������������������Ǹ�����Ч
		stime, etime�����ʱ����������null��Ч*/
	/************************************************************************/
	list<blacklist> *GetBlacklist(int id, int index, int count, char *platenum, int platetype, 
		int blacklisttype, int stime, int etime);
	/************************************************************************/
	/* 
		��ȡ����������
		id��id�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч*/
	/************************************************************************/
	list<blacklisttype> *GetBlacklistType(int id, int index, int count);
	/************************************************************************/
	/* 
		��ȡ�豸��Ϣ
		id��id�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼid��ÿҳ��Ŀ���Ǹ�����Ч
		devid���豸�����������null��Ч
		regcode,���豸��Ȩ��������null��Ч
		authcode������ʶ����Ȩ����������null��Ч*/
	/************************************************************************/
	list<deviceInfo> *GetDeviceInfo(int id, int index, int count, char *devid, char *regcode, 
		char *authcode, char *stime, char *etime);
	/************************************************************************/
	/* 
		��ȡ�豸ʹ������Ϣ
		id,��id�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼid��ÿҳ��Ŀ���Ǹ�����Ч
		username���û�����������null��Ч
		name��������������nNULL��Ч
		workcode��������������null��Ч
		devid��ʹ���豸��������null��Ч*/
	/************************************************************************/
	list<deviceuser> *GetDevuser(int id, int index, int count, char *username, char *name, char *workcode, 
		int devid);
	/************************************************************************/
	/* 
		��ȡ�豸��־����
		id,��ID�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼid��ÿҳ��Ŀ���Ǹ�����Ч
		logtypeid,��־���ͱ���������Ǹ�����Ч*/
	/************************************************************************/
	list<devlogtype> *GetDevLogType(int id, int index, int count, int logtypeid);
	/************************************************************************/
	/* 
		��ȡ��������
		id,��ID�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼid��ÿҳ��Ŀ���Ǹ�����Ч
		platetypeid���������ͱ�ţ��Ǹ�����Ч
		*/
	/************************************************************************/
	list<platetype> *GetPlateType(int id, int index, int count, int platetypeid);
	/************************************************************************/
	/* 
		id��ID�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼid��ÿҳ��Ŀ���Ǹ�����Ч
		usrename�����û�����������Ϊnull��Ч
		group�����û����������Ǹ�����Ч*/
	/************************************************************************/
	list<user> *GetUserInfo(int id, int index, int count, char *username, int group);

	list<usergroup> *GetUserGroup(int id, int index, int count, int power);
	list<viotype> *GetVioType(int id, int index, int count, int viotypeid);
private:
	MYSQL *mysql;
	int lasterror;
	int ConnectDB(char *host, int port, char *user, char *pwd);
	int DisConnectDB();
	char *itoa(int ivalue);
	int execuSQL(string sql);
};

#endif
