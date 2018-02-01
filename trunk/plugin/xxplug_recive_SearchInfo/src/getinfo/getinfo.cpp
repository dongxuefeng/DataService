#include "soapH.h" 
#include "TmriOutAccessSoapBinding.nsmap"
#include "../chinese_code.h"
#include <iostream>
#include <windows.h>
using namespace std;

char Dec2HexChar(short int n) 
{
    if ( 0 <= n && n <= 9 ) {
        return char( short('0') + n );
    } else if ( 10 <= n && n <= 15 ) {
        return char( short('A') + n - 10 );
    } else {
        return char(0);
    }
}

short int HexChar2Dec(char c)
{
    if ( '0'<=c && c<='9' ) {
        return short(c-'0');
    } else if ( 'a'<=c && c<='f' ) {
        return ( short(c-'a') + 10 );
    } else if ( 'A'<=c && c<='F' ) {
        return ( short(c-'A') + 10 );
    } else {
        return -1;
    }
}

char * EncodeURL(const char *Str)
{
 wchar_t *Bufw = NULL;
 char *Bufc = NULL;
 char RTV[5120];
 long needSize = MultiByteToWideChar(CP_ACP,
    NULL,
    Str,
    -1,
    NULL,
    0);
 Bufw = new wchar_t[needSize];
 if ( NULL == Bufw )
 {
    return "";
 }
 
 memset(Bufw,0x0,needSize*2);
 MultiByteToWideChar(CP_ACP,
    NULL,
    Str,
    -1,
    Bufw,
    needSize);
 
 needSize = WideCharToMultiByte(CP_UTF8,
    NULL,
    Bufw,
    -1,
    NULL,
    0,
    NULL,
    NULL);
 if ( 0 == needSize )
 {
    if ( NULL != Bufw )
    {
     delete [] Bufw;
     Bufw = NULL;
    }
    if ( NULL != Bufc )
    {
     delete [] Bufc;
     Bufc = NULL;
    }
 }
 
 Bufc = new char[needSize];
 if ( NULL == Bufc )
 {
    if ( NULL != Bufw )
    {
     delete [] Bufw;
     Bufw = NULL;
    }
    if ( NULL != Bufc )
    {
     delete [] Bufc;
     Bufc = NULL;
    }
 }
 
 memset(Bufc,0x0,needSize);
 WideCharToMultiByte(CP_UTF8,
    NULL,
    Bufw,
    -1,
    Bufc,
    needSize,
    NULL,
    NULL);
 
 unsigned char *pWork = (unsigned char *)Bufc;
 memset(RTV,0x0,sizeof(RTV));
 if ( strlen(Bufc) > 5120 )
 {
    if ( NULL != Bufw )
    {
     delete [] Bufw;
     Bufw = NULL;
    }
    if ( NULL != Bufc )
    {
     delete [] Bufc;
     Bufc = NULL;
    }
 }
 while( *pWork != 0x0 )
 {
    if ( *pWork != '!' && *pWork != '@' && *pWork != '#' &&
     *pWork != '$' && *pWork != '&' && *pWork != '*' &&
     *pWork != '(' && *pWork != ')' && *pWork != '=' &&
     *pWork != ':' && *pWork != '/' && *pWork != ';' &&
     *pWork != '?' && *pWork != '+' && *pWork != '\'' &&
     *pWork != '.' )
    {
     sprintf(RTV+strlen(RTV),"%%%2X",*pWork);
    }
    else
    {
     sprintf(RTV+strlen(RTV),"%c",*pWork);
    }
    pWork++;
 }
 
 if ( NULL != Bufw )
 {
    delete [] Bufw;
    Bufw = NULL;
 }
 if ( NULL != Bufc )
 {
    delete [] Bufc;
    Bufc = NULL;
 }
 return RTV;
}

string DecodeURL(const string &URL)
{
    string result = "";
    for ( unsigned int i=0; i<URL.size(); i++ ) 
    {
        char c = URL[i];
        if ( c != '%' ) 
        {
            result += c;
        } 
        else 
        {
            char c1 = URL[++i];
            char c0 = URL[++i];
            int num = 0;
            num += HexChar2Dec(c1) * 16 + HexChar2Dec(c0);
            result += char(num);
        }
    }

    return result;
}

struct soap soap;

void InitSoap()
{
	soap_init(&soap);
}

void DestroySoap()
{
	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
}

int getInfo(int type, char * platetype, char * platenum, char * driverlicence, string & res)
{
	string result; 
	string id = "";
	string id2 = "";
	string xml = "";
	string str = "";
	string platenumgbk = "";

	if(platenum != NULL)
		CChineseCode::UTF_8ToGB2312(platenumgbk, (char *)platenum, strlen(platenum));

	string code = "7A1F090903051704081582838F87F3E288E9F2ED8B9395EF96B3A9AED0F2A383CFB5CDB3";

	switch(type)
	{
	case 1:
		{
			if((platetype == NULL)||(platenum == NULL)) return -1;

			id = "01";
			id2 = "01C21";
			xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><QueryCondition><hpzl>";
			str = platetype;
			xml += EncodeURL(str.c_str());
			xml += "</hpzl><hphm>";
			str = platenumgbk;
			xml += EncodeURL(str.c_str());
			xml += "</hphm></QueryCondition></root>";
		}
		break;
	case 2:
		{
			if((platetype == NULL)||(platenum == NULL)) return -1;

			id = "04";
			id2 = "04C03";
			xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><QueryCondition><hpzl>";
			str = platetype;
			xml += EncodeURL(str.c_str());
			xml += "</hpzl><hphm>";
			str = platenumgbk;
			xml += EncodeURL(str.c_str());
			xml += "</hphm><xh></xh><clbj></clbj></QueryCondition></root>";
		}
		break;
	case 3:
		{
			if(driverlicence == NULL) return -1;

			id = "02";
			id2 = "02C06";
			xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><QueryCondition><sfzmhm>";
			str = driverlicence;
			xml += EncodeURL(str.c_str());
			xml += "</sfzmhm></QueryCondition></root>";
		}
		break;
	case 4:
		{
			if(driverlicence == NULL) return -1;

			id = "02";
			id2 = "02C05";
			xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><QueryCondition><sfzmhm>";
			str = driverlicence;
			xml += EncodeURL(str.c_str());
			xml += "</sfzmhm></QueryCondition></root>";
		}
		break;
	case 5:
		{
			if(driverlicence == NULL) return -1;

			id = "04";
			id2 = "04C04";
			xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><QueryCondition><xh>";
			str = driverlicence;
			xml += EncodeURL(str.c_str());
			xml += "</xh></QueryCondition></root>";
		}
		break;
	default:
		{
			return 0;
		}
		break;
	}

	//printf("inxml: %s\n", xml.c_str());

	soap_call_ns1__queryObjectOut(&soap, "http://10.172.134.9:9080/trffweb/services/TmriOutAccess", "", id, code, id2, xml, result); 
	if (soap.error) 
	{
		soap_print_fault(&soap, stderr); 
		exit(1); 
	} 
	else 
	{
		res = DecodeURL(result);
		//printf("OUTXML utf8 : %s\n", res.c_str());
		string gbkres;
		CChineseCode::UTF_8ToGB2312(gbkres, (char *)res.c_str(), res.size());
		printf("GBK: %s\n", gbkres.c_str());
	}

	return 0; 
}
