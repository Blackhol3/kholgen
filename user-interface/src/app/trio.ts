import { immerable } from 'immer';

export class Trio {
	[immerable] = true;
	
	constructor(
		readonly id: number,
		readonly initialGroupIds: readonly string[],
	) {}
}
