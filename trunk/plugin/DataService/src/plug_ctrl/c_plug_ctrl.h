/*
 * c_plug_ctrl.h
 *
 *  Created on: 2011-9-27
 *      Author: xiaohua.shan
 *      plug control class
 */

#ifndef C_PLUG_CTRL_H_
#define C_PLUG_CTRL_H_

#ifndef _define_xml_field__
#define _define_xml_field__
typedef struct XMLField
{
	int ntype;   //�����������
	char struser[20];  //�û�
	char strdev[20];   //�豸
}XMLFIELD,*PXMLFIELD;
#endif

#include <map>
#include <pthread.h>

class _device_item
{
public:
	typedef int (* xx_rcv_procinfo)(char * xmldata,char ** outxmldata);
	typedef int (* xx_rcv_gettype)();

public:
	unsigned char plug_name[256];

private:
	xx_rcv_procinfo p_rcv_procinfo;
	xx_rcv_gettype p_rcv_gettype;
	pthread_mutex_t mtx;
	void *	m_dll_handle;

public:
	_device_item(const char * strplug_name);
	~_device_item();
	int get_type();
	int rcv_prc_info(char * xmldata,char ** outxmldata);
};

class c_plug_ctrl
{
public:
	std::map<int ,_device_item*> m_device_list;
public:
	c_plug_ctrl(char * pluger_dir);
	virtual ~c_plug_ctrl();
public:
	_device_item * get_device_mode(int ntype);
	int proc_info(char * inxmldata,char ** outxmldata); //plug ִ�е��
};

#endif
