import { immerable } from 'immer';
import { DateTime, Interval } from 'luxon';

import { nbDaysInWeek } from './timeslot';
import { Week } from './week';

export function getFirstValidDate() {
	return DateTime.now().plus({week: 1}).startOf('week');
}

export class Calendar {
	[immerable] = true;

	constructor(
		readonly academie = '',
		readonly start = getFirstValidDate(),
		readonly end = DateTime.now().plus({week: 10}).endOf('week').minus({days: 7 - nbDaysInWeek}),
		readonly firstWeekNumber = 1,
	) {}

	createWeeks(holidays: Interval[]) {
		return Interval
			.fromDateTimes(this.start.startOf('week'), this.end.endOf('week'))
			.splitBy({week: 1})
			.map(week => Interval.after(week.start!, {days: nbDaysInWeek}))
			.filter(week => week
				.splitBy({day: 1})
				.map(day => day.start!)
				.some(day => day >= this.start && day <= this.end && !holidays.some(interval => interval.contains(day)))
			)
			.map((week, index) => new Week(this.firstWeekNumber + index, week.start!))
		;
	}
}

