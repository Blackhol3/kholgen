#include "Option.h"

#include <QHash>
#include <QObject>

Option const Option::NoConsecutiveColles(1, QObject::tr("Un trinôme n'a jamais deux colles consécutives"), false);
Option const Option::NoSameTeacherConsecutively(2, QObject::tr("Un trinôme n'a jamais le même professeur deux fois d'affilée"), true);
Option const Option::SameTeacherOnlyOnceInCycle(3, QObject::tr("Au cours d'un cycle, un trinôme n'a jamais deux fois le même professeur"), true);
Option const Option::SameTeacherAndTimeslotOnlyOnceInCycle(4, QObject::tr("Au cours d'un cycle, un trinôme n'a jamais deux fois le même professeur, le même jour, à la même heure"), true);
Option const Option::OnlyOneCollePerDay(5, QObject::tr("Un trinôme n'a jamais deux colles le même jour"), false);

Option::Option(): id(0), name()
{

}

Option::Option(int const id, QString const &name, const bool definedBySubject): id(id), name(name), definedBySubject(definedBySubject)
{

}

int Option::getId() const
{
	return id;
}

QString Option::getName() const
{
	return name;
}

bool Option::isDefinedBySubject() const
{
	return definedBySubject;
}

bool operator==(const Option &a, const Option &b)
{
	return a.getId() == b.getId();
}

bool operator!=(const Option &a, const Option &b)
{
	return !(a == b);
}

unsigned int qHash(const Option &key, unsigned int seed)
{
	return qHash(key.getId(), seed);
}
