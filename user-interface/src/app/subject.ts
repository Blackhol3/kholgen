import { immerable } from 'immer';
import { nanoid } from 'nanoid/non-secure';

export class Subject {
	[immerable] = true;
	readonly id: string;
	
	constructor(
		readonly name: string,
		readonly shortName: string,
		readonly frequency: number,
		readonly color: string,
	) {
		this.id = nanoid();
	}
}
