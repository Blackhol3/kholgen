import { immerable } from 'immer';
import { nanoid } from 'nanoid/non-secure';

import type { HumanJson, HumanJsonable } from './json';

export class Subject implements HumanJsonable {
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
	
	toHumanJson() {
		return {
			name: this.name,
			shortName: this.shortName,
			frequency: this.frequency,
			color: this.color,
		};
	}
	
	static fromHumanJson(json: HumanJson<Subject>) {
		return new Subject(
			json.name,
			json.shortName,
			json.frequency,
			json.color,
		);
	}
}
