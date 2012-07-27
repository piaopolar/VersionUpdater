#pragma once
#include <afxwin.h>
#include <string>

#define CONFIG_INI	"./config.ini"

const int MAX_STRING = 1024;

std::string GetTimeStr(void);
void FormatPath(std::string &strPath);
bool IsIncludeFileNamePath(std::string strPath);
void MyMakeSureDirectoryPathExists(std::string strPath);
void TrimRight(char *pszStr);
void SetLogEdit(CEdit *pEditLog);
void LogInfoIn(const char *pszFormat, ...);
void LogFile(const char *pszFormat, ...);
std::string MyTrim(char sz[]);
std::string MyTrim(std::string &str);
void ReplaceStdString(std::string &str, const std::string &strSrc, const std::string &strDest);
bool CreateIntermediateDirectory(const char *strDirectory);
int GetFileCode(std::string strFile);
std::string UTF8ToAnsi(const std::string &strIn, std::string &strOut);
std::string AnsiToUTF8(const std::string &strIn, std::string &strOut);
std::string ANSI_2_UTF8(const std::string &strANSI);
std::wstring ANSI_2_UTF16(const std::string &strANSI);
std::string UTF8_2_ANSI(const std::string &strUTF8);
std::wstring UTF8_2_UTF16(const std::string &strUTF8);
std::string UTF16_2_ANSI(const std::wstring &strUTF16);
std::string UTF16_2_UTF8(const std::wstring &strUTF16);
bool UTF16_2_ANSI(const wchar_t* wszUTF16, char *szANSI, int nBufANSI);
