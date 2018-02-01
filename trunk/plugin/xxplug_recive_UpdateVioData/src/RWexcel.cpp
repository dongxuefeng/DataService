#include "RWexcel.h"
#include "chinese_code.h"

void GB2312ToUnicode(char * str_,WCHAR*& pWStr_)//�õ�Unicode�ַ�
{
	int count = strlen(str_)*2+1;
	if(pWStr_ == NULL) 
	{
		pWStr_ = new WCHAR[count];
	}
	DWORD UnicodeSize=MultiByteToWideChar(CP_ACP,0,str_,count,pWStr_,(DWORD)count);  
}

rwExcel::rwExcel()
{

}

rwExcel::~rwExcel()
{

}

bool IsZhCn(char * buf, unsigned int buflen)
{
	for (unsigned int i = 0; i < buflen; i++)
	{
		if ((*(buf+i) < 0)||(*(buf+i) > 128))
			return true;
	}

	return false;
}

//дxls
int rwExcel::NewSheet(const char * sheetName, char * rowList[], unsigned int rowNum)
{
	BasicExcelCell * cell;
	WCHAR * buf = new WCHAR[255];
	string  gbk = "";

	m_xls.New(1);
	m_xls.RenameWorksheet("Sheet1", sheetName);
	m_sheet = m_xls.GetWorksheet(sheetName);

	if (m_sheet)
	{
		for(unsigned int i = 0; i < rowNum; i++)
		{
			cell = m_sheet->Cell(0, i);

			if(strlen(rowList[i]) != 0)
			{
				gbk = "";
				CChineseCode::UTF_8ToGB2312(gbk,rowList[i],strlen(rowList[i]));
				memset(buf, 0, 255*2);
				GB2312ToUnicode((char *)gbk.c_str(), buf);
			}
			cell->Set(buf);
		}
		return 0;
	}
	else
		return -1;
}

int rwExcel::AddLine(char * sheetName, char * rowList[], unsigned int rowNum)
{
	BasicExcelCell * cell;
	unsigned int maxRows;
	WCHAR * buf = new WCHAR[255];
	string gbk = "";

	if (m_sheet)
	{
		m_sheet = m_xls.GetWorksheet(sheetName);
		maxRows = m_sheet->GetTotalRows();

		printf("id = %s\n", rowList[0]);
		for(unsigned int i = 0; i < rowNum; i++)
		{
			cell = m_sheet->Cell(maxRows, i);

			memset(buf, 0, 255*2);
			if(strlen(rowList[i]) != 0)
			{
				gbk = "";
				CChineseCode::UTF_8ToGB2312(gbk,rowList[i],strlen(rowList[i]));
				GB2312ToUnicode((char *)gbk.c_str(), buf);
			}
			//printf("---%d---\nrowList = %s\nbuf = %s\n", i, rowList[i], buf);
			cell->Set(buf);
		}
		return 0;
	}
	else
		return -1;
}

int rwExcel::SaveAs(const char * fileName)
{
	m_xls.SaveAs(fileName);

	return 0;
}

//��xls
int rwExcel::LoadFile(const char * fileName)
{
	m_xls.Load(fileName);

	return 0;
}

int rwExcel::ReadLine(unsigned int rowNum, char * rowList)
{

	return 0;
}

void rwExcel::printxls()
{
	if (m_sheet)
	{
		size_t maxRows = m_sheet->GetTotalRows();
		size_t maxCols = m_sheet->GetTotalCols();
		cout << "Dimension of " << m_sheet->GetAnsiSheetName() << " (" << maxRows << ", " << maxCols << ")" << endl;

		printf("          ");
		for (size_t c=0; c<maxCols; ++c) printf("%10d", c+1);
		printf("\n");

		for (size_t r=0; r<maxRows; ++r)
		{
			printf("%10d", r+1);
			for (size_t c=0; c<maxCols; ++c)
			{
				BasicExcelCell* cell = m_sheet->Cell(r,c);
				switch (cell->Type())
				{
				case BasicExcelCell::UNDEFINED:
					printf("          ");
					break;

				case BasicExcelCell::INT:
					printf("%10d", cell->GetInteger());
					break;

				case BasicExcelCell::DOUBLE:
					printf("%10.6lf", cell->GetDouble());
					break;

				case BasicExcelCell::STRING:
					printf("%10s", cell->GetString());
					break;

				case BasicExcelCell::WSTRING:
					wprintf(L"%10s", cell->GetWString());
					break;
				}
			}
			printf("\n");
		}
	}
}
