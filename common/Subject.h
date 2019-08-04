#pragma once

#include <QObject>
#include <QColor>
#include <QString>

class Subject : public QObject
{
	Q_OBJECT
	public:
		Subject(QString const &name, QString const &shortName, int frequency, QColor color = Qt::lightGray);
		QString getName() const;
		QString getShortName() const;
		int getFrequency() const;
		QColor getColor() const;
		void setName(QString const &value);
		void setShortName(QString const &value);
		void setFrequency(int value);
		void setColor(QColor const &value);

	signals:
		void changed();

	protected:
		QString name;
		QString shortName;
		int frequency;
		QColor color;
};
