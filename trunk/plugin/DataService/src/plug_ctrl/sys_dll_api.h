/*
 * plug_ctrl_sysapi.h
 *
 *  Created on: 2012-1-11
 *      Author: Administrator
 */

#ifndef PLUG_CTRL_SYSAPI_H_
#define PLUG_CTRL_SYSAPI_H_

#include <windows.h>
inline void * load_dll(const char * plug_name)
{
	return LoadLibrary((const char *) plug_name);
};

inline void * get_func_addr(void * dll_handle,const char * func_name)
{
	return (void *)GetProcAddress((HINSTANCE)dll_handle, func_name);
};

inline void free_dll(void * dll_handle)
{
	FreeLibrary((HINSTANCE)dll_handle);
	CloseHandle((HINSTANCE)dll_handle);
};





#endif /* PLUG_CTRL_SYSAPI_H_ */
