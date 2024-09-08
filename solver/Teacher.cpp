#include "Teacher.h"

#include <QJsonArray>
#include <QJsonObject>
#include "Subject.h"

Teacher::Teacher(QString const &id, QString const &name, Subject const &subject, std::set<Timeslot> const &availableTimeslots, int weeklyAvailabilityFrequency, std::optional<double> meanWeeklyVolume):
	id(id), name(name), subject(&subject), availableTimeslots(availableTimeslots), weeklyAvailabilityFrequency(weeklyAvailabilityFrequency), meanWeeklyVolume(meanWeeklyVolume)
{

}

Teacher::Teacher(QJsonObject const &json, std::vector<Subject> const &subjects): Teacher(
	json["id"].toString(),
	json["name"].toString(),
	*std::ranges::find_if(subjects, [&](auto const &subject) { return subject.getId() == json["subjectId"].toString(); }),
	Timeslot::getSet(json["availableTimeslots"].toArray()),
	json["weeklyAvailabilityFrequency"].toInt(),
	json["meanWeeklyVolume"].isNull() ? std::nullopt : std::optional(json["meanWeeklyVolume"].toDouble())
)
{
}

QString const &Teacher::getId() const
{
	return id;
}

QString const &Teacher::getName() const
{
	return name;
}

Subject const &Teacher::getSubject() const
{
	return *subject;
}

std::set<Timeslot> const &Teacher::getAvailableTimeslots() const
{
	return availableTimeslots;
}

int Teacher::getWeeklyAvailabilityFrequency() const
{
	return weeklyAvailabilityFrequency;
}

bool Teacher::hasMeanWeeklyVolume() const
{
	return meanWeeklyVolume.has_value();
}

TeacherTotalVolume Teacher::getTotalVolume(int nbWeeks) const
{
	double totalVolume = meanWeeklyVolume.value() * nbWeeks;

	int flooredValue = std::floor(totalVolume);
	int roundedValue = std::round(totalVolume);
	bool isExact = std::abs(totalVolume - roundedValue) < 1.0/(2 * nbWeeks);

	return {
		isExact,
		isExact ? roundedValue : flooredValue
	};
}

bool Teacher::isAvailableAtTimeslot(const Timeslot &timeslot) const
{
	return availableTimeslots.contains(timeslot);
}

size_t std::hash<Teacher>::operator()(const Teacher& teacher) const
{
	return std::hash<QString>()(teacher.getId());
}
