#pragma once

#include "BaseCode/Singleton.h"

#include <string>

class CUpdateMgr : Singleton<CUpdateMgr>
{
public:
	CUpdateMgr(void);
	~ CUpdateMgr(void);


	void SetEnvPath(const char* pszOld, const char* pszNew, const char *pszBefore, const char *pszAfter);
	bool Update3DMotion(void);
	bool UpdateGUI(void);
private:
	std::string m_strEnvOld;
	std::string m_strEnvNew;
	std::string m_strEnvBefore;
	std::string m_strEnvAfter;
};
