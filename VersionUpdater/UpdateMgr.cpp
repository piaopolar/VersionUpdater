#include "StdAfx.h"

#include <algorithm>

#include "BaseCode/BaseFunc.h"
#include "UpdateMgr.h"

namespace
{
const char *INI_3DMOTION = "3dmotion.ini";
const char *INI_GUI = "gui.ini";
std::string COMMENT_PREFIX = "comment=";
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
template<typename INDEX, typename DATA>
void Update(const std::map<INDEX, DATA> &mapOld,
			const std::map<INDEX, DATA> &mapNew,
			const std::map<INDEX, DATA> &mapBefore,
			OUT std::map<INDEX, DATA> &rMapAfter)
{
	std::map<INDEX, DATA> mapChg;
	std::map<INDEX, DATA> mapAdd;

	std::map<INDEX, DATA>::const_iterator itNew = mapNew.begin();
	std::map<INDEX, DATA>::const_iterator itOld = mapOld.begin();

	int nCount = 0;
	int nMulti = 0;
	int nCheckPoint = 10000;
	while (itNew != mapNew.end() && itOld != mapOld.end()) {

		if (nCount >= nCheckPoint) {
			++nMulti;
			nCount = 0;
			LogInfoIn("ana %d  vecChg %d vecAdd %d  sum %d", nMulti * nCheckPoint, mapChg.size(), mapAdd.size(), mapChg.size() + mapAdd.size());
		}

		if (itNew->first == itOld->first) {
			if (itOld->second != itNew->second) {
				mapChg[itNew->first] = itNew->second;
			}

			++nCount;
			++itNew;
			++itOld;
// 			if (nCount % 10000 == 0) {
// 				LogInfoIn("ana %d/%d", nCount, mapNew.size());
// 			}

			continue;
		}

		if (itNew->first < itOld->first) {

			mapAdd[itNew->first] = itNew->second;
			++nCount;
			++itNew;
// 			if (nCount % 10000 == 0) {
// 				LogInfoIn("ana %d/%d", nCount, mapNew.size());
// 			}

			continue;
		}

		if (itNew->first > itOld->first) {
			++itOld;
		}
	}

	while (itNew != mapNew.end()) {

		mapAdd[itNew->first] = itNew->second;

		++nCount;
		++itNew;
// 		if (nCount % 10000 == 0) {
// 			LogInfoIn("ana %d/%d", nCount, mapNew.size());
// 		}
	}

	LogInfoIn("ana ok add num %d chg num %d", mapAdd.size(), mapChg.size());

	rMapAfter = mapBefore;

	nCount = 0;

	for (std::map<INDEX, DATA>::iterator itAfter = rMapAfter.begin();
		 itAfter != rMapAfter.end(); ++itAfter) {

		 ++nCount;
		 if (nCount % 10000 == 0) {
			 LogInfoIn("replace %d/%d", nCount, mapNew.size());
		 }

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		INDEX key = itAfter->first;
		std::map<INDEX, DATA>::const_iterator itChg = mapChg.find(key);

		if (itChg != mapChg.end()) {
			itAfter->second = itChg->second;
		}
	}

	LogInfoIn("replace ok", nCount, mapNew.size());

	for (std::map<INDEX, DATA>::const_iterator itAdd =
			 mapAdd.begin(); itAdd != mapAdd.end(); ++itAdd) {
		rMapAfter[itAdd->first] = itAdd->second;
	}

	LogInfoIn("add ok", nCount, mapNew.size());
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
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<std::string, std::vector<std::string> > mapOld;
	std::map<std::string, std::vector<std::string> > mapNew;
	std::map<std::string, std::vector<std::string> > mapBefore;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	this->LoadGUIIni(m_strEnvOld + INI_GUI, mapOld);
	this->LoadGUIIni(m_strEnvNew + INI_GUI, mapNew);
	this->LoadGUIIni(m_strEnvBefore + INI_GUI, mapBefore);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<std::string, std::vector<std::string>> mapAfter;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Update(mapOld, mapNew, mapBefore, mapAfter);

	this->SaveGUIIni(m_strEnvAfter + INI_GUI, mapAfter);
	
	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Load3DMotionIni(std::string strFilePath,
								 std::map<__int64, std::string> &mapData)
{
	LogInfoIn("open %s", strFilePath.c_str());

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "r");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		return false;
	}

	LogInfoIn("open %s ok", strFilePath.c_str());

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
	LogInfoIn("open %s", strFilePath.c_str());

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "r");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		return false;
	}

	LogInfoIn("open %s ok", strFilePath.c_str());

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
	LogInfoIn("open %s", strFilePath.c_str());

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "w");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		return false;
	}

	LogInfoIn("open %s ok", strFilePath.c_str());

	for (std::map < __int64, std::string >::const_iterator itData = mapData.
			 begin(); itData != mapData.end(); ++itData) {
		 if (itData->first >= 1000000000000) {
			 fprintf(pFile, "%014I64d=%s\n", itData->first, itData->second.c_str());
		 } else {
			 fprintf(pFile, "%011I64d=%s\n", itData->first, itData->second.c_str());
		 }
	}

	fclose(pFile);
	return true;
}

bool CUpdateMgr::SaveGUIIni( std::string strFilePath, const std::map<std::string, std::vector<std::string> > & mapData )
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "w");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		return false;
	}

	std::map<std::string, std::vector<std::string> >::const_iterator itSection = mapData.begin();
	for (; itSection != mapData.end(); ++itSection) {
		const std::vector<std::string>& rVecValue = itSection->second;
		std::vector<std::string>::const_iterator itValue = rVecValue.begin();
		if (itValue != rVecValue.end()) {
			std::string strFirst = *itValue;
			std::string::size_type posStr = strFirst.find(COMMENT_PREFIX);
			if (posStr != std::string::npos) {
				fprintf(pFile, "%s\n", strFirst.substr(posStr + COMMENT_PREFIX.length()).c_str());
				++itValue;
			}
		}

		fprintf(pFile, "[%s]\n", itSection->first.c_str());
		for (; itValue != rVecValue.end(); ++itValue) {
			fprintf(pFile, "%s\n", itValue->c_str());
		}
		fprintf(pFile, "\n");
	}

	fclose(pFile);
	return true;
}