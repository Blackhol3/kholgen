import { immerable } from 'immer';
import { Interval } from 'luxon';
import { nanoid } from 'nanoid/non-secure';

export class Interruption {
	[immerable] = true;
	readonly id: string;
	
	constructor(
		readonly name: string,
		readonly interval: Interval,
		readonly weeksNumbering: boolean = false,
		readonly groupsRotation: boolean = false,
	) {
		this.id = nanoid();
	}

	toHumanJsonObject() {
		return {
			name: this.name,
			interval: this.interval.toISODate(),
			weeksNumbering: this.weeksNumbering,
			groupsRotation: this.groupsRotation,
		}
	}
	
	static fromJsonObject(json: ReturnType<Interruption['toHumanJsonObject']>) {
		return new Interruption(
			json.name,
			Interval.fromISO(json.interval),
			json.weeksNumbering,
			json.groupsRotation,
		);
	}
}
