#pragma once
#include <map>
#include <string>
#include <vector>

#include "BaseCode/Singleton.h"

class CUpdateMgr : public Singleton<CUpdateMgr>
{
private:
	enum { TYPE_3DMOTION = 1, TYPE_GUI, };
public:
	CUpdateMgr(void);
	~ CUpdateMgr(void);

	void SetEnvPath(const char *pszOld, const char *pszNew, const char *pszBefore,
					const char *pszAfter);

	bool UpdateFile(const char *pszFile, int nType);
	bool UpdateGUILike(const char *pszFile);
	bool Update3DMotionLike(const char *pszFile);

	bool Update3DMotion(void);
	bool UpdateGUI(void);
private:
	bool Load3DMotionIni(std::string strFilePath, 
		std::map < __int64, std::string > &mapData);
	bool Save3DMotionIni(std::string strFilePath, const::std::map < __int64, std::string > &mapData);

	bool LoadGUIIni(std::string strFilePath, std::map < std::string, std::vector < std::string > > &mapData);
	bool SaveGUIIni(std::string strFilePath,
					const std::map<std::string, std::vector<std::string> > &mapData);
private:
	std::string m_strEnvOld;
	std::string m_strEnvNew;
	std::string m_strEnvBefore;
	std::string m_strEnvAfter;
};
