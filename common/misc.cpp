#include "misc.h"

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

template QVector<QPair<int, int>> getAllPairs(QVector<int> const &vector);
template QVector<QPair<int, int>> getAllConsecutivePairs(QVector<int> const &vector);
