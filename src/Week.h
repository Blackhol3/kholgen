#pragma once

#include <functional>

class QJsonObject;

class Week
{
	public:
		Week(int id, int number);
		explicit Week(QJsonObject const &json);

		int getId() const;
		int getNumber() const;

		bool operator==(Week const &) const = default;

	protected:
		int id;
		int number;
};

namespace std {
	template <>
	struct hash<Week>
	{
		size_t operator()(const Week &week) const;
	};
}
