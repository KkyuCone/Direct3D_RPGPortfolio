#include "Engine.h"

std::wstring StringToWstring(std::string s)
{
	const char* all = s.c_str();
	int len = (int)(1 + strlen(all));
	wchar_t* t = new wchar_t[len];
	if (NULL == t) throw std::bad_alloc();

	size_t AfterLen = 0;
	//mbstowcs(t, all, len);
	mbstowcs_s(&AfterLen, t, len, all, _TRUNCATE);

	std::wstring ws = t;;

	delete[] t;

	return ws;
}

// TCHAR -> String
std::string TCHARToString(const TCHAR* ptsz)
{
	size_t len = wcslen((wchar_t*)ptsz);
	char* psz = new char[2 * len + 1];

	size_t AfterLen = 0;
	wcstombs_s(&AfterLen, psz, 2 * len + 1, (wchar_t*)ptsz, _TRUNCATE);
	//wcstombs(psz, (wchar_t*)ptsz, 2 * len + 1);
	std::string s = psz;
	delete[] psz;
	return s;
}

// String -> TCHAR
bool StringToTCHAR(std::string _str, TCHAR& _strDest)
{
	TCHAR* pRetrun = new TCHAR[_str.size() + 1];
	pRetrun[_str.size()] = 0;
	std::copy(_str.begin(), _str.end(), pRetrun);

	_strDest = *pRetrun;
	delete[] pRetrun;

	return true;
}

// string -> wchar_t
std::wstring  StringToLPWSTR(const std::string input)
{
	// null-call to get the size
	//size_t len = mbstowcs(NULL, &input[0], input.length());
	size_t len = input.size();

	// allocate
	wchar_t* t = new wchar_t[len + 1];

	// real call
	// 	mbstowcs_s(&AfterLen, t, len, all, _TRUNCATE);
	size_t AfterLen = 0;
	mbstowcs_s(&AfterLen, t, input.length() + 1, &input[0], _TRUNCATE);

	std::wstring returnValue = t;
	delete[] t;

	return returnValue;
}

// string ->wstring
bool StringToWstring(const std::string input, std::wstring& _output)
{
	// null-call to get the size
	size_t len = mbstowcs(NULL, &input[0], input.length());

	// allocate
	wchar_t* t = new wchar_t[len+1];

	// real call
	// 	mbstowcs_s(&AfterLen, t, len, all, _TRUNCATE);
	size_t AfterLen = 0;
	mbstowcs_s(&AfterLen, t, input.length() + 1, &input[0], _TRUNCATE);

	_output = t;
	delete[] t;
 
	return true;
}

// string -> int
int StringToInt(std::string _str)
{
	return (std::stoi(_str));
}


// int -> string
std::string	IntToString(int _Number)
{
	return std::to_string(_Number);
}