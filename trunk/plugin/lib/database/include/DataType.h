#ifndef _DATATYPE_H_
#define _DATATYPE_H_

typedef struct viodata				//illegal data 
{									//include all the field what from client
	int id;
	char datetime[32];
	char roadname[32];
	char platenum[12];
	int platetype;
	int picnum;
	char picurl0[256];
	char picurl1[256];
	char picurl2[256];
	char picurl3[256];
	int viotype;
	int device;
	int userid;
	int affirm;
	int download;
} viodata, *pviodata;

typedef struct gpsdata					//GPS data
{										//include all the field what from client
	int id;
	char datetime[32];
	char provider[18];
	char latitude[18];
	char longitude[18];
	char altitude[18];
	char bearing[18];
	int device;
	int userid;
	char jobs[256];
} gpsdata, *pgpsdata;

typedef struct devlog					//the log of client device
{										//include all the field what from the client
	int id;
	char datetime[32];
	char classname[18];
	int logtype;
	char manipuliation[256];
	int device;
	int userid;
} devlog, *pdevlog;

typedef struct devconnlog				//the log of client device what describe the client device connected log 
{
	int id;
	char datetime[32];
	int device;
	int userid;
	char event[256];
} devconnlog, *pdevconnlog;

typedef struct intercept				//all the field describe the client who got the car because its info was in the blacklist
{
	int id;
	char datetime[32];
	int picnum;
	char picurl0[256];
	char picurl1[256];
	char picurl2[256];
	char picurl3[256];
	char platenum[12];
	int platetype;
	char roadname[128];
	int bltype;
	int device;
	int userid;
} intercept, *pintercept;

typedef struct serverlog				//this server's log
{
	int id;
	char datetime[32];
	int logtype;
	char file[18];
	char describe[255];
} serverlog, *pserverlog;

typedef struct blacklist				//the blacklist,include some of car is illegal
{
	int  id;
	char platenum[18];
	int  platetype;
	int  blacklisttype;
	int  timestamp;
	char blt[64];
	char carcolor[4];
	char brand[10];
	char owner[10];
	int  altertype;
	//char addtime[32];
} blacklist, *pblacklist;

typedef struct blacklisttype			//the type of illegal
{
	int id;
	char blisttype[128];
} blacklisttype, *pblacklisttype;

typedef struct deviceInfo				//the client device info
{
	int id;
	char deviceid[32];
	char regcode[32];
	char authcode[32];
	char regdate[32];
} deviceInfo, *pdeviceInfo;

typedef struct deviceuser				//the man's info who use the device
{
	int id;
	char username[18];
	char password[18];
	char name[10];
	char sex[10];
	char department[32];
	char workcode[10];
	char regdate[32];
	int devid;
	char getdate[32];
} deviceuser, *pdeviceuser;

typedef struct devlogtype				//the type of device log
{
	int id;
	int logtypeid;
	char logtype[32];
} devlogtype, *pdevlogtype;

typedef struct platetype				//the type of plate
{
	int id;
	int platetypeid;
	char platetypechar[32];
} platetype, *pPlatetype;

typedef struct user					//the users' info
{
	int id;
	char username[10];
	char password[10];
	char regdate[32];
	int group;
} user, *puser;

typedef struct usergroup			//the group of users who was belong to
{
	int id;
	char group[10];
	int power;
} usergroup, *pusergroup;

typedef struct viotype
{
	int id;
	int viotypeid;
	char viotypechar[32];
} viotype, *pviotype;
#endif	
