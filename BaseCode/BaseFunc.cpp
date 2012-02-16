#include "stdafx.h"

#include "BaseFunc.h"

void FormatPath( std::string &strPath )
{
	if (strPath.empty()) {
		return;
	}

	char cLast = strPath.at(strPath.length() - 1);
	switch (cLast) {
	case '\\':
	case '/':
		break;
	default:
		strPath += "/";
		break;
	}
}
