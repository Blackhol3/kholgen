import { immerable } from 'immer';

export class Week {
	[immerable] = true;
	
	constructor(
		readonly id: number,
	) {}
}
