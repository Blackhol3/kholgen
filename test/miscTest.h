#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "misc.h"

using namespace testing;

TEST(miscTest, getIntegerGroups)
{
	QVector<QVector<int>> result, expected;

	result = getIntegerGroups(0, 5, 3, 1);
	expected = {{0, 1, 2}, {1, 2, 3}, {2, 3, 4}, {3, 4, 5}, {4, 5}, {5}};
	EXPECT_EQ(result, expected);

	result = getIntegerGroups(0, 9, 4, 2);
	expected = {{0, 2, 4, 6}, {1, 3, 5, 7}, {2, 4, 6, 8}, {3, 5, 7, 9}, {4, 6, 8}, {5, 7, 9}, {6, 8}, {7, 9}, {8}, {9}};
	EXPECT_EQ(result, expected);
}

TEST(miscTest, getAllPairs)
{
	QVector<QPair<int, int>> result, expected;

	result = getAllPairs(QVector<int>{0, 1, 2, 3, 4, 5});
	expected = {
		{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5},
		{1, 2}, {1, 3}, {1, 4}, {1, 5},
		{2, 3}, {2, 4}, {2, 5},
		{3, 4}, {3, 5},
		{4, 5},
	};
	EXPECT_EQ(result, expected);

	result = getAllPairs(QVector<int>{0, 1});
	expected = {{0, 1}};
	EXPECT_EQ(result, expected);

	result = getAllPairs(QVector<int>{0});
	expected = {};
	EXPECT_EQ(result, expected);

	result = getAllPairs(QVector<int>{});
	expected = {};
	EXPECT_EQ(result, expected);
}

TEST(miscTest, getAllConsecutivePairs)
{
	QVector<QPair<int, int>> result, expected;

	result = getAllConsecutivePairs(QVector<int>{0, 1, 2, 3, 4, 5});
	expected = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}};
	EXPECT_EQ(result, expected);

	result = getAllConsecutivePairs(QVector<int>{0, 1});
	expected = {{0, 1}};
	EXPECT_EQ(result, expected);

	result = getAllConsecutivePairs(QVector<int>{0});
	expected = {};
	EXPECT_EQ(result, expected);

	result = getAllConsecutivePairs(QVector<int>{});
	expected = {};
	EXPECT_EQ(result, expected);
}
