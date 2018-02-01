#ifndef _analysis_xml_h__
#define _analysis_xml_h__

#ifndef _define_xml_field__
#define _define_xml_field__
typedef struct XMLField
{
	int ntype;
	char struser[20];
	char strdev[20];
}XMLFIELD,*PXMLFIELD;
#endif

bool set_data(char * xmldata,XMLField * xmlField);

#endif
