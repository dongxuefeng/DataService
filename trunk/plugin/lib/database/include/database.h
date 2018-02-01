#ifndef DATABASE_H_
#define DATABASE_H_
#include "XXDataBase.h"
extern"C"
{
	int InitDB(XXDataBase **xxdb);
	int FInitDB(XXDataBase *xxdb);

	int getLaseError(XXDataBase *xxdb);
	int CreateDataBase(XXDataBase *xxdb);

	int InsertVioData(pviodata vd, XXDataBase *xxdb);
	int InsertGpsData(pgpsdata gd, XXDataBase *xxdb);
	int InsertDevLogData(pdevlog dl, XXDataBase *xxdb);
	int InsertDevConnData(pdevconnlog dc, XXDataBase *xxdb);
	int	InsertInterceptData(pintercept intercept, XXDataBase *xxdb);
	int InsertServerLogData(pserverlog sl, XXDataBase *xxdb);
	int InsertBlackListData(pblacklist bl, XXDataBase *xxdb);
	int InsertBlaclListTypeData(pblacklisttype blt, XXDataBase *xxdb);
	int InsertDeviceInfoData(pdeviceInfo di, XXDataBase *xxdb);
	int InsertDeviceUserData(pdeviceuser du, XXDataBase *xxdb);
	int	InsertDevLogTypeData(pdevlogtype dt, XXDataBase *xxdb);
	int InsertPlateTypeData(pPlatetype pt, XXDataBase *xxdb);		//insert the type of plate
	int InsertUserData(puser user, XXDataBase *xxdb);			//insert the users' info
	int InsertUserGroupData(pusergroup ug, XXDataBase *xxdb);		//insert the group of users
	int InsertVioTypeData(pviotype vy, XXDataBase *xxdb);		//insert the type of viodata

		/************************************************************************/
	/* 
		更新内容，入参为相关的结构体。
		结构体中ID字段表示要更新的数据唯一标识，其他字段不为null或不为负数时生效*/
	/************************************************************************/
	int UpDateVioData(pviodata vd, XXDataBase *xxdb);		//update illegal data
	int UpDateGpsData(pgpsdata gd, XXDataBase *xxdb);		//update GPS data
	int UpDateDevLog(pdevlog dl, XXDataBase *xxdb);			//update log of client device
	int UpDateDevConnLog(pdevconnlog dc, XXDataBase *xxdb);		//update data of client connected info
	int	UpDateIntercept(pintercept intercept, XXDataBase *xxdb);	//update the car info because they was in blacklist
	int UpDateServerLog(pserverlog sl, XXDataBase *xxdb);		//update this server's log
	int UpDateBlackList(pblacklist bl, XXDataBase *xxdb);		//update the blacklist info
	int UpDateBlackListType(pblacklisttype blt, XXDataBase *xxdb);	//update the type of blacklist
	int UpDateDevInfo(pdeviceInfo di, XXDataBase *xxdb);		//update the device info
	int UpDateDevUser(pdeviceuser du, XXDataBase *xxdb);		//update the man's info who used the device
	int	UpDateDevLogType(pdevlogtype dt, XXDataBase *xxdb);		//update the type of log
	int UpDatePlateType(pPlatetype pt, XXDataBase *xxdb);		//update the type of plate
	int UpDateUser(puser user, XXDataBase *xxdb);			//update the users' info
	int UpDateUserGroup(pusergroup ug, XXDataBase *xxdb);		//update the group of users
	int UpDateVioType(pviotype vy, XXDataBase *xxdb);		//update the type of viodata
	/************************************************************************/
	/* 
		按ID删除表数据，不提供批量删除功能，如有需要后期增加*/
	/************************************************************************/
	int DeleteVioData(int id, XXDataBase *xxdb);
	int DeleteGpsData(int id, XXDataBase *xxdb);
	int DeleteDevLog(int id, XXDataBase *xxdb);
	int DeleteDevconnLog(int id, XXDataBase *xxdb);
	int DeleteIntercept(int id, XXDataBase *xxdb);
	int DeleteServerLog(int id, XXDataBase *xxdb);
	int DeleteBlackList(int id, XXDataBase *xxdb);
	int DeleteBlackListType(int id, XXDataBase *xxdb);
	int DeleteDeviceInfo(int id, XXDataBase *xxdb);
	int DeleteDeviceuser(int id, XXDataBase *xxdb);
	int DeleteDevLogType(int id, XXDataBase *xxdb);
	int DeletePlateType(int id, XXDataBase *xxdb);
	int DeleteUser(int id, XXDataBase *xxdb);
	int DeleteGroup(int id, XXDataBase *xxdb);
	int DeleteVioType(int id, XXDataBase *xxdb);

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
		int user, int devid, int viotype, XXDataBase *xxdb);
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
	list<gpsdata> *GetGpsData(int id, int index, int count, int userid, int devid, char *stime, char *etime, XXDataBase *xxdb);
	/************************************************************************/
	/*	
		获取设备日志
		id,按id进行索引，非负数生效
		index,count分页查询的起始ID和每页数目，非负数生效
		logtype，按日志类型索引，非负数生效
		userid，devid，按用户和设备索引，非负数生效
		stime, etime，查询时间范围，不为null生效*/
	/************************************************************************/
	list<devlog> *GetDevLogData(int id, int index, int count, int logtype, int userid, int devid, char *stime, char *etime, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		获取设备连接日志
		id,按id进行索引，非负数生效
		index,count分页查询的起始ID和每页数目，非负数生效
		userid, devid按用户或设备索引，非负数生效
		stime, etime*/
	/************************************************************************/
	list<devconnlog> *GetDevconnLog(int id, int index, int count, int userid, int devid, char *stime, char *etime, XXDataBase *xxdb);
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
		int user, int devid, int bltype, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		获取系统运行日志
		id 大于0，按ID进行索引
		index, count分页查询，起始ID和每页数目，非负数生效
		stime,etime 按时间范围查询，不为null生效
		logtype按日志类型索引，非负数生效*/
	/************************************************************************/
	list<serverlog> *GetServerLog(int id, int index, int count, char *stime, char *etime, int logtype, XXDataBase *xxdb);
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
		int blacklisttype, int stime, int etime, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		获取黑名单类型
		id按id索引，非负数生效
		index, count分页查询，超始ID和每页数目，非负数生效*/
	/************************************************************************/
	list<blacklisttype> *GetBlacklistType(int id, int index, int count, XXDataBase *xxdb);
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
		char *authcode, char *stime, char *etime, XXDataBase *xxdb);
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
		int devid, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		获取设备日志类型
		id,按ID索引，非负数生效
		index, count分页查询，超始id和每页数目，非负数生效
		logtypeid,日志类型编号索引，非负数生效*/
	/************************************************************************/
	list<devlogtype> *GetDevLogType(int id, int index, int count, int logtypeid, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		获取车牌类型
		id,按ID索引，非负数生效
		index, count分页查询，超始id和每页数目，非负数生效
		platetypeid，车牌类型编号，非负数生效
		*/
	/************************************************************************/
	list<platetype> *GetPlateType(int id, int index, int count, int platetypeid, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		id按ID索引，非负数生效
		index, count分页查询，超始id和每页数目，非负数生效
		usrename，按用户名索引，不为null生效
		group，按用户组索引，非负数生效*/
	/************************************************************************/
	list<user> *GetUserInfo(int id, int index, int count, char *username, int group, XXDataBase *xxdb);

	list<usergroup> *GetUserGroup(int id, int index, int count, int power, XXDataBase *xxdb);
	list<viotype> *GetVioType(int id, int index, int count, int viotypeid, XXDataBase *xxdb);
};

#endif
