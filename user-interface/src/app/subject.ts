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
	
	toHumanJsonObject() {
		return {
			name: this.name,
			shortName: this.shortName,
			frequency: this.frequency,
			color: this.color,
		};
	}
	
	static fromJsonObject(json: ReturnType<Subject['toHumanJsonObject']>) {
		return new Subject(
			json.name,
			json.shortName,
			json.frequency,
			json.color,
		);
	}
}
