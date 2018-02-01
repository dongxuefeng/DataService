#ifndef _RWEXCEL_H_
#define _RWEXCEL_H_

#include <stdio.h>
#include <Windows.h>

#include "BasicExcel.hpp"
using namespace YExcel;

class rwExcel
{
public:
	rwExcel();
	~rwExcel();

public:
	//Ð´xls
	int NewSheet(const char * sheetName, char * rowList[], unsigned int rowNum);
	int AddLine(char * sheetName, char * rowList[], unsigned int rowNum);
	int SaveAs(const char * fileName);
	//¶Áxls
	int LoadFile(const char * fileName);
	int ReadLine(unsigned int rowNum, char * rowList);
	void printxls();

private:
	BasicExcel m_xls;
	BasicExcelWorksheet * m_sheet;

};

#endif