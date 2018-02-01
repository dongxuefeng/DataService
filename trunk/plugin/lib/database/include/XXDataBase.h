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
		更新内容，入参为相关的结构体。
		结构体中ID字段表示要更新的数据唯一标识，其他字段不为null或不为负数时生效*/
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
		按ID删除表数据，不提供批量删除功能，如有需要后期增加*/
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

	//以下返回结果内存已经申请，使用完后务必释放

	/*
		获取违法信息
		id 大于0，按ID进行索引
		index, count分页查询，起始ID和每页数目，非负数生效
		stime,etime 按时间范围查询，不为null生效
		platenum 按车牌查询，支持模糊查询，不为null生效
		platetype 按车牌类型查询，非负数生效
		userid,devid按用户和设备查询，非负数生效
		返回viodata类型list
	*/
	list<viodata> *GetViodata(int id, int index, int count, char *stime, char *etime, char *platenum, int platetype,
		int user, int devid, int viotype);
	/************************************************************************/
	/* 
		获取gps信息
		id，按id进行索引，非负数生效
		index,count分页查询的起始ID和每页数目，非负数生效
		userid 按用户索引，非负数生效
		devid 按设备索引，非负数生效
		stime, etime，时间范围，不为null生效
		返回gpsdata类型list*/
	/************************************************************************/
	list<gpsdata> *GetGpsData(int id, int index, int count, int userid, int devid, char *stime, char *etime);
	/************************************************************************/
	/*	
		获取设备日志
		id,按id进行索引，非负数生效
		index,count分页查询的起始ID和每页数目，非负数生效
		logtype，按日志类型索引，非负数生效
		userid，devid，按用户和设备索引，非负数生效
		stime, etime，查询时间范围，不为null生效*/
	/************************************************************************/
	list<devlog> *GetDevLogData(int id, int index, int count, int logtype, int userid, int devid, char *stime, char *etime);
	/************************************************************************/
	/* 
		获取设备连接日志
		id,按id进行索引，非负数生效
		index,count分页查询的起始ID和每页数目，非负数生效
		userid, devid按用户或设备索引，非负数生效
		stime, etime*/
	/************************************************************************/
	list<devconnlog> *GetDevconnLog(int id, int index, int count, int userid, int devid, char *stime, char *etime);
	/************************************************************************/
	/* 
		获取拦截信息
		id 大于0，按ID进行索引
		index, count分页查询，起始ID和每页数目，非负数生效
		stime,etime 按时间范围查询，不为null生效
		platenum 按车牌查询，支持模糊查询，不为null生效
		platetype 按车牌类型查询，非负数生效
		userid,devid按用户和设备查询，非负数生效
		bltype,按黑名单类型索引
		返回intercept类型list*/
	/************************************************************************/
	list<intercept> *GetInterceptDate(int id, int index, int count, char *stime, char *etime, char *platenum, int platetype,
		int user, int devid, int bltype);
	/************************************************************************/
	/* 
		获取系统运行日志
		id 大于0，按ID进行索引
		index, count分页查询，起始ID和每页数目，非负数生效
		stime,etime 按时间范围查询，不为null生效
		logtype按日志类型索引，非负数生效*/
	/************************************************************************/
	list<serverlog> *GetServerLog(int id, int index, int count, char *stime, char *etime, int logtype);
	/************************************************************************/
	/* 
		获取黑名单
		id大于0按id进行索引
		index, count分页查询，起始ID和每页数目，非负数生效
		platenum按车牌索引，非null生效
		platetype按车牌类型生效，非负数生效
		blacklisttype按黑名单类型索引，非负数生效
		stime, etime按添加时间索引，非null生效*/
	/************************************************************************/
	list<blacklist> *GetBlacklist(int id, int index, int count, char *platenum, int platetype, 
		int blacklisttype, int stime, int etime);
	/************************************************************************/
	/* 
		获取黑名单类型
		id按id索引，非负数生效
		index, count分页查询，超始ID和每页数目，非负数生效*/
	/************************************************************************/
	list<blacklisttype> *GetBlacklistType(int id, int index, int count);
	/************************************************************************/
	/* 
		获取设备信息
		id按id索引，非负数生效
		index, count分页查询，超始id和每页数目，非负数生效
		devid按设备编号索引，非null生效
		regcode,按设备授权索引，非null生效
		authcode按车牌识别授权号索引，非null生效*/
	/************************************************************************/
	list<deviceInfo> *GetDeviceInfo(int id, int index, int count, char *devid, char *regcode, 
		char *authcode, char *stime, char *etime);
	/************************************************************************/
	/* 
		获取设备使用者信息
		id,按id索引，非负数生效
		index, count分页查询，超始id和每页数目，非负数生效
		username，用户名索引，非null生效
		name，姓名索引，非nNULL生效
		workcode，工号索引，非null生效
		devid，使用设备索引，非null生效*/
	/************************************************************************/
	list<deviceuser> *GetDevuser(int id, int index, int count, char *username, char *name, char *workcode, 
		int devid);
	/************************************************************************/
	/* 
		获取设备日志类型
		id,按ID索引，非负数生效
		index, count分页查询，超始id和每页数目，非负数生效
		logtypeid,日志类型编号索引，非负数生效*/
	/************************************************************************/
	list<devlogtype> *GetDevLogType(int id, int index, int count, int logtypeid);
	/************************************************************************/
	/* 
		获取车牌类型
		id,按ID索引，非负数生效
		index, count分页查询，超始id和每页数目，非负数生效
		platetypeid，车牌类型编号，非负数生效
		*/
	/************************************************************************/
	list<platetype> *GetPlateType(int id, int index, int count, int platetypeid);
	/************************************************************************/
	/* 
		id按ID索引，非负数生效
		index, count分页查询，超始id和每页数目，非负数生效
		usrename，按用户名索引，不为null生效
		group，按用户组索引，非负数生效*/
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
