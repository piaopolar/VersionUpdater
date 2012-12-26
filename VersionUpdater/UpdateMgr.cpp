#include "StdAfx.h"

#include <algorithm>

#include "boost/regex.hpp"

#include "BaseCode/BaseFunc.h"
#include "UpdateMgr.h"

namespace
{
const char *INI_3DMOTION = "3dmotion.ini";
const char *INI_GUI = "gui.ini";
const char *INI_GUI800 = "gui800X600.ini";
const char *COMMENT_PREFIX = "comment=";

// ============================================================================
// ==============================================================================

std::string GetOutFilePath(std::string strFilePath)
{
	FormatPath(strFilePath);
	boost::regex expPath("(.*)\\\\(.*)\\\\(.*\\.\\w\\w\\w)$");

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::string strOutFilePath = boost::regex_replace(strFilePath, expPath, "$1\\\\客户端更新\\\\$2\\\\$3");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	return strOutFilePath;
}

// ============================================================================
// ==============================================================================
std::string GetAddFilePath(std::string strFilePath)
{
	FormatPath(strFilePath);
	boost::regex expPath("(.*)\\\\(.*)\\\\(.*\\.\\w\\w\\w)$");

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::string strAddFilePath = boost::regex_replace(strFilePath, expPath, "$1\\\\+$2\\\\+$3");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	return strAddFilePath;
}
}

// ============================================================================
// ==============================================================================
CUpdateMgr::CUpdateMgr(void)
{
	m_bUnicodeMode = false;
}

// ============================================================================
// ==============================================================================
CUpdateMgr::~CUpdateMgr(void)
{
}

// ============================================================================
// ==============================================================================
void CUpdateMgr::SetEnvPath(const char *pszOld, const char *pszNew, const char *pszBefore, const char *pszAfter)
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
				mapChg.insert(std::map<INDEX, DATA>::value_type(itNew->first, itNew->second));
			}

			++itNew;
			++itOld;

			continue;
		}

		if (itNew->first < itOld->first) {
			mapAdd.insert(std::map<INDEX, DATA>::value_type(itNew->first, itNew->second));
			++itNew;

			continue;
		}

		if (itNew->first > itOld->first) {
			++itOld;
		}
	}

	while (itNew != mapNew.end()) {
		mapAdd.insert(std::map<INDEX, DATA>::value_type(itNew->first, itNew->second));
		++itNew;
	}

	LogInfoIn("ana ok chg %d add %d", mapChg.size(), mapAdd.size());

	rMapAfter = mapBefore;

	//~~~~~~~~~~~~~~~~~~~~
	int nReadySameCount = 0;
	//~~~~~~~~~~~~~~~~~~~~

	for (std::map<INDEX, DATA>::iterator itAfter = rMapAfter.begin(); itAfter != rMapAfter.end(); ++itAfter) {

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		INDEX key = itAfter->first;
		std::map<INDEX, DATA>::const_iterator itChg = mapChg.find(key);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (itChg != mapChg.end()) {
			if (itAfter->second == itChg->second) {
				++nReadySameCount;
			} else {
				itAfter->second = itChg->second;
				rMapAllChg.insert(*itChg);
			}
		}
	}

	LogInfoIn("  replace ok, total %d, replaced:%d, ingore:%d, unfound:%d", mapChg.size(), rMapAllChg.size(),
			  nReadySameCount, mapChg.size() - rMapAllChg.size() - nReadySameCount);

	nReadySameCount = 0;

	for (std::map<INDEX, DATA>::const_iterator itAdd = mapAdd.begin(); itAdd != mapAdd.end(); ++itAdd) {
		if (rMapAfter[itAdd->first] == itAdd->second) {
			++nReadySameCount;
		} else {
			rMapAfter[itAdd->first] = itAdd->second;
			rMapAllChg.insert(*itAdd);
		}
	}

	LogInfoIn("  add ok, total %d, added %d, ingore %d", mapAdd.size(), mapAdd.size() - nReadySameCount, nReadySameCount);
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

	if (MODE_ADD == m_nMode) {
		if (!this->Load3DMotionIni(m_strEnvBefore + pszFile, mapOld)) {
			return false;
		}

		if (!this->Load3DMotionIni(GetAddFilePath(m_strEnvBefore + pszFile), mapNew)) {
			return false;
		}
	} else {
		if (!this->Load3DMotionIni(m_strEnvOld + pszFile, mapOld)) {
			return false;
		}

		if (!this->Load3DMotionIni(m_strEnvNew + pszFile, mapNew)) {
			return false;
		}
	}

	if (!this->Load3DMotionIni(m_strEnvBefore + pszFile, mapBefore)) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<__int64, std::string> mapAfter;
	std::map<__int64, std::string> mapAllChg;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Update(mapOld, mapNew, mapBefore, mapAfter, mapAllChg);

	if (!this->Save3DMotionIni(GetOutFilePath(m_strEnvAfter + pszFile), mapAfter)) {
		return false;
	}

	if (!this->Save3DMotionIni(GetAddFilePath(m_strEnvAfter + pszFile), mapAllChg)) {
		return false;
	}

	return true;
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

	if (MODE_ADD == m_nMode) {
		if (!this->LoadGUIIni(m_strEnvBefore + pszFile, mapOld)) {
			return false;
		}

		if (!this->LoadGUIIni(GetAddFilePath(m_strEnvBefore + pszFile), mapNew)) {
			return false;
		}
	} else {
		if (!this->LoadGUIIni(m_strEnvOld + pszFile, mapOld)) {
			return false;
		}

		if (!this->LoadGUIIni(m_strEnvNew + pszFile, mapNew)) {
			return false;
		}
	}

	if (!this->LoadGUIIni(m_strEnvBefore + pszFile, mapBefore)) {
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<std::string, std::vector<std::string> > mapAfter;
	std::map<std::string, std::vector<std::string> > mapAllChg;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	Update(mapOld, mapNew, mapBefore, mapAfter, mapAllChg);

	if (!this->SaveGUIIni(GetOutFilePath(m_strEnvAfter + pszFile), mapAfter)) {
		return false;
	}

	if (!this->SaveGUIIni(GetAddFilePath(m_strEnvAfter + pszFile), mapAllChg)) {
		return false;
	}

	return true;
}

// ============================================================================
// ==============================================================================
void Parse3DMotionLike(char szLine[], std::map<__int64, std::string> &mapData)
{
	TrimRight(szLine);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char *pPos = strstr(szLine, "=");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pPos) {
		return;
	}

	*pPos = 0;

	//~~~~~~~~~~~
	__int64 i64Key;
	//~~~~~~~~~~~

	if (1 != sscanf(szLine, "%I64d", &i64Key)) {
		return;
	}

	//~~~~~~~~~~~~~~~~~~~~
	char szPath[MAX_STRING];
	//~~~~~~~~~~~~~~~~~~~~

	sscanf(pPos + 1, "%s", szPath);
	mapData[i64Key] = szPath;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Load3DMotionIni(std::string strFilePath, std::map<__int64, std::string> &mapData)
{
	//~~~~~~~~~~~~~~~~~~~~
	std::string strCCS = "";
	//~~~~~~~~~~~~~~~~~~~~

	if (GetFileCode(strFilePath) > 0) {
		m_bUnicodeMode = true;
		strCCS += ", ccs=unicode";
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~
	std::string strOpenFmt = "r";
	//~~~~~~~~~~~~~~~~~~~~~~~~~

	strOpenFmt += strCCS;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), strOpenFmt.c_str());
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		LogInfoIn("!!!!! open %s error", strFilePath.c_str());
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char szLine[MAX_STRING];
	std::string strKey;
	std::vector<std::string> vecSection;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (m_bUnicodeMode) {

		//~~~~~~~~~~~~~~~~~~~~~~~~
		wchar_t wszLine[MAX_STRING];
		//~~~~~~~~~~~~~~~~~~~~~~~~

		while (fgetws(wszLine, sizeof(wszLine), pFile)) {
			UTF16_2_ANSI(wszLine, szLine, sizeof(szLine));
			Parse3DMotionLike(szLine, mapData);
		}
	} else {
		while (fgets(szLine, sizeof(szLine), pFile)) {
			Parse3DMotionLike(szLine, mapData);
		}
	}

	fclose(pFile);

	LogInfoIn("load %s ok section %d", strFilePath.c_str(), mapData.size());
	return true;
}

// ============================================================================
// ==============================================================================
void ParseGUILine(char szLine[],
				  std::string &strKey,
				  std::string &strComment,
				  std::vector<std::string> &vecSection,
				  std::map<std::string, std::vector<std::string> > &mapData)
{
	TrimRight(szLine);

	//~~~~~~~
	char *pPos;
	//~~~~~~~

	// comment
	pPos = strstr(szLine, COMMENT_PREFIX);
	if (pPos) {

		//~~~~~~~~~~~~~~~~~~~~~~~~
		std::string strTmp = szLine;
		//~~~~~~~~~~~~~~~~~~~~~~~~

		if (strTmp.find("comment=/") != std::string::npos || strTmp.find("comment= ") != std::string::npos) {
			ReplaceStdString(strTmp, "comment=/", COMMENT_PREFIX);
			ReplaceStdString(strTmp, "comment= ", COMMENT_PREFIX);
		}

		strncpy(szLine, strTmp.c_str(), sizeof(szLine));
	} else {
		pPos = strstr(szLine, "//");
		if (pPos) {
			strComment = std::string(COMMENT_PREFIX) + pPos;
		}
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

// ============================================================================
// ==============================================================================
bool CUpdateMgr::LoadGUIIni(std::string strFilePath, std::map<std::string, std::vector<std::string> > &mapData)
{
	//~~~~~~~~~~~~~~~~~~~~
	std::string strCCS = "";
	//~~~~~~~~~~~~~~~~~~~~

	if (GetFileCode(strFilePath) > 0) {
		m_bUnicodeMode = true;
		strCCS += ", ccs=unicode";
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~
	std::string strOpenFmt = "r";
	//~~~~~~~~~~~~~~~~~~~~~~~~~

	strOpenFmt += strCCS;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), strOpenFmt.c_str());
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		LogInfoIn("!!!!! open %s error", strFilePath.c_str());
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char szLine[MAX_STRING];
	std::string strKey;
	std::string strComment;
	std::vector<std::string> vecSection;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (m_bUnicodeMode) {

		//~~~~~~~~~~~~~~~~~~~~~~~~
		wchar_t wszLine[MAX_STRING];
		//~~~~~~~~~~~~~~~~~~~~~~~~

		while (fgetws(wszLine, sizeof(wszLine), pFile)) {
			UTF16_2_ANSI(wszLine, szLine, sizeof(szLine));
			ParseGUILine(szLine, strKey, strComment, vecSection, mapData);
		}
	} else {
		while (fgets(szLine, sizeof(szLine), pFile)) {
			ParseGUILine(szLine, strKey, strComment, vecSection, mapData);
		}
	}

	if (!strKey.empty()) {
		mapData[strKey] = vecSection;
	}

	fclose(pFile);

	LogInfoIn("open %s ok section %d", strFilePath.c_str(), mapData.size());
	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::Save3DMotionIni(std::string strFilePath, const std::map<__int64, std::string> &mapData)
{
	if (mapData.empty()) {
		return true;
	}

	MyMakeSureDirectoryPathExists(strFilePath);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "w");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		LogInfoIn("!!!!! create %s error", strFilePath.c_str());
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bool b3DMotion = strstr(strFilePath.c_str(), "3dmotion.ini") ? true : false;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (b3DMotion) {
		for (std::map < __int64, std::string >::const_iterator itData = mapData.begin(); itData != mapData.end();
			 ++itData) {
			if (itData->first >= 1000000000000) {
				fprintf(pFile, "%014I64d=%s\n", itData->first, itData->second.c_str());
			} else {
				fprintf(pFile, "%011I64d=%s\n", itData->first, itData->second.c_str());
			}
		}
	} else {
		for (std::map < __int64, std::string >::const_iterator itData = mapData.begin(); itData != mapData.end();
			 ++itData) {
			fprintf(pFile, "%I64d=%s\n", itData->first, itData->second.c_str());
		}
	}

	fclose(pFile);
	LogInfoIn("***** save %s ok section %d", strFilePath.c_str(), mapData.size());
	return true;
}

// ============================================================================
// ==============================================================================
bool CUpdateMgr::SaveGUIIni(std::string strFilePath, const std::map<std::string, std::vector<std::string> > &mapData)
{
	if (mapData.empty()) {
		return true;
	}

	MyMakeSureDirectoryPathExists(strFilePath);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	FILE *pFile = fopen(strFilePath.c_str(), "w");
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (NULL == pFile) {
		LogInfoIn("!!!!! create %s error", strFilePath.c_str());
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	std::map<std::string, std::vector<std::string> >::const_iterator itSection = mapData.begin();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

				//~~~~~~~~~~~~~~~~~~~~~~~~~~
				std::string strComment = "//";
				//~~~~~~~~~~~~~~~~~~~~~~~~~~

				strComment += strFirst.substr(posStr + strlen(COMMENT_PREFIX));
				while (strComment.find("///") != std::string::npos) {
					ReplaceStdString(strComment, "///", "//");
				}

				fprintf(pFile, "%s\n", strComment.c_str());
				++itValue;
			}
		}

		fprintf(pFile, "[%s]\n", itSection->first.c_str());
		for (; itValue != rVecValue.end(); ++itValue) {

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			std::string strValue = *itValue;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			std::string::size_type posStr = strValue.find(COMMENT_PREFIX);
			if (posStr != std::string::npos) {
				continue;
			}

			fprintf(pFile, "%s\n", strValue.c_str());
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
	LogInfoIn("***** save %s section %d", strFilePath.c_str(), mapData.size());
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

// ============================================================================
// ==============================================================================
void CUpdateMgr::SetMode(int nMode)
{
	m_nMode = nMode;
}
