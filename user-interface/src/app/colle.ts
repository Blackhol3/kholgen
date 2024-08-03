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

	isDuringWorkingDay(state: State) {
		const week = state.calendar.weeks.find(week => week.id === this.weekId)!;
		const day = week.start.plus({days: this.timeslot.day});
		
		return state.calendar.isWorkingDay(day);
	}
}
