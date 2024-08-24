#include "misc.h"

#include <QTextStream>

#ifdef Q_OS_WIN
	#include <cstdio>
	#include <Windows.h>
#endif

void initStdout()
{
	#ifdef Q_OS_WIN
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
