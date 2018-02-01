#ifndef _DAY_H_
#define _DAY_H_

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
using namespace std;

//IsLeap函数判断一个年份是否为闰年，方法如下:
inline bool IsLeap(int year)
{
   return (year % 4 ==0 || year % 400 ==0) && (year % 100 !=0);
}

//上面的StringToDate函数用于取出日期中的年月日并判断日期是否合法
//从字符中最得年月日 规定日期的格式是yyyy-mm-dd
inline bool StringToDate(char * date, int& year, int& month, int& day)
{
	char * q = new char[20];
	sprintf(q,date);
	char * p = strtok(q, "-");
	if(p)
	{
		year = atoi(p);
		p = strtok(NULL,"-");
		month = atoi(p);
		p = strtok(NULL,"-");
		day = atoi(p);

		int DAY[12]={31,28,31,30,31,30,31,31,30,31,30,31};
		if(IsLeap(year)){
			 DAY[1] = 29;
		}
		delete q;
		return year >= 0 && month<=12 && month>0 && day<=DAY[month] && day>0;
	}
	else
	{
		delete q;
		return -1;
	}
}

//DayInYear能根据给定的日期，求出它在该年的第几天，代码如下
inline int DayInYear(int year, int month, int day)
{
    //int _day = 0;
    int DAY[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(IsLeap(year))
        DAY[1] = 29;
    for(int i=0; i<month - 1; ++i)
    {
        day += DAY[i];
    }
    return day;
}

inline int DaysBetween2Date(char * date1, char * date2)
{
     //取出日期中的年月日
     int year1, month1, day1;
     int year2, month2, day2;

     if(!StringToDate(date1, year1, month1, day1) || !StringToDate(date2, year2,month2,day2))
     {
        cout<<"输入的日期格式不正确";
        return -1;
     }
     if(year1 == year2 && month1 == month2)
     {
         //return day1 > day2 ? day1 - day2 : day2 - day1;
    	 return day2 - day1;

     //如果年相同
     }else if(year1 == year2)
     {
         int d1, d2;
         d1 = DayInYear(year1, month1, day1);
         d2 = DayInYear(year2, month2, day2);
         //return d1 > d2 ? d1 - d2 : d2 - d1;
         return d2 - d1;

     //年月都不相同
     }
     else
     {
    	 int a = 1;
         //确保year1年份比year2早
         if(year1 > year2)
         {
             //swap进行两个值的交换
             swap(year1, year2);
             swap(month1, month2);
             swap(day1, day2);
             a = -1;
         }

         int d1,d2,d3;
         if(IsLeap(year1))
             d1 = 366 - DayInYear(year1,month1, day1); //取得这个日期在该年还于下多少天
         else
             d1 = 365 - DayInYear(year1,month1, day1);
         d2 = DayInYear(year2,month2,day2); //取得在当年中的第几天
         //cout<<"d1:"<<d1<<", d2:"<<d2;

         d3 = 0;
         for(int year = year1 + 1; year < year2; year++)
         {
             if(IsLeap(year))
                 d3 += 366;
             else
                 d3 += 365;
         }

         return (d1 + d2 + d3)*a;
     }
}

inline void GetDate(char * intime, char * outtime, int days)//获取一个日期后的N天后的日期
{
	struct tm *local;
	time_t old;

	old = time(NULL);
	local=localtime(&old);

	char tmp[16] = {0};
	sprintf(tmp, "%d-%d-%d", local->tm_year+1900, local->tm_mon+1, local->tm_mday);

	int a = DaysBetween2Date(intime,tmp);

	//printf("a = %d\n", a);

	old = time(NULL);
	old = old - a*24*60*60;
	old = old + days*24*60*60;
	local=localtime(&old);

	sprintf(outtime, "%d-%02d-%02d", local->tm_year+1900, local->tm_mon+1, local->tm_mday);
}

#endif
