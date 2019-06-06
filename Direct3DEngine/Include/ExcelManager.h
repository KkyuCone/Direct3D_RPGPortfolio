#pragma once
#include "Engine.h"
#include "libxl.h"
using namespace libxl;

ENGINE_BEGIN

class ENGINE_DLL ExcelManager
{
private:
	std::unordered_map<std::string, Book*> m_mapBook;

public:

public:
	void SetBook(Book* _Book);

public:
	bool Init();

public:
	bool BookCreate(const char* _strName);		// 
	bool LoadExcelFile(const char* _strName, const char* _FileName,
		const char* _FilePath = PATH_EXCEL);
	// Save
	//void SaveExcel(const std::string& _strFileName, const std::string& _PathName = PATH_EXCEL);

public:
	// 시트번호, 행, 열 순
	std::string ReadString(const char* _strName, int _Row, int _Col, int _SheetNumber = 0);
	int ReadInt(const char* _strName, int _Row, int _Col, int _SheetNumber = 0);
	float ReadFloat(const char* _strName, int _Row, int _Col, int _SheetNumber = 0);
	double ReadDouble(const char* _strName, int _Row, int _Col, int _SheetNumber = 0);

public:
	DECLARE_SINGLETON(ExcelManager)
};

ENGINE_END