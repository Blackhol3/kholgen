import { immerable, type Draft } from 'immer';
import { DateTime, Interval } from 'luxon';

import { Interruption } from './interruption';
import type { HumanJson, HumanJsonable } from './json';
import { intervalFromISO } from './misc';
import { nbDaysInWeek } from './timeslot';
import { Week, type WorkingWeek } from './week';

import { type CalendarService } from './calendar.service';

export function getFirstValidDate() {
	return DateTime.now().plus({week: 1}).startOf('week');
}

class CalendarCache {
	weeks: readonly Week[] = [];
	schoolHolidays: readonly Interval[] = [];
	publicHolidays: readonly DateTime[] = [];
}

export class Calendar implements HumanJsonable {
	[immerable] = true;

	protected readonly cache = new CalendarCache();

	constructor(
		readonly academie: string | null = null,
		readonly interval = Interval.fromDateTimes(
			getFirstValidDate(),
			DateTime.now().plus({week: 10}).endOf('week').minus({days: 7 - nbDaysInWeek}),
		),
		readonly firstWeekNumber = 1,
		readonly interruptions: readonly Interruption[] = [],
	) {}
	
	async updateWeeksAndHolidays(calendarService: CalendarService) {
		this.cache.schoolHolidays = this.academie === null ? [] : await calendarService.getSchoolHolidays(this.academie);
		this.cache.publicHolidays = this.academie === null ? [] : await calendarService.getPublicHolidays(this.academie);
		this.updateWeeks();
	}

	updateWeeks() {
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

		this.cache.weeks = weeks;
	}

	getWeeks() {
		return this.cache.weeks;
	}

	getWorkingWeeks() {
		return this.cache.weeks.filter(week => week.isWorking()) as WorkingWeek[];
	}

	findInterruptionId<S extends this | Draft<this>>(this: S, id: S['interruptions'][number]['id']): S['interruptions'][number] | undefined {
		return this.interruptions.find(x => x.id === id);
	}

	isWorkingDay(date: DateTime, ignoredInterruptionId: string | null = null) {
		return (
			date.weekday <= nbDaysInWeek
			&& !this.interruptions.some(x => x.interval.contains(date) && x.id !== ignoredInterruptionId)
			&& !this.cache.schoolHolidays.some(x => x.contains(date))
			&& !this.cache.publicHolidays.some(x => x.hasSame(date, 'day'))
		);
	}

	toHumanJson() {
		return {
			academie: this.academie === null ? undefined : this.academie,
			interval: this.interval.toISODate(),
			firstWeekNumber: this.firstWeekNumber === 1 ? undefined : this.firstWeekNumber,
			interruptions: this.interruptions.length === 0 ? undefined : this.interruptions,
		}
	}

	protected isWorkingWeekInterval(weekInterval: Interval, ignoredInterruptionId: string | null = null) {
		return weekInterval
			.splitBy({day: 1})
			.map(day => day.start)
			.some(day => this.interval.contains(day) && this.isWorkingDay(day, ignoredInterruptionId))
		;
	}

	static async fromHumanJson(json: HumanJson<Calendar>, calendarService: CalendarService) {
		const academies = await calendarService.getAcademies();
		if (json.academie !== undefined && !academies.includes(json.academie)) {
			throw new SyntaxError(`L'académie « ${json.academie} » n'existe pas.`);
		}

		const interval = intervalFromISO(json.interval, message => `L'intervalle de début et fin des colles est invalide (${message}).`);
		const calendar = new Calendar(
			json.academie,
			Interval.fromDateTimes(interval.start, interval.end.endOf('day')),
			json.firstWeekNumber,
			json.interruptions?.map(jsonInterruption => Interruption.fromHumanJson(jsonInterruption)),
		);
		await calendar.updateWeeksAndHolidays(calendarService);

		return calendar;
	}
}

