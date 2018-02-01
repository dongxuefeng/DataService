/*
 * c_plug_ctrl.cpp
 *
 *  Created on: 2011-9-27
 *      Author: Administrator
 */

#include "c_plug_ctrl.h"
#include <stdio.h>
#include <map>
#include <iostream>
#include "sys_dll_api.h"
#include <io.h>
#include "analysisXML.h"

using namespace std;

_device_item::_device_item(const char * strplug_name)
{
	if(pthread_mutex_init(&mtx, NULL) != 0)
		printf("pthread_mutex_init error!\n");
	p_rcv_procinfo = 0;
	p_rcv_gettype = 0;
	sprintf((char *) plug_name, strplug_name);
	m_dll_handle = load_dll((const char *) plug_name);

	if (m_dll_handle)
	{
		p_rcv_gettype = (xx_rcv_gettype) get_func_addr(
						m_dll_handle, "xx_rcv_gettype");

		p_rcv_procinfo = (xx_rcv_procinfo) get_func_addr(
				m_dll_handle, "xx_rcv_procinfo");
	}
}
_device_item::~_device_item()
{
	p_rcv_procinfo = 0;
	p_rcv_gettype = 0;

	free_dll(m_dll_handle);
	pthread_mutex_destroy(&mtx);
}

int _device_item::get_type()
{
	return p_rcv_gettype();
}

int _device_item::rcv_prc_info(char * xmldata,char ** outxmldata)
{
	int iresult = 0;
	if (pthread_mutex_lock(&mtx) == 0)
	{
		if (p_rcv_procinfo != NULL)
		{
			try
			{
				iresult = (*p_rcv_procinfo)(xmldata,outxmldata);
			}
			catch (...)
			{
				printf("plug error\n");
			}
		}
		pthread_mutex_unlock(&mtx);
	}
	else
		printf("=====this dev is working,lost data!");

	return iresult;
}

_device_item * c_plug_ctrl::get_device_mode(int ntype)
{
	_device_item * p_dev_item = NULL;

	for (std::map<int ,_device_item*>::iterator iter = m_device_list.begin(); iter
				!= m_device_list.end(); iter++)
	{
		if(iter->first == ntype)
		{
			p_dev_item = (_device_item *)(iter->second);
			break;
		}
	}

	return p_dev_item;
}

int c_plug_ctrl::proc_info(char * inxmldata,char ** outxmldata)
{
	XMLField xmlField;

	if(set_data(inxmldata, &xmlField) == false) return -1;
	//printf("====== plug type = %d ======\n", xmlField.ntype);

	_device_item * pdeviceitem = get_device_mode(xmlField.ntype);
	if (pdeviceitem == NULL)
	{
		printf("Not Find Plug!\n");
		return -1;
	}

	int ret = pdeviceitem->rcv_prc_info(inxmldata,outxmldata);
	pdeviceitem = NULL;

	return ret;
}

c_plug_ctrl::c_plug_ctrl(char * pluger_dir)
{
	_finddata_t find_data;
	char filetmp[256];
	char file[256];
	int  type = 0;

	memset(file, 0, 256);
	sprintf(file, "%s/%s", pluger_dir, "libxxplug_recive_*.dll");
	printf("find pluger: %s\n", file);

	long find_handle = _findfirst(file,&find_data);

	if(find_handle != -1)
	{
		do
		{
			memset(filetmp, 0, 256);
			sprintf(filetmp, "%s/%s", pluger_dir, find_data.name);
			printf("load plug %s\n", filetmp);
			_device_item * p_dev_ctrl = new _device_item(filetmp);

			type = p_dev_ctrl->get_type();
			m_device_list.insert(std::pair<int,_device_item*>(type, p_dev_ctrl));
			printf("=== TYPE = %d ===\n", type);

		}while(_findnext(find_handle,&find_data)!= -1);
		_findclose(find_handle);
	}
}

c_plug_ctrl::~c_plug_ctrl()
{
	for (std::map<int ,_device_item*>::const_iterator iter = m_device_list.begin(); iter
			!= m_device_list.end(); iter++)
	{
		_device_item * p_dev_item = (_device_item *)(iter->second);
		delete p_dev_item;
	}
	m_device_list.clear();
}


