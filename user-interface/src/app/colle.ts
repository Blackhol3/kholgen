import { immerable } from 'immer';

import { type Calendar } from './calendar';
import { Timeslot } from './timeslot';

export class Colle {
	[immerable] = true;
	
	constructor(
		readonly teacherId: string,
		readonly timeslot: Timeslot,
		readonly trioId: number,
		readonly weekId: number,
	) {}

	getStartDate(calendar: Calendar) {
		const week = calendar.getWeeks().find(week => week.id === this.weekId)!;
		return week.start.plus({
			days: this.timeslot.day,
			hours: this.timeslot.hour,
		});
	}

	isDuringWorkingDay(calendar: Calendar) {
		return calendar.isWorkingDay(this.getStartDate(calendar));
	}
}
