#pragma once
#include <afxwin.h>
#include <string>

#define CONFIG_INI	"config.ini"

const int MAX_STRING = 1024;

void FormatPath(std::string &strPath);
void TrimRight(char *pszStr);
void SetLogEdit(CEdit *pEditLog);
void LogInfoIn(const char *pszFormat, ...);
void LogFile(const char *pszFormat, ...);
std::string MyTrim(char sz[]);
std::string MyTrim(std::string &str);
