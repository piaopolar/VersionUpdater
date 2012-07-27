#include "stdafx.h"

#include "boost/regex.hpp"

#include "BaseFunc.h"

CEdit *s_pEditLog;

// ============================================================================
// ==============================================================================
bool IsIncludeFileNamePath(std::string strPath)
{
	boost::regex expFilePath("(.*)\\\\(.*\\.\\w\\w\\w)$");

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bool bMatch = boost::regex_match(strPath, expFilePath);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	return bMatch;
}

// ============================================================================
// ==============================================================================
void FormatPath(std::string &strPath)
{
	if (strPath.empty()) {
		return;
	}

	if (!IsIncludeFileNamePath(strPath)) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		char cLast = strPath.at(strPath.length() - 1);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		switch (cLast) {
		case '\\':
		case '/':
			break;
		default:
			strPath += "\\";
			break;
		}
	}

	ReplaceStdString(strPath, "/", "\\");
}

// ============================================================================
// ==============================================================================
void TrimRight(char *pszStr)
{
	if (NULL == pszStr) {
		return;
	}

	//~~~~~~~~~~~~~~~~~~~~~~
	int nLen = strlen(pszStr);
	char *pLast;
	//~~~~~~~~~~~~~~~~~~~~~~

	for (pLast = pszStr + nLen - 1; pszStr >= pszStr; --pLast) {

		//~~~~~~~~~~~~~~
		bool bRid = false;
		//~~~~~~~~~~~~~~

		switch (*pLast) {
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			bRid = true;
			break;
		default:
			break;
		}

		if (!bRid) {
			break;
		}
	}

	*(pLast + 1) = 0;
}

// ============================================================================
// ==============================================================================
void LogInfoIn(const char *pszFormat, ...)
{
	if (NULL == s_pEditLog) {
		return;
	}

	//~~~~~~~~~~~~~
	CString cstrData;
	//~~~~~~~~~~~~~

	s_pEditLog->GetWindowText(cstrData);

	//~~~~~~~~~~~~~~~~
	std::string strLine;
	va_list args;
	int len;
	char *buffer;
	//~~~~~~~~~~~~~~~~

	va_start(args, pszFormat);
	len = _vscprintf(pszFormat, args) + 1;	// _vscprintf doesn't count

	// terminating '\0'
	buffer = static_cast<char *>(malloc(len * sizeof(char)));
	vsprintf_s(buffer, len, pszFormat, args);

	strLine = buffer;
	free(buffer);

	strLine += "\r\n";
	cstrData += strLine.c_str();

	s_pEditLog->SetWindowText(cstrData.GetBuffer(0));
	s_pEditLog->UpdateWindow();
	s_pEditLog->LineScroll(s_pEditLog->GetLineCount());

	LogFile(strLine.c_str());
}

// ============================================================================
// ==============================================================================
void LogFile(const char *pszFormat, ...)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	static std::string s_strLogFile = "";
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (s_strLogFile.empty()) {

		//~~~~~~~~~~~~~~~~~~~~~~
		char szLogDir[MAX_STRING];
		//~~~~~~~~~~~~~~~~~~~~~~

		GetPrivateProfileString("GlobalSet", "LogDir", "", szLogDir, sizeof(szLogDir), CONFIG_INI);

		if (!strlen(szLogDir)) {
			::GetCurrentDirectory(sizeof(szLogDir), szLogDir);

			strcat(szLogDir, "\\Log\\");
		}

		s_strLogFile = szLogDir;
		FormatPath(s_strLogFile);
		MyMakeSureDirectoryPathExists(s_strLogFile);
		s_strLogFile += GetTimeStr();
		s_strLogFile += ".log";
	}

	//~~~~~~~~~~~~~~~~
	std::string strLine;
	va_list args;
	int len;
	char *buffer;
	//~~~~~~~~~~~~~~~~

	va_start(args, pszFormat);
	len = _vscprintf(pszFormat, args) + 1;	// _vscprintf doesn't count

	// terminating '\0'
	buffer = static_cast<char *>(malloc(len * sizeof(char)));
	vsprintf_s(buffer, len, pszFormat, args);

	strLine = buffer;
	free(buffer);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(s_strLogFile.c_str(), "a+");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		return;
	}

	fprintf(pFile, "%s\n", strLine.c_str());
	fclose(pFile);
}

// ============================================================================
// ==============================================================================
std::string MyTrim(char sz[])
{
	//~~~~~~~~~~~~~~
	CString cstr = sz;
	//~~~~~~~~~~~~~~

	cstr.Trim();
	strcpy(sz, cstr.GetBuffer(0));
	return sz;
}

// ============================================================================
// ==============================================================================
std::string MyTrim(std::string &str)
{
	//~~~~~~~~~~~~~~~~~~~~~~~
	CString cstr = str.c_str();
	//~~~~~~~~~~~~~~~~~~~~~~~

	cstr.Trim();
	str = cstr.GetBuffer(0);
	return str;
}

// ============================================================================
// ==============================================================================
void SetLogEdit(CEdit *pEditLog)
{
	s_pEditLog = pEditLog;
}

// ============================================================================
// ==============================================================================
void ReplaceStdString(std::string &str, const std::string &strSrc, const std::string &strDest)
{
	//~~~~~~~~~~~~~~~~~~~~~~~
	CString cstr = str.c_str();
	//~~~~~~~~~~~~~~~~~~~~~~~

	cstr.Replace(strSrc.c_str(), strDest.c_str());

	str = cstr.GetBuffer(0);
}

// ============================================================================
// ==============================================================================
void MyMakeSureDirectoryPathExists(std::string strPath)
{
	FormatPath(strPath);
	if (IsIncludeFileNamePath(strPath)) {
		std::string::size_type iLast = strPath.find_last_of("\\");
		strPath = strPath.substr(0, iLast);
	}

	CreateIntermediateDirectory(strPath.c_str());
}

// ============================================================================
// ==============================================================================
bool CreateIntermediateDirectory(const char *strDirectory)
{
	if (strDirectory == NULL || strDirectory[0] == 0) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bool bErrorOccur = false;
	CString csDirectory = strDirectory;
	CString csIntermediateDirectory;
	int iLastIndex = 0;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	while (true) {
		iLastIndex = csDirectory.Find('\\', iLastIndex);

		if (iLastIndex == -1) {
			csIntermediateDirectory = csDirectory;
		} else {
			csIntermediateDirectory = csDirectory.Left(iLastIndex);
			iLastIndex++;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// 如果该文件夹不存在，则创建之
		HANDLE hDirectory = CreateFile(csIntermediateDirectory, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
									   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (hDirectory == INVALID_HANDLE_VALUE) {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			BOOL bCreated = CreateDirectory(csIntermediateDirectory, NULL);
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			if (!bCreated) {
				bErrorOccur = true;
				break;
			}
		} else {
			CloseHandle(hDirectory);
		}

		if (iLastIndex == -1) {
			break;
		}
	}

	return !bErrorOccur;
}

// ============================================================================
// ==============================================================================
std::string GetTimeStr(void)
{
	//~~~~~~~~~
	time_t ltime;
	//~~~~~~~~~

	time(&ltime);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	struct tm *pTime = localtime(&ltime);
	char szTmp[MAX_STRING] = { 0 };
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	_snprintf(szTmp, MAX_STRING - 1, "%04d_%02d_%02d__%02d_%02d_%02d", pTime->tm_year + 1900, pTime->tm_mon + 1,
			  pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
	return std::string(szTmp);
}

// ============================================================================
// ==============================================================================
int GetFileCode(std::string strFile)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (INVALID_HANDLE_VALUE == hFile) {
		return -1;
	}

	//~~~~~~~~~~~~~~~~~~
	unsigned char buff[2];
	//~~~~~~~~~~~~~~~~~~

	memset(buff, 0, sizeof(buff));

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	DWORD dwRead = 0;
	BOOL bRet = ReadFile(hFile, buff, 2, &dwRead, NULL);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	CloseHandle(hFile);

	if (!bRet) {
		return 0;
	}

	if (0xFF == buff[0] && 0xFE == buff[1]) {
		return 1;
	}

	if (0xEF == buff[0] && 0xBB == buff[1]) {
		return 2;
	}

	if (0xFE == buff[0] && 0xFF == buff[1]) {
		return 3;
	}

	return 0;
}

// ============================================================================
// ==============================================================================
std::string UTF8ToAnsi(const std::string &strIn, std::string &strOut)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	WCHAR *strSrc = NULL;
	TCHAR *szRes = NULL;
	int i = MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, NULL, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, strIn.c_str(), -1, strSrc, i);

	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);

	szRes = new TCHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	strOut = szRes;

	delete[] strSrc;
	delete[] szRes;

	return strOut;
}

// ============================================================================
// ==============================================================================
std::string AnsiToUTF8(const std::string &strIn, std::string &strOut)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	WCHAR *strSrc = NULL;
	TCHAR *szRes = NULL;
	int len = MultiByteToWideChar(CP_ACP, 0, (LPCTSTR) strIn.c_str(), -1, NULL, 0);
	unsigned short *wszUtf8 = new unsigned short[len + 1];
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	memset(wszUtf8, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR) strIn.c_str(), -1, (LPWSTR) wszUtf8, len);

	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR) wszUtf8, -1, NULL, 0, NULL, NULL);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char *szUtf8 = new char[len + 1];
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR) wszUtf8, -1, szUtf8, len, NULL, NULL);

	strOut = szUtf8;

	delete[] szUtf8;
	delete[] wszUtf8;

	return strOut;
}

// ============================================================================
// ==============================================================================
std::wstring ANSI_2_UTF16(const std::string &strANSI)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nUnicodeLength = ::MultiByteToWideChar(CP_ACP, 0, strANSI.c_str(), -1, NULL, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	std::wstring strUTF16(nUnicodeLength, _T(' '));

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nRet = ::MultiByteToWideChar(CP_ACP, 0, strANSI.c_str(), -1, &strUTF16[0], nUnicodeLength);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	ASSERT(0 != nRet);

	return strUTF16;
}

// ============================================================================
// ==============================================================================
std::string UTF16_2_ANSI(const std::wstring &strUTF16)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nANSILength = ::WideCharToMultiByte(CP_ACP, 0, strUTF16.c_str(), -1, NULL, 0, 0, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	std::string strANSI(nANSILength, ' ');

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nRet = ::WideCharToMultiByte(CP_ACP, 0, strUTF16.c_str(), -1, &strANSI[0], nANSILength, 0, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	ASSERT(0 != nRet);
	return strANSI;
}

// ============================================================================
// ==============================================================================
bool UTF16_2_ANSI(const wchar_t *wszUTF16, char *szANSI, int nBufANSI)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nANSILength = ::WideCharToMultiByte(CP_ACP, 0, wszUTF16, -1, NULL, 0, 0, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (nBufANSI < nANSILength) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nRet = ::WideCharToMultiByte(CP_ACP, 0, wszUTF16, -1, szANSI, nANSILength, 0, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	ASSERT(0 != nRet);
	return true;
}

// ============================================================================
// ==============================================================================
std::string UTF16_2_UTF8(const std::wstring &strUTF16)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nUTF8Length = ::WideCharToMultiByte(CP_UTF8, 0, strUTF16.c_str(), -1, NULL, 0, 0, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	std::string strUTF8(nUTF8Length + 1, '\0');

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nRet = ::WideCharToMultiByte(CP_UTF8, 0, strUTF16.c_str(), -1, &strUTF8[0], nUTF8Length + 1, 0, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	return strUTF8;
}

// ============================================================================
// ==============================================================================
std::wstring UTF8_2_UTF16(const std::string &strUTF8)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nUTF16Length = ::MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	nUTF16Length += 1;
	std::wstring strUTF16(nUTF16Length, ' ');

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nRet = ::MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, &strUTF16[0], nUTF16Length);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	ASSERT(0 != nRet);

	return strUTF16;
}

// ============================================================================
// ==============================================================================
std::string UTF8_2_ANSI(const std::string &strUTF8)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::wstring wstrUTF16 = UTF8_2_UTF16(strUTF8);
	std::string strANSI = UTF16_2_ANSI(wstrUTF16);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	return strANSI;
}

// ============================================================================
// ==============================================================================
std::string ANSI_2_UTF8(const std::string &strANSI)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::wstring wstrUTF16 = ANSI_2_UTF16(strANSI);
	std::string strUTF8 = UTF16_2_UTF8(wstrUTF16);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	return strUTF8;
}
