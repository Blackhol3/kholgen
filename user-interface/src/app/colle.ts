import { immerable } from 'immer';
import { Timeslot } from './timeslot';

export class Colle {
	[immerable] = true;
	
	constructor(
		readonly teacherId: string,
		readonly timeslot: Timeslot,
		readonly trioId: number,
		readonly weekId: number,
	) {}
}
