#include "OptionsVariations.h"

#include <QDebug>
#include "Subject.h"
#include "Subjects.h"

OptionsVariations::OptionsVariations(Options const* const options, Subjects const* const subjects, int numberOfGroups): options(options), isExhausted(false)
{
	numberOfVariations.clear();
	numberOfVariations[Option::NoConsecutiveColles][0] = 2;
	numberOfVariations[Option::OnlyOneCollePerDay][0] = 2;

	for (int idSubject = 0; idSubject < subjects->size(); ++idSubject) {
		auto subject = subjects->at(idSubject);
		numberOfVariations[Option::SameTeacherOnlyOnceInCycle][idSubject] = numberOfGroups / subject->getFrequency() + (numberOfGroups % subject->getFrequency() != 0);
	}

	numberOfVariations[Option::SameTeacherAndTimeslotOnlyOnceInCycle] = numberOfVariations[Option::SameTeacherOnlyOnceInCycle];

	variations = numberOfVariations;
	for (auto &variation: variations) {
		for (auto &subVariation: variation) {
			subVariation = 0;
		}
	}
}

bool OptionsVariations::exhausted() const
{
	return isExhausted;
}

/**
 * @brief Return the identifiant (as a non-negative integer) of the variation of the option at the current index.
 * For instance, with binary option, 0 means that the constraint should be enforced, and 1 that it should be relaxed.
 * With tertiary option, 0 means that the constraint should be enforced, 1 partially enforced and 2 relaxed.
 * OptionsVariations::Enforce is a constant defined to 0.
 */
int OptionsVariations::get(Option option, int subOption) const
{
	return variations[option][subOption];
}

bool OptionsVariations::shouldEnforce(Option option, int subOption) const
{
	return get(option, subOption) == 0;
}

void OptionsVariations::increment()
{
	for (int idOption = options->size() - 1; idOption >= 0; --idOption)
	{
		auto option = options->at(idOption);

		QVector<int> subOptions(variations[option].size());
		std::iota(subOptions.begin(), subOptions.end(), 0);
		std::stable_sort(subOptions.begin(), subOptions.end(), [&](int a, int b) { return variations[option][a] < variations[option][b]; });

		for (auto const &subOption: subOptions)
		{
			if (variations[option][subOption] < numberOfVariations[option][subOption] - 1)
			{
				++variations[option][subOption];
				qDebug().noquote() << QString("%1 | %2 => %3").arg(Options::optionNames[option], QString::number(subOption), QString::number(variations[option][subOption]));
				return;
			}
		}
	}

	isExhausted = true;
}