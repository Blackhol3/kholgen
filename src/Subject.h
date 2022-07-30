#pragma once

#include <QColor>
#include <QString>
#include <functional>

class QJsonObject;

class Subject
{
	public:
		Subject(QString const &id, QString const &name, QString const &shortName, int frequency, QColor color = Qt::lightGray);
		explicit Subject(QJsonObject const &json);

		QString const &getId() const;
		QString const &getName() const;
		QString const &getShortName() const;
		int getFrequency() const;
		QColor const &getColor() const;

		bool operator==(Subject const &) const = default;

	protected:
		QString id;
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

