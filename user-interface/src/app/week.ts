import { immerable } from 'immer';
import { type DateTime } from 'luxon';

export class Week {
	[immerable] = true;
	
	constructor(
		readonly id: number,
		readonly start: DateTime,
	) {}
}
