#include "misc.h"

#ifdef Q_OS_WIN
	#include <Windows.h>
#else
	#include <QThread>
#endif

QVector<QVector<int>> getIntegerGroups(int const start, int const end, int const size, int const step)
{
	QVector<QVector<int>> integerGroups;

	for (int position = start; position <= end; ++position)
	{
		QVector<int> integerGroup;
		for (int offset = 0; offset < size * step; offset += step)
		{
			int integer = position + offset;
			if (integer > end) {
				break;
			}

			integerGroup << integer;
		}

		integerGroups << integerGroup;
	}

	return integerGroups;
}

template<typename T>
QVector<QPair<T, T>> getAllPairs(QVector<T> const &vector)
{
	int const size = vector.size();
	QVector<QPair<T, T>> allPairs;

	for (int i = 0; i < size - 1; ++i)
	{
		for (int j = i + 1; j < size; ++j)
		{
			allPairs.append({vector[i], vector[j]});
		}
	}

	return allPairs;
}

template<typename T>
QVector<QPair<T, T>> getAllConsecutivePairs(QVector<T> const &vector)
{
	int const size = vector.size();
	QVector<QPair<T, T>> allConsecutivePairs;

	for (int i = 0; i < size - 1; ++i) {
		allConsecutivePairs.append({vector[i], vector[i + 1]});
	}

	return allConsecutivePairs;
}

template<typename T>
T divideCeil(T const &a, T const &b)
{
	return a/b + (a % b != 0);
}

template QVector<QPair<int, int>> getAllPairs(QVector<int> const &vector);
template QVector<QPair<int, int>> getAllConsecutivePairs(QVector<int> const &vector);
template int divideCeil(int const &a, int const &b);

/** @link https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation */
int getNumberOfPhysicalCores()
{
	#ifdef Q_OS_WIN
		typedef BOOL (WINAPI *LPFN_GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

		auto glpi = reinterpret_cast<LPFN_GLPI>(GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation"));
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = nullptr;
		DWORD returnLength = 0;
		DWORD byteOffset = 0;
		int processorCoreCount = 0;

		Q_ASSERT(glpi != nullptr);

		forever
		{
			auto rc = glpi(buffer, &returnLength);
			if (rc == 0)
			{
				Q_ASSERT(GetLastError() == ERROR_INSUFFICIENT_BUFFER);

				if (buffer) {
					free(buffer);
				}

				buffer = static_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(malloc(returnLength));
				Q_ASSERT(buffer != nullptr);
			}
			else {
				break;
			}
		}

		ptr = buffer;
		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
		{
			if (ptr->Relationship == RelationProcessorCore) {
				++processorCoreCount;
			}

			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			ptr++;
		}

		return processorCoreCount;
	#else
		return QThread::idealThreadCount();
	#endif
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
