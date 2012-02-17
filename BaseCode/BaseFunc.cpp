#include "stdafx.h"

#include "BaseFunc.h"

// ============================================================================
// ==============================================================================

void FormatPath(std::string &strPath)
{
	if (strPath.empty()) {
		return;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	char cLast = strPath.at(strPath.length() - 1);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	switch (cLast) {
	case '\\':
	case '/':
		break;
	default:
		strPath += "/";
		break;
	}
}

// ============================================================================
// ==============================================================================
void TrimRight(char *pszStr)
{
	if (NULL == pszStr) {
		return;
	}

	//~~~~~~~~~~~~~~~~~~~~~~
	int nLen = strlen(pszStr);
	char *pLast;
	//~~~~~~~~~~~~~~~~~~~~~~

	for (pLast = pszStr + nLen - 1; pszStr >= pszStr; --pLast) {

		//~~~~~~~~~~~~~~
		bool bRid = false;
		//~~~~~~~~~~~~~~

		switch (*pLast) {
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			bRid = true;
			break;
		default:
			break;
		}

		if (!bRid) {
			break;
		}
	}
	*(pLast + 1) = 0;
}
