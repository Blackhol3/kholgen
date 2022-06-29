#pragma once

#include <QJsonObject>
#include <functional>

class Trio
{
	public:
		Trio(int id);
		int getId() const;

		QJsonObject toJsonObject() const;
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
