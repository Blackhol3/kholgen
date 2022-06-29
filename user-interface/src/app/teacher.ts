import { Subject } from './subject';
import { Timeslot } from './timeslot';

export class Teacher {
	constructor(
		readonly name: string,
		readonly subject: Subject,
		readonly availableTimeslots: Timeslot[],
	) {}
}
