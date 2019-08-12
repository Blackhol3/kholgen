#pragma once

#include <QString>

class Option
{
	public:
		Option();
		int getId() const;
		QString getName() const;
		bool isDefinedBySubject() const;

		static Option const
			NoConsecutiveColles,
			NoSameTeacherConsecutively,
			SameTeacherOnlyOnceInCycle,
			SameTeacherAndTimeslotOnlyOnceInCycle,
			OnlyOneCollePerDay
		;

	protected:
		Option(int const id, QString const &name, bool const definedBySubject);

		int id;
		QString name;
		bool definedBySubject;
};

bool operator==(Option const &a, Option const &b);
bool operator!=(Option const &a, Option const &b);
unsigned int qHash(Option const &key, unsigned int seed);

