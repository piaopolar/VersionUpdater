#include "StdAfx.h"

#include <algorithm>

#include "BaseCode/BaseFunc.h"
#include "UpdateMgr.h"

namespace
{
const char *INI_3DMOTION = "3dmotion.ini";
const char *INI_GUI = "gui.ini";
const char *INI_GUI800 = "gui800X600.ini";
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
			OUT std::map<INDEX, DATA> &rMapAfter,
			OUT std::map<INDEX, DATA> &rMapAllChg)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<INDEX, DATA> mapChg;
	std::map<INDEX, DATA> mapAdd;
	std::map<INDEX, DATA>::const_iterator itNew = mapNew.begin();
	std::map<INDEX, DATA>::const_iterator itOld = mapOld.begin();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	while (itNew != mapNew.end() && itOld != mapOld.end()) {
		if (itNew->first == itOld->first) {
			if (itOld->second != itNew->second) {
				mapChg.insert(std::map<INDEX, DATA>::value_type(itNew->first,
							  itNew->second));
				rMapAllChg.insert(std::map<INDEX, DATA>::value_type(
									  itNew->first, itNew->second));
			}

			++itNew;
			++itOld;

			continue;
		}

		if (itNew->first < itOld->first) {
			mapAdd.insert(std::map<INDEX, DATA>::value_type(itNew->first,
						  itNew->second));
			rMapAllChg.insert(std::map<INDEX, DATA>::value_type(itNew->first,
							  itNew->second));
			++itNew;

			continue;
		}

		if (itNew->first > itOld->first) {
			++itOld;
		}
	}

	while (itNew != mapNew.end()) {
		mapAdd.insert(std::map<INDEX, DATA>::value_type(itNew->first, itNew->second));
		rMapAllChg.insert(std::map<INDEX, DATA>::value_type(itNew->first, itNew->second));
		++itNew;
	}

	LogInfoIn("ana ok add %d chg %d", mapAdd.size(), mapChg.size());

	rMapAfter = mapBefore;

	for (std::map<INDEX, DATA>::iterator itAfter = rMapAfter.begin();
		 itAfter != rMapAfter.end(); ++itAfter) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		INDEX key = itAfter->first;
		std::map<INDEX, DATA>::const_iterator itChg = mapChg.find(key);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (itChg != mapChg.end()) {
			itAfter->second = itChg->second;
		}
	}

	LogInfoIn("replace ok");

	for (std::map<INDEX, DATA>::const_iterator itAdd = mapAdd.begin();
		 itAdd != mapAdd.end(); ++itAdd) {
		rMapAfter[itAdd->first] = itAdd->second;
	}

	LogInfoIn("add ok");
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Update3DMotionLike(const char *pszFile)
{
	if (NULL == pszFile) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<__int64, std::string> mapOld;
	std::map<__int64, std::string> mapNew;
	std::map<__int64, std::string> mapBefore;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (!this->Load3DMotionIni(m_strEnvOld + pszFile, mapOld)) {
		return false;
	}

	if (!this->Load3DMotionIni(m_strEnvNew + pszFile, mapNew)) {
		return false;
	}

	if (!this->Load3DMotionIni(m_strEnvBefore + pszFile, mapBefore)) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<__int64, std::string> mapAfter;
	std::map<__int64, std::string> mapAllChg;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Update(mapOld, mapNew, mapBefore, mapAfter, mapAllChg);

	if (!this->Save3DMotionIni(m_strEnvAfter + pszFile, mapAfter)) {
		return false;
	}

	if (!this->Save3DMotionIni(m_strEnvAfter + "+" + pszFile, mapAllChg)) {
		return false;
	}

	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Update3DMotion(void)
{
	return this->Update3DMotionLike(INI_3DMOTION);
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::UpdateGUILike(const char *pszFile)
{
	if (NULL == pszFile) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<std::string, std::vector<std::string> > mapOld;
	std::map<std::string, std::vector<std::string> > mapNew;
	std::map<std::string, std::vector<std::string> > mapBefore;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (!this->LoadGUIIni(m_strEnvOld + pszFile, mapOld)) {
		return false;
	}

	if (!this->LoadGUIIni(m_strEnvNew + pszFile, mapNew)) {
		return false;
	}

	if (!this->LoadGUIIni(m_strEnvBefore + pszFile, mapBefore)) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<std::string, std::vector<std::string> > mapAfter;
	std::map<std::string, std::vector<std::string> > mapAllChg;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Update(mapOld, mapNew, mapBefore, mapAfter, mapAllChg);

	if (!this->SaveGUIIni(m_strEnvAfter + pszFile, mapAfter)) {
		return false;
	}

	if (!this->SaveGUIIni(m_strEnvAfter + "+" + pszFile, mapAllChg)) {
		return false;
	}

	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::UpdateGUI(void)
{
	//~~~~~~~~~~~~~
	bool bRet = true;
	//~~~~~~~~~~~~~

	bRet = bRet && this->UpdateGUILike(INI_GUI);
	bRet = bRet && this->UpdateGUILike(INI_GUI800);
	return bRet;
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
		LogInfoIn("!!!!! open %s error", strFilePath.c_str());
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

	LogInfoIn("load %s ok", strFilePath.c_str());
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
		LogInfoIn("!!!!! open %s error", strFilePath.c_str());
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char szLine[MAX_STRING];
	char szTmp[MAX_STRING];
	std::string strKey;
	std::string strComment;
	std::vector<std::string> vecSection;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

	LogInfoIn("open %s ok", strFilePath.c_str());
	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Save3DMotionIni(std::string strFilePath,
								 const std::map<__int64, std::string> &mapData)
{
	MyMakeSureDirectoryPathExists(strFilePath);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "w");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		LogInfoIn("!!!!! create %s error", strFilePath.c_str());
		return false;
	}

	for (std::map < __int64, std::string >::const_iterator itData = mapData.
			 begin(); itData != mapData.end(); ++itData) {
		if (itData->first >= 1000000000000) {
			fprintf(pFile, "%014I64d=%s\n", itData->first,
					itData->second.c_str());
		} else {
			fprintf(pFile, "%011I64d=%s\n", itData->first,
					itData->second.c_str());
		}
	}

	fclose(pFile);
	LogInfoIn("***** save %s ok", strFilePath.c_str());
	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::SaveGUIIni(std::string strFilePath,
							const std::map<std::string, std::vector<std::string> > &mapData)
{
	MyMakeSureDirectoryPathExists(strFilePath);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "w");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		LogInfoIn("!!!!! create %s error", strFilePath.c_str());
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<std::string, std::vector<std::string> >::const_iterator itSection =
		mapData.begin();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	for (; itSection != mapData.end(); ++itSection) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		const std::vector<std::string> &rVecValue = itSection->second;
		std::vector<std::string>::const_iterator itValue = rVecValue.begin();
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (itValue != rVecValue.end()) {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			std::string strFirst = *itValue;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			std::string::size_type posStr = strFirst.find(COMMENT_PREFIX);
			if (posStr != std::string::npos) {
				fprintf(pFile, "%s\n",
						strFirst.substr(posStr + COMMENT_PREFIX.length()).c_str());
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
	LogInfoIn("***** save %s", strFilePath.c_str());
	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::UpdateFile(const char *pszFile, int nType)
{
	switch (nType) {
	case TYPE_3DMOTION: return this->Update3DMotionLike(pszFile); break;
	case TYPE_GUI:		return this->UpdateGUILike(pszFile); break;
	default:			break;
	}

	return false;
}
