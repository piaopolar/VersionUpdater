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
template<typename INDEX, typename VALUE>
void Update(const std::map<INDEX, VALUE> &mapOld,
			const std::map<INDEX, VALUE> &mapNew,
			const std::map<INDEX, VALUE> &mapBefore,
			OUT std::map<INDEX, VALUE> &rMapAfter)
{
	//~~~~~~~~~~~~~~~~~~~~~~
	std::vector<INDEX> vecChg;
	std::vector<INDEX> vecAdd;
	//~~~~~~~~~~~~~~~~~~~~~~

	for (std::map<INDEX, VALUE>::const_iterator itNew = mapNew.begin();
		 itNew != mapNew.end(); ++itNew) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		INDEX key = itNew->first;
		std::map<INDEX, VALUE>::const_iterator itOld = mapOld.find(key);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (itOld == mapOld.end()) {
			vecAdd.push_back(key);
			continue;
		}

		if (itOld->second != itNew->second) {
			vecChg.push_back(key);
		}
	}

	rMapAfter = mapBefore;

	for (std::map<INDEX, VALUE>::iterator itAfter = rMapAfter.begin();
		 itAfter != rMapAfter.end(); ++itAfter) {

		//~~~~~~~~~~~~~~~~~~~~~~~
		INDEX key = itAfter->first;
		//~~~~~~~~~~~~~~~~~~~~~~~

		if (std::find(vecChg.begin(), vecChg.end(), key) != vecChg.end()) {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			std::map<INDEX, VALUE>::const_iterator itNew = mapNew.find(key);
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			if (itNew != mapNew.end()) {
				itAfter->second = itNew->second;
			}
		}
	}

	for (std::vector<INDEX>::const_iterator itAdd = vecAdd.begin();
		 itAdd != vecAdd.end(); ++itAdd) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		INDEX key = *itAdd;
		std::map<INDEX, VALUE>::const_iterator itNew = mapNew.find(key);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (itNew != mapNew.end()) {
			rMapAfter[key] = itNew->second;
		}
	}
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Update3DMotion(void)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<__int64, std::string> mapOld;
	std::map<__int64, std::string> mapNew;
	std::map<__int64, std::string> mapBefore;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	this->Load3DMotionIni(m_strEnvOld + INI_3DMOTION, mapOld);
	this->Load3DMotionIni(m_strEnvNew + INI_3DMOTION, mapNew);
	this->Load3DMotionIni(m_strEnvBefore + INI_3DMOTION, mapBefore);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<__int64, std::string> mapAfter;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Update(mapOld, mapNew, mapBefore, mapAfter);

	this->Save3DMotionIni(m_strEnvAfter + INI_3DMOTION, mapAfter);

	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::UpdateGUI(void)
{
	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Load3DMotionIni(std::string strFilePath,
								 std::map<__int64, std::string> &mapData)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "r");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~
	char szLine[MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~

	while (fgets(szLine, sizeof(szLine), pFile)) {
		TrimRight(szLine);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		char *pPos = strstr(szLine, "=");
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (NULL == pPos) {
			continue;
		}

		*pPos = 0;

		//~~~~~~~~~~~
		__int64 i64Key;
		//~~~~~~~~~~~

		if (1 != sscanf(szLine, "%I64d", &i64Key)) {
			continue;
		}

		//~~~~~~~~~~~~~~~~~~~~
		char szPath[MAX_STRING];
		//~~~~~~~~~~~~~~~~~~~~

		sscanf(pPos + 1, "%s", szPath);
		mapData[i64Key] = szPath;
	}

	fclose(pFile);
	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::LoadGUIIni(std::string strFilePath,
							std::map<std::string, std::vector<std::string> > &mapData)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "r");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	const std::string COMMENT_PREFIX = "comment=";
	char szLine[MAX_STRING];
	char szTmp[MAX_STRING];
	std::string strKey;
	std::string strComment;
	std::vector<std::string> vecSection;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	while (fgets(szLine, sizeof(szLine), pFile)) {
		TrimRight(szLine);

		//~~~~~~~
		char *pPos;
		//~~~~~~~

		// comment
		pPos = strstr(szLine, "//");
		if (pPos) {
			strComment = COMMENT_PREFIX + pPos;
		}

		// a new section
		if (szLine[0] == '[' && (pPos = strstr(szLine, "]"))) {
			if (!strKey.empty()) {

				// save old section
				mapData[strKey] = vecSection;
			}

			*pPos = 0;
			strKey = szLine + 1;
			vecSection.clear();

			if (!strComment.empty()) {
				vecSection.push_back(strComment);
				strComment.clear();
			}
		}

		if (strstr(szLine, "=")) {
			vecSection.push_back(szLine);
		}
	}

	mapData[strKey] = vecSection;

	fclose(pFile);
	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Save3DMotionIni(std::string strFilePath,
								 const::std::map<__int64, std::string> &mapData)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "w");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		return false;
	}

	for (std::map < __int64, std::string >::const_iterator itData = mapData.
			 begin(); itData != mapData.end(); ++itData) {
		fprintf(pFile, "%011I64d=%s\n", itData->first, itData->second.c_str());
	}

	fclose(pFile);
	return true;
}
