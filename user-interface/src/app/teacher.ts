import { immerable } from 'immer';
import { nanoid } from 'nanoid/non-secure';
import { Timeslot } from './timeslot';

export class Teacher {
	[immerable] = true;
	readonly id: string;
	
	constructor(
		readonly name: string,
		readonly subjectId: string,
		readonly availableTimeslots: readonly Timeslot[],
	) {
		this.id = nanoid();
	}
}
