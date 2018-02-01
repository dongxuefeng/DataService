#ifndef _GETINFO_H_
#define _GETINFO_H_

void InitSoap();
void DestroySoap();
int getInfo(int type, char * platetype, char * platenum, char * driverlicence, string & res);

#endif
