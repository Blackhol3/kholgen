#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "Timeslot.h"

using namespace testing;

TEST(TimeslotTest, isAdjacentTo)
{
	Timeslot timeslot1(Day::Monday, 12);
	Timeslot timeslot2(Day::Monday, 11);
	Timeslot timeslot3(Day::Monday, 13);

	Timeslot timeslot4(Day::Monday, 12);
	Timeslot timeslot5(Day::Monday, 14);
	Timeslot timeslot6(Day::Tuesday, 13);

	EXPECT_TRUE(timeslot1.isAdjacentTo(timeslot2));
	EXPECT_TRUE(timeslot1.isAdjacentTo(timeslot3));

	EXPECT_FALSE(timeslot1.isAdjacentTo(timeslot4));
	EXPECT_FALSE(timeslot1.isAdjacentTo(timeslot5));
	EXPECT_FALSE(timeslot1.isAdjacentTo(timeslot6));
}

TEST(TimeslotTest, getters)
{
	Timeslot timeslot(Day::Monday, 12);

	EXPECT_EQ(timeslot.getDay(), Day::Monday);
	EXPECT_EQ(timeslot.getHour(), 12);
}
