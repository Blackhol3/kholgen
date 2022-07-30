import { immerable } from 'immer';
import { nanoid } from 'nanoid/non-secure';
import { Timeslot } from './timeslot';

export class Group {
	[immerable] = true;
	readonly id: string;
	readonly duration: number | null = null;
	readonly nextGroupId: string | null = null;
	
	constructor(
		readonly name: string,
		readonly availableTimeslots: Timeslot[],
		readonly numberOfTrios: number,
	) {
		this.id = nanoid();
	}
	
	setNextGroup(duration: number, nextGroup: Group) {
		toMutable(this).duration = duration;
		toMutable(this).nextGroupId = nextGroup.id;
	}
}

function toMutable<Class>(object: Class): {-readonly [property in keyof Class]: Class[property]} {
	return object;
}
