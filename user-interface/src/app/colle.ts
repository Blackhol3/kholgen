import { immerable } from 'immer';

import { Timeslot } from './timeslot';
import { type State } from './state';

export class Colle {
	[immerable] = true;
	
	constructor(
		readonly teacherId: string,
		readonly timeslot: Timeslot,
		readonly trioId: number,
		readonly weekId: number,
	) {}

	getStartDate(state: State) {
		const week = state.calendar.getWeeks().find(week => week.id === this.weekId)!;
		return week.start.plus({
			days: this.timeslot.day,
			hours: this.timeslot.hour,
		});
	}

	isDuringWorkingDay(state: State) {
		return state.calendar.isWorkingDay(this.getStartDate(state));
	}
}
