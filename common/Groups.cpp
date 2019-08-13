#include "Groups.h"

#include "Group.h"

void Groups::append(Group *const group)
{
	group->setParent(this);
	groups << group;
}

Group* Groups::at(int i) const
{
	return groups.at(i);
}

QVector<Group*>::iterator Groups::begin()
{
	return groups.begin();
}

QVector<Group const*>::const_iterator Groups::begin() const
{
	return groups.begin();
}

QVector<Group const*>::const_iterator Groups::cbegin() const
{
	return groups.cbegin();
}

QVector<Group*>::iterator Groups::end()
{
	return groups.end();
}

QVector<Group const*>::const_iterator Groups::end() const
{
	return groups.end();
}

QVector<Group const*>::const_iterator Groups::cend() const
{
	return groups.cend();
}

int Groups::indexOf(Group* const group) const
{
	return groups.indexOf(group);
}

int Groups::indexOf(Group const* const group) const
{
	return groups.indexOf(const_cast<Group* const>(group));
}

void Groups::remove(int i)
{
	groups.remove(i);
}

int Groups::size() const
{
	return groups.size();
}

const QVector<Group*> Groups::withSubject(const Subject *const subject) const
{
	QVector<Group*> groupsWithSubject;
	std::copy_if(groups.cbegin(), groups.cend(), std::back_inserter(groupsWithSubject), [=](auto const &group) { return group->hasSubject(subject); });
	return groupsWithSubject;
}

Groups &Groups::operator<<(Group *const group)
{
	append(group);
	return *this;
}

Group* Groups::operator[](int i) const
{
	return groups[i];
}

