#include "Subject.h"

Subject::Subject(const QString &name, const QString &shortName, int frequency, QColor color): QObject(), name(name), shortName(shortName), frequency(frequency), color(color)
{

}

Subject::Subject(Subject const &subject, int frequency): QObject(), name(subject.name), shortName(subject.shortName), frequency(frequency), color(subject.color)
{

}

QString Subject::getName() const
{
	return name;
}

QString Subject::getShortName() const
{
	return shortName;
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

void Subject::setShortName(const QString &value)
{
	shortName = value;
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
