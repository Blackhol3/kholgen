#pragma once

#include <QColor>
#include <QString>
#include <functional>

class QJsonObject;

class Subject
{
	public:
		Subject(QString const &name, QString const &shortName, int frequency, QColor color = Qt::lightGray);
		QString const &getName() const;
		QString const &getShortName() const;
		int getFrequency() const;
		QColor const &getColor() const;

		QJsonObject toJsonObject() const;
		bool operator==(Subject const &) const = default;

	protected:
		QString name;
		QString shortName;
		int frequency;
		QColor color;
};

namespace std {
	template <>
	struct hash<Subject>
	{
		size_t operator()(const Subject &subject) const;
	};
}

