#ifndef _WEBCLIENT_H_
#define _WEBCLIENT_H_

void * xxWebInfoInit(void ** arg);
int xxGetCarInfo(void * arg, char *devid, char *platenum, char *platetype, char *Verify, char *outxml);
int xxGetVerify(void * arg, char* devid, char * picdata, int & picsize);
void xxDestroy(void * arg);
void xxCancelSearch(void * arg, char * devid);

#endif
