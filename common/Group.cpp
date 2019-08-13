#include "Group.h"

Group::Group(QSet<const Subject *> subjects): QObject(), subjects(subjects)
{

}

bool Group::hasSubject(const Subject *const subject) const
{
	return subjects.contains(subject);
}

void Group::addSubject(const Subject *const subject)
{
	subjects.insert(subject);
}

void Group::removeSubject(const Subject *const subject)
{
	subjects.remove(subject);
}

