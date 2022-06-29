import { Teacher } from './teacher';
import { Timeslot } from './timeslot';
import { Trio } from './trio';
import { Week } from './week';

export class Colle {
	constructor(
		readonly teacher: Teacher,
		readonly timeslot: Timeslot,
		readonly trio: Trio,
		readonly week: Week,
	) {}
}
