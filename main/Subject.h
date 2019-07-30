#pragma once

#include <QObject>
#include <QColor>
#include <QString>

class Subject : public QObject
{
	Q_OBJECT
	public:
		Subject(QString name, int frequency, QColor color = Qt::lightGray);
		QString getName() const;
		int getFrequency() const;
		QColor getColor() const;
		void setName(QString const &value);
		void setFrequency(int value);
		void setColor(QColor const &value);

	signals:
		void changed();

	protected:
		QString name;
		int frequency;
		QColor color;
};
