/*
 * xxWriteLog.h
 *
 *  Created on: 2014-3-22
 *      Author: Administrator
 */

#ifndef XXWRITELOG_H_
#define XXWRITELOG_H_

#include <stdio.h>

inline void write_log(const char * logfilename,const char * logMsg)
{

	//char strip[256] = "";
	FILE * fp = fopen(logfilename,"at+");
	if(fp)
	{
			fwrite(logMsg,1,strlen(logMsg),fp);
			fclose(fp);
	}
};
#endif /* XXWRITELOG_H_ */
