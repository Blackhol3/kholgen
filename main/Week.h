#pragma once

class Week
{
	public:
		Week(int id);
		int getId() const;

	protected:
		int id;
};

bool operator==(const Week &a, const Week &b);
bool operator!=(const Week &a, const Week &b);
unsigned int qHash(Week const &key, unsigned int seed);

