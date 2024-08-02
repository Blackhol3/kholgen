import { immerable } from 'immer';
import { type DateTime } from 'luxon';

import { type SolverJsonable } from './json';

export type WorkingWeek = Week & {number: number};

export class Week implements SolverJsonable {
	[immerable] = true;
	
	constructor(
		readonly id: number,
		readonly number: number | null,
		readonly start: DateTime,
	) {}

	isWorking(): this is WorkingWeek {
		return this.number !== null;
	}

	toSolverJson() {
		return {
			id: this.id,
			number: this.number,
		};
	}
}
