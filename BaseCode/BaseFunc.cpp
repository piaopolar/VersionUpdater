#include "stdafx.h"

#include "BaseFunc.h"

CEdit *s_pEditLog;

// ============================================================================
// ==============================================================================
bool IsIncludeFileNamePath(std::string strPath)
{
	return strstr(strPath.c_str(), ".");
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
	//~~~~~~~~~~~~~~~~~~~~
	static CString cstrData;
	std::string strLine;
	va_list args;
	int len;
	char *buffer;
	//~~~~~~~~~~~~~~~~~~~~

	va_start(args, pszFormat);
	len = _vscprintf(pszFormat, args) + 1;	// _vscprintf doesn't count

	// terminating '\0'
	buffer = static_cast<char *>(malloc(len * sizeof(char)));
	vsprintf_s(buffer, len, pszFormat, args);

	strLine = buffer;
	free(buffer);

	strLine += "\r\n";
	cstrData += strLine.c_str();

	if (NULL == s_pEditLog) {
		return;
	}

	s_pEditLog->SetWindowText(cstrData.GetBuffer(0));
	s_pEditLog->UpdateWindow();
	s_pEditLog->LineScroll(s_pEditLog->GetLineCount());
}

// ============================================================================
// ==============================================================================
void LogFile(const char *pszFormat, ...)
{
	//~~~~~~~~~~~~~~~~~~~~
	static CString cstrData;
	std::string strLine;
	va_list args;
	int len;
	char *buffer;
	//~~~~~~~~~~~~~~~~~~~~

	va_start(args, pszFormat);
	len = _vscprintf(pszFormat, args) + 1;	// _vscprintf doesn't count

	// terminating '\0'
	buffer = static_cast<char *>(malloc(len * sizeof(char)));
	vsprintf_s(buffer, len, pszFormat, args);

	strLine = buffer;
	free(buffer);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char szLogFile[_MAX_PATH] = { 0 };
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	GetPrivateProfileString("GlobalSet", "LogFile", "", szLogFile,
							sizeof(szLogFile), CONFIG_INI);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(szLogFile, "w+");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
void ReplaceStdString(std::string &str,
					  const std::string &strSrc,
					  const std::string &strDest)
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
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if 0
	csDirectory.Replace('/', '//');
	while (csDirectory.Replace("////", "//") > 0);
	csDirectory.TrimRight('//');
#endif

	//~~~~~~~~~~~~~~~
	int iLastIndex = 0;
	//~~~~~~~~~~~~~~~

	while (true) {
		iLastIndex = csDirectory.Find('\\', iLastIndex);

		if (iLastIndex == -1) {
			csIntermediateDirectory = csDirectory;
		} else {
			csIntermediateDirectory = csDirectory.Left(iLastIndex);
			iLastIndex++;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// 如果该文件夹不存在，则创建之
		HANDLE hDirectory = CreateFile(csIntermediateDirectory, GENERIC_READ,
									   FILE_SHARE_READ, NULL, OPEN_EXISTING,
									   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
									   NULL);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (hDirectory == INVALID_HANDLE_VALUE) {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			BOOL bCreated = CreateDirectory(csIntermediateDirectory, NULL);
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			if (!bCreated)
			{
#if 0
				// logger.Log (LL_ERROR, "Create directory %s error!
				// ErrorCode=%d", csIntermediateDirectory, GetLastError () );
#endif
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
