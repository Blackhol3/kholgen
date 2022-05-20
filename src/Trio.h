#pragma once

#include <functional>

class Trio
{
	public:
		Trio(int id);
		int getId() const;

		bool operator==(Trio const &) const = default;

	protected:
		int id;
};

namespace std {
	template <>
	struct hash<Trio>
	{
		size_t operator()(const Trio &trio) const;
	};
}
