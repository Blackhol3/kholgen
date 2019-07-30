#pragma once


class Group
{
	public:
		Group(int id);
		int getId() const;

	protected:
		int id;
};

bool operator==(const Group &a, const Group &b);
bool operator!=(const Group &a, const Group &b);
unsigned int qHash(Group const &key, unsigned int seed);
