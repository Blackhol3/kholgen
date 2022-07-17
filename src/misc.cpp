#include "misc.h"

#ifdef _WIN32
	#include <cstdio>
	#include <Windows.h>
#endif

#include <QTextStream>

void initStdout()
{
	#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
		setvbuf(stdout, nullptr, _IOFBF, 1000);
	#endif
}

QTextStream& qStdout()
{
	static QTextStream ts(stdout);
	return ts;
}
