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

unsigned int divideCeil(unsigned int a, unsigned int b)
{
	return a/b + (a % b != 0);
}

void preventSleepMode(bool shouldPreventSleepMode)
{
	if (shouldPreventSleepMode) {
		#ifdef Q_OS_WIN
			SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
		#endif
	}
	else {
		#ifdef Q_OS_WIN
			SetThreadExecutionState(ES_CONTINUOUS);
		#endif
	}
}
