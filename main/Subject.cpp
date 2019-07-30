#include "Subject.h"

Subject::Subject(QString name, int frequency, QColor color): QObject(), name(name), frequency(frequency), color(color)
{

}

QString Subject::getName() const
{
	return name;
}

int Subject::getFrequency() const
{
	return frequency;
}

QColor Subject::getColor() const
{
	return color;
}


void Subject::setName(const QString &value)
{
    name = value;
	emit changed();
}

void Subject::setFrequency(int value)
{
	frequency = value;
	emit changed();
}

void Subject::setColor(const QColor &value)
{
	color = value;
	emit changed();
}
