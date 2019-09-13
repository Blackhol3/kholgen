#include "Trio.h"

Trio::Trio(QSet<const Subject *> subjects): QObject(), subjects(subjects)
{

}

bool Trio::hasSubject(const Subject *const subject) const
{
	return subjects.contains(subject);
}

void Trio::addSubject(const Subject *const subject)
{
	subjects.insert(subject);
	changed();
}

void Trio::removeSubject(const Subject *const subject)
{
	subjects.remove(subject);
	changed();
}

void Trio::toggleSubject(const Subject *const subject)
{
	hasSubject(subject) ? removeSubject(subject) : addSubject(subject);
}

