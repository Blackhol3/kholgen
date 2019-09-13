#include "Trios.h"

#include "Trio.h"

void Trios::append(Trio *const trio)
{
	insert(trios.size(), trio);
}

Trio* Trios::at(int i) const
{
	return trios.at(i);
}

QVector<Trio*>::iterator Trios::begin()
{
	return trios.begin();
}

QVector<Trio const*>::const_iterator Trios::begin() const
{
	return trios.begin();
}

QVector<Trio const*>::const_iterator Trios::cbegin() const
{
	return trios.cbegin();
}

QVector<Trio*>::iterator Trios::end()
{
	return trios.end();
}

QVector<Trio const*>::const_iterator Trios::end() const
{
	return trios.end();
}

QVector<Trio const*>::const_iterator Trios::cend() const
{
	return trios.cend();
}

int Trios::indexOf(Trio* const Trio) const
{
	return trios.indexOf(Trio);
}

int Trios::indexOf(Trio const* const trio) const
{
	return trios.indexOf(const_cast<Trio* const>(trio));
}

void Trios::insert(int i, Trio *const trio)
{
	connect(trio, &Trio::changed, this, [=]() { emit changed(trios.indexOf(trio)); });
	trio->setParent(this);

	trios.insert(i, trio);
	emit inserted(i);
}

void Trios::remove(int i)
{
	auto Trio = trios[i];
	Trio->disconnect(this);

	trios.remove(i);
	emit removed(i);
}

int Trios::size() const
{
	return trios.size();
}

const QVector<Trio*> Trios::withSubject(const Subject *const subject) const
{
	QVector<Trio*> triosWithSubject;
	std::copy_if(trios.cbegin(), trios.cend(), std::back_inserter(triosWithSubject), [=](auto const &trio) { return trio->hasSubject(subject); });
	return triosWithSubject;
}

Trios &Trios::operator<<(Trio *const Trio)
{
	append(Trio);
	return *this;
}

Trio* Trios::operator[](int i) const
{
	return trios[i];
}

