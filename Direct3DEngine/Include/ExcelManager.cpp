#include "ExcelManager.h"
#include "PathManager.h"

ENGINE_USING

DEFINITION_SINGLETON(ExcelManager)

ExcelManager::ExcelManager()
{
	
}

ExcelManager::~ExcelManager()
{
	std::unordered_map<std::string, Book*>::iterator StartIter = m_mapBook.begin();
	std::unordered_map<std::string, Book*>::iterator EneIter = m_mapBook.end();

	for (; StartIter!= EneIter; ++StartIter)
	{
		StartIter->second->release();
	}
}


bool ExcelManager::Init()
{
#pragma region 예제
	//  단순 세이브
	/*Book* book = xlCreateBook();

	if(nullptr != book)
	{
	Sheet* sheet = book->addSheet(L"Sheet1");

	if (sheet)
	{
	sheet->writeStr(2, 1, L"익명이는 귀여워");
	sheet->writeNum(3, 1, 1000);
	}

	book->save(L"Ikmeyng is cute.xls");
	book->release();
	}
	else
	{
	return false;
	}*/


	//Book* book = xlCreateBook();

	//if (nullptr != book)
	//{
	//	if (true == book->load(L"Ikmeyng is cute.xls"))
	//	{
	//		Sheet* sheet = book->getSheet(0);

	//		const wchar_t* s = sheet->readStr(2, 1);
	//		double d = sheet->readNum(3, 1);
	//	}
	//}

	//book->release();
#pragma endregion
	BookCreate("Ikmeyng");
	LoadExcelFile("Ikmeyng", "Ikmeyng.xls", PATH_EXCEL);
	std::string ddd = ReadString("Ikmeyng", 2, 1, 0);
	
	return true;
}

bool ExcelManager::LoadExcelFile(const char* _strName,
	const char* _FileName, const char * _FilePath)
{
	std::string strname = _strName;
	std::unordered_map<std::string, Book*>::iterator FIter = m_mapBook.find(strname);

	if (FIter == m_mapBook.end())
	{
		return false;
	}

	std::string Path = _FilePath;
	const char* FilePath = GET_SINGLETON(PathManager)->FindPathMultibyte(Path);


	if (nullptr == FilePath)
	{
		return false;
	}
	std::string FullFileName = FilePath;
	FullFileName += _FileName;

	// 해당 경로의 엑셀파일 로드하기
	/*std::wstring FullFilePath = L"";
	StringToWstring(FullFileName, FullFilePath);*/

	std::wstring FullFilePath = L"";
	FullFilePath = StringToLPWSTR(FullFileName);


	if (false == FIter->second->load(FullFilePath.c_str()))
	{
		return false;
	}
	return true;
}

// 읽기
std::string ExcelManager::ReadString(const char* _strName, int _Row, int _Col, int _SheetNumber)
{
	std::string strname = _strName;
	std::unordered_map<std::string, Book*>::iterator FIter = m_mapBook.find(strname);

	if (FIter == m_mapBook.end())
	{
		return nullptr;
	}

	Sheet* sheet = FIter->second->getSheet(_SheetNumber); 

	if(nullptr == sheet)
	{
		return nullptr;
	}

	// 첫번째 줄은 못읽어온다.
	const wchar_t* s = sheet->readStr(1, 1);
	std::wstring ws(s);

	std::string RerturnStr(ws.begin(), ws.end());

	return RerturnStr;
}

int ExcelManager::ReadInt(const char* _strName, int _Row, int _Col, int _SheetNumber)
{
	std::string strname = _strName;
	std::unordered_map<std::string, Book*>::iterator FIter = m_mapBook.find(strname);

	if (FIter == m_mapBook.end())
	{
		assert(false);
		return -1;
	}

	Sheet* sheet = FIter->second->getSheet(_SheetNumber);
	if (nullptr == sheet)
	{
		return -1;
	}
	int i = (int)(sheet->readNum(3, 1));

	return i;
}

float ExcelManager::ReadFloat(const char* _strName, int _Row, int _Col, int _SheetNumber)
{
	std::string strname = _strName;
	std::unordered_map<std::string, Book*>::iterator FIter = m_mapBook.find(strname);

	if (FIter == m_mapBook.end())
	{
		assert(false);
		return -1.0f;
	}

	Sheet* sheet = FIter->second->getSheet(_SheetNumber);
	if (nullptr == sheet)
	{
		return -1.0f;
	}
	float f = (float)(sheet->readNum(3, 1));

	return f;
}

double ExcelManager::ReadDouble(const char* _strName, int _Row, int _Col, int _SheetNumber)
{
	std::string strname = _strName;
	std::unordered_map<std::string, Book*>::iterator FIter = m_mapBook.find(strname);

	if (FIter == m_mapBook.end())
	{
		assert(false);
		return -1.0;
	}

	Sheet* sheet = FIter->second->getSheet(_SheetNumber);
	if (nullptr == sheet)
	{
		return -1.0;
	}
	double d = sheet->readNum(3, 1);

	return d;
}



bool ExcelManager::BookCreate(const char* _strName)
{
	// 이미 생성된거면 생성 X
	std::string strname = _strName;
	std::unordered_map<std::string, Book*>::iterator FIter = m_mapBook.find(strname);

	if (FIter != m_mapBook.end())
	{
		return false;
	}

	Book* NewBook = xlCreateBook();

	if (nullptr == NewBook)
	{
		return false;
	}

	m_mapBook.insert(std::make_pair(strname, NewBook));


	return true;
}
