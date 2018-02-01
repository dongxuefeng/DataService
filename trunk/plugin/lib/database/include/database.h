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
		�������ݣ����Ϊ��صĽṹ�塣
		�ṹ����ID�ֶα�ʾҪ���µ�����Ψһ��ʶ�������ֶβ�Ϊnull��Ϊ����ʱ��Ч*/
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
		��IDɾ�������ݣ����ṩ����ɾ�����ܣ�������Ҫ��������*/
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
		int user, int devid, int viotype, XXDataBase *xxdb);
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
	list<gpsdata> *GetGpsData(int id, int index, int count, int userid, int devid, char *stime, char *etime, XXDataBase *xxdb);
	/************************************************************************/
	/*	
		��ȡ�豸��־
		id,��id�����������Ǹ�����Ч
		index,count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		logtype������־�����������Ǹ�����Ч
		userid��devid�����û����豸�������Ǹ�����Ч
		stime, etime����ѯʱ�䷶Χ����Ϊnull��Ч*/
	/************************************************************************/
	list<devlog> *GetDevLogData(int id, int index, int count, int logtype, int userid, int devid, char *stime, char *etime, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		��ȡ�豸������־
		id,��id�����������Ǹ�����Ч
		index,count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		userid, devid���û����豸�������Ǹ�����Ч
		stime, etime*/
	/************************************************************************/
	list<devconnlog> *GetDevconnLog(int id, int index, int count, int userid, int devid, char *stime, char *etime, XXDataBase *xxdb);
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
		int user, int devid, int bltype, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		��ȡϵͳ������־
		id ����0����ID��������
		index, count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч
		stime,etime ��ʱ�䷶Χ��ѯ����Ϊnull��Ч
		logtype����־�����������Ǹ�����Ч*/
	/************************************************************************/
	list<serverlog> *GetServerLog(int id, int index, int count, char *stime, char *etime, int logtype, XXDataBase *xxdb);
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
		int blacklisttype, int stime, int etime, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		��ȡ����������
		id��id�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼID��ÿҳ��Ŀ���Ǹ�����Ч*/
	/************************************************************************/
	list<blacklisttype> *GetBlacklistType(int id, int index, int count, XXDataBase *xxdb);
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
		char *authcode, char *stime, char *etime, XXDataBase *xxdb);
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
		int devid, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		��ȡ�豸��־����
		id,��ID�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼid��ÿҳ��Ŀ���Ǹ�����Ч
		logtypeid,��־���ͱ���������Ǹ�����Ч*/
	/************************************************************************/
	list<devlogtype> *GetDevLogType(int id, int index, int count, int logtypeid, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		��ȡ��������
		id,��ID�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼid��ÿҳ��Ŀ���Ǹ�����Ч
		platetypeid���������ͱ�ţ��Ǹ�����Ч
		*/
	/************************************************************************/
	list<platetype> *GetPlateType(int id, int index, int count, int platetypeid, XXDataBase *xxdb);
	/************************************************************************/
	/* 
		id��ID�������Ǹ�����Ч
		index, count��ҳ��ѯ����ʼid��ÿҳ��Ŀ���Ǹ�����Ч
		usrename�����û�����������Ϊnull��Ч
		group�����û����������Ǹ�����Ч*/
	/************************************************************************/
	list<user> *GetUserInfo(int id, int index, int count, char *username, int group, XXDataBase *xxdb);

	list<usergroup> *GetUserGroup(int id, int index, int count, int power, XXDataBase *xxdb);
	list<viotype> *GetVioType(int id, int index, int count, int viotypeid, XXDataBase *xxdb);
};

#endif
