#pragma once

#include <QString>
#include <functional>

class QJsonObject;

class Subject
{
	public:
		Subject(QString const &id, QString const &name, QString const &shortName, int frequency);
		explicit Subject(QJsonObject const &json);

		QString const &getId() const;
		QString const &getName() const;
		QString const &getShortName() const;
		int getFrequency() const;

		bool operator==(Subject const &) const = default;

	protected:
		QString id;
		QString name;
		QString shortName;
		int frequency;
};

namespace std {
	template <>
	struct hash<Subject>
	{
		size_t operator()(const Subject &subject) const;
	};
}

