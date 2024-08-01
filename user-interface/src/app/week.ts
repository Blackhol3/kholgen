import { immerable } from 'immer';
import { type DateTime } from 'luxon';

export type WorkingWeek = Week & {number: number};

export class Week {
	[immerable] = true;
	
	constructor(
		readonly id: number,
		readonly number: number | null,
		readonly start: DateTime,
	) {}

	isWorking(): this is WorkingWeek {
		return this.number !== null;
	}

	toSolverJsonObject() {
		return {
			id: this.id,
			number: this.number,
		};
	}
}
