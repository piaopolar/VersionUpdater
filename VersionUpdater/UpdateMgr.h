#pragma once


#include <map>
#include <string>
#include <vector>

#include "BaseCode/Singleton.h"


class C3DMotionElement
{
public:
	C3DMotionElement(const char* pszPath);
private:
	std::string m_strPath;

};

class CUpdateMgr : public Singleton<CUpdateMgr>
{
public:
	CUpdateMgr(void);
	~ CUpdateMgr(void);


	void SetEnvPath(const char* pszOld, const char* pszNew, const char *pszBefore, const char *pszAfter);
	bool Update3DMotion(void);
	bool UpdateGUI(void);
private:
	bool Load3DMotionIni(std::string strFilePath, std::map<__int64, std::string>& mapData);
	bool Save3DMotionIni(std::string strFilePath, const::std::map<__int64, std::string>& mapData);
private:
	std::string m_strEnvOld;
	std::string m_strEnvNew;
	std::string m_strEnvBefore;
	std::string m_strEnvAfter;
};
