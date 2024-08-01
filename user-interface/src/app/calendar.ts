import { immerable, type Draft } from 'immer';
import { DateTime, Interval } from 'luxon';

import { type Interruption } from './interruption';
import { nbDaysInWeek } from './timeslot';
import { Week, type WorkingWeek } from './week';

export function getFirstValidDate() {
	return DateTime.now().plus({week: 1}).startOf('week');
}

export class Calendar {
	[immerable] = true;

	readonly weeks: Week[];

	constructor(
		readonly academie: string | null = null,
		readonly interval = Interval.fromDateTimes(
			getFirstValidDate(),
			DateTime.now().plus({week: 10}).endOf('week').minus({days: 7 - nbDaysInWeek}),
		),
		readonly firstWeekNumber = 1,
		readonly interruptions: readonly Interruption[] = [],
		readonly schoolHolidays: readonly Interval[] = [],
		readonly publicHolidays: readonly DateTime[] = [],
	) {
		this.weeks = this.createWeeks();
	}

	createWeeks() {
		const weeks: Week[] = [];
		let currentWeekId = 0;
		let currentWeekNumber = this.firstWeekNumber;

		const weekIntervals = Interval
			.fromDateTimes(this.interval.start.startOf('week'), this.interval.end.endOf('week'))
			.splitBy({week: 1})
		;

		for (const weekInterval of weekIntervals) {
			if (this.isWorkingWeekInterval(weekInterval)) {
				weeks.push(new Week(currentWeekId, currentWeekNumber, weekInterval.start));
				++currentWeekId;
				++currentWeekNumber;
				continue;
			}

			for (const interruption of this.interruptions) {
				if (this.isWorkingWeekInterval(weekInterval, interruption.id)) {
					if (interruption.groupsRotation) {
						weeks.push(new Week(currentWeekId, null, weekInterval.start));
						++currentWeekId;
					}
					
					if (interruption.weeksNumbering) {
						++currentWeekNumber;
					}
					
					continue;
				}
			}
		}

		return weeks;
	}

	getWorkingWeeks() {
		return this.weeks.filter(week => week.isWorking()) as WorkingWeek[];
	}

	findInterruptionId<S extends this | Draft<this>>(this: S, id: S['interruptions'][number]['id']): S['interruptions'][number] | undefined {
		return this.interruptions.find(x => x.id === id);
	}

	isWorkingDay(date: DateTime, ignoredInterruptionId: string | null = null) {
		return (
			date.weekday <= nbDaysInWeek
			&& !this.interruptions.some(x => x.interval.contains(date) && x.id !== ignoredInterruptionId)
			&& !this.schoolHolidays.some(x => x.contains(date))
			&& !this.publicHolidays.some(x => x.hasSame(date, 'day'))
		);
	}

	protected isWorkingWeekInterval(weekInterval: Interval, ignoredInterruptionId: string | null = null) {
		return weekInterval
			.splitBy({day: 1})
			.map(day => day.start)
			.some(day => this.interval.contains(day) && this.isWorkingDay(day, ignoredInterruptionId))
		;
	}
}

