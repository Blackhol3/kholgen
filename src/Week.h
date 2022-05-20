#pragma once

#include <functional>

class Week
{
	public:
		Week(int id);
		int getId() const;

		bool operator==(Week const &) const = default;

	protected:
		int id;
};

namespace std {
	template <>
	struct hash<Week>
	{
		size_t operator()(const Week &week) const;
	};
}
