#include "Subjects.h"

#include <QRandomGenerator>
#include "Subject.h"

void Subjects::append()
{
	QString name(tr("Matière %1"));
	int i = 1;

	while (std::any_of(subjects.cbegin(), subjects.cend(), [&](auto const &subject) { return subject->getName() == name.arg(i) || subject->getShortName() == name.arg(i); })) {
		++i;
	}

	append(new Subject(name.arg(i), name.arg(i), 1, QColor::fromHsv(QRandomGenerator::global()->bounded(0, 360), 192, 192)));
}

void Subjects::append(Subject *const subject)
{
	int i = subjects.size();
	connect(subject, &Subject::changed, this, [=]() { emit changed(i); });
	subject->setParent(this);

	subjects << subject;
	emit appended(i);
}

Subject* Subjects::at(int i) const
{
	return subjects.at(i);
}

QVector<Subject const*>::const_iterator Subjects::begin() const
{
	return subjects.begin();
}

QVector<Subject const*>::const_iterator Subjects::cbegin() const
{
	return subjects.cbegin();
}

QVector<Subject const*>::const_iterator Subjects::end() const
{
	return subjects.end();
}

QVector<Subject const*>::const_iterator Subjects::cend() const
{
	return subjects.cend();
}

int Subjects::indexOf(Subject* const subject) const
{
	return subjects.indexOf(subject);
}

int Subjects::indexOf(Subject const* const subject) const
{
	return subjects.indexOf(const_cast<Subject* const>(subject));
}

void Subjects::remove(int i)
{
	auto subject = subjects[i];
	subject->disconnect(this);

	subjects.remove(i);
	emit removed(i);
}

int Subjects::size() const
{
	return subjects.size();
}

Subjects &Subjects::operator<<(Subject *const subject)
{
	append(subject);
	return *this;
}

Subject* Subjects::operator[](int i) const
{
	return subjects[i];
}
