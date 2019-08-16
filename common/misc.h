#pragma once

#include <QPair>
#include <QVector>

QVector<QVector<int>> getIntegerGroups(int const start, int const end, int const size, int const step);

template<typename T>
QVector<QPair<T, T>> getAllPairs(QVector<T> const &vector);

template<typename T>
QVector<QPair<T, T>> getAllConsecutivePairs(QVector<T> const &vector);

template<typename T>
T divideCeil(T const &a, T const &b);
