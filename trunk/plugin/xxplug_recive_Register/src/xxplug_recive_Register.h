#ifndef _XXPLUG_RECIVE_GPS_
#define _XXPLUG_RECIVE_GPS_

extern "C"
{
	__declspec(dllexport) int xx_rcv_gettype();
	__declspec(dllexport) int xx_rcv_procinfo(char * inxmldata,char ** outxmldata);
}

#include <cxx_xml.h>

#define TYPE 18
#define TYPESTR "0018"

//phone
int PhoneRegister(cxx_xml xx_xml, char ** outxmldata);//手机注册

//client
int GetNewRegisterCode(cxx_xml xx_xml, char ** outxmldata);//获取新验证码
int GetOldRegisterCode(cxx_xml xx_xml, char ** outxmldata);//获取旧验证码的使用情况

#endif
