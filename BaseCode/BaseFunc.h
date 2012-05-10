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
