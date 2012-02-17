#include "StdAfx.h"

#include <algorithm>

#include "BaseCode/BaseFunc.h"
#include "UpdateMgr.h"

namespace
{
const char *INI_3DMOTION = "3dmotion.ini";
const char *INI_GUI = "gui.ini";
}

// ============================================================================
// ==============================================================================

CUpdateMgr::CUpdateMgr(void)
{
}

// ============================================================================
// ==============================================================================
CUpdateMgr::~CUpdateMgr(void)
{
}

// ============================================================================
// ==============================================================================
void CUpdateMgr::SetEnvPath(const char *pszOld,
							const char *pszNew,
							const char *pszBefore,
							const char *pszAfter)
{
	m_strEnvOld = pszOld;
	m_strEnvNew = pszNew;
	m_strEnvBefore = pszBefore;
	m_strEnvAfter = pszAfter;
	FormatPath(m_strEnvOld);
	FormatPath(m_strEnvNew);
	FormatPath(m_strEnvBefore);
	FormatPath(m_strEnvAfter);
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Update3DMotion(void)
{
	std::map<__int64, std::string> mapOld;
	std::map<__int64, std::string> mapNew;
	std::map<__int64, std::string> mapBefore;
	this->Load3DMotionIni(m_strEnvOld + INI_3DMOTION, mapOld);
	this->Load3DMotionIni(m_strEnvNew + INI_3DMOTION, mapNew);
	this->Load3DMotionIni(m_strEnvBefore + INI_3DMOTION, mapBefore);

	std::vector<__int64> vecChg;
	std::vector<__int64> vecAdd;
	for (std::map<__int64, std::string>::const_iterator itNew = mapNew.begin(); itNew != mapNew.end(); ++itNew) {
		__int64 i64Key = itNew->first;
		std::map<__int64, std::string>::const_iterator itOld = mapOld.find(i64Key);
		if (itOld == mapOld.end()) {
			vecAdd.push_back(i64Key);
			continue;
		}

		if (itOld->second != itNew->second) {
			vecChg.push_back(i64Key);
		}
	}

	std::map<__int64, std::string> mapAfter = mapBefore;

	for (std::map<__int64, std::string>::iterator itAfter = mapAfter.begin(); itAfter != mapAfter.end(); ++itAfter) {
		__int64 i64Key = itAfter->first;
		if (std::find(vecChg.begin(), vecChg.end(), i64Key) != vecChg.end()) {
			itAfter->second = mapNew[i64Key];
		}
	}

	for (std::vector<__int64>::const_iterator itAdd = vecAdd.begin(); itAdd !=  vecAdd.end(); ++itAdd) {
		__int64 i64Key = *itAdd;
		mapAfter[i64Key] = mapNew[i64Key];
	}

	this->Save3DMotionIni(m_strEnvAfter + INI_3DMOTION, mapAfter);

	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::UpdateGUI(void)
{
	return true;
}

bool CUpdateMgr::Load3DMotionIni( std::string strFilePath, std::map<__int64, std::string>& mapData )
{
	FILE* pFile = fopen(strFilePath.c_str(), "r");
	if (NULL == pFile) {
		return false;
	}

	char szLine[MAX_STRING];
	while (fgets(szLine, sizeof(szLine), pFile)) {
		char* pPos = strstr(szLine, "=");
		if (NULL == pPos) {
			continue;
		}

		*pPos = 0;

		__int64 i64Key;
		if (1 != sscanf(szLine, "%I64d", &i64Key)) {
			continue;
		}

		char szPath[MAX_STRING];
		sscanf(pPos + 1, "%s", szPath);
		std::string strPath = szPath;
		std::replace(strPath.begin(), strPath.end(), '\n', '\0');
		std::replace(strPath.begin(), strPath.end(), '\r', '\0');
		mapData[i64Key] = strPath;
	}

	fclose(pFile);
	return true;
}

bool CUpdateMgr::Save3DMotionIni( std::string strFilePath, const::std::map<__int64, std::string>& mapData )
{
	FILE *pFile = fopen(strFilePath.c_str(), "w");
	if (NULL == pFile) {
		return false;
	}

	for (std::map<__int64, std::string>::const_iterator itData = mapData.begin(); itData != mapData.end(); ++itData) {
		fprintf(pFile, "%011I64d=%s\n", itData->first, itData->second.c_str());
	}

	fclose(pFile);
	return true;
}
