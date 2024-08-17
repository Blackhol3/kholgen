#include "Week.h"

#include <QJsonObject>

Week::Week(int id, int number):
	id(id),
	number(number)
{

}

Week::Week(const QJsonObject& json):
	id(json["id"].toInt()),
	number(json["number"].toInt())
{
}

int Week::getId() const
{
	return id;
}


int Week::getNumber() const
{
	return number;
}

size_t std::hash<Week>::operator()(const Week& week) const
{
	return std::hash<int>()(week.getId());
}
