import { immerable } from 'immer';
import { Interval } from 'luxon';
import { nanoid } from 'nanoid/non-secure';

import type { HumanJson, HumanJsonable } from './json';

export class Interruption implements HumanJsonable {
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

	toHumanJson() {
		return {
			name: this.name,
			interval: this.interval.toISODate(),
			weeksNumbering: this.weeksNumbering,
			groupsRotation: this.groupsRotation,
		}
	}
	
	static fromHumanJson(json: HumanJson<Interruption>) {
		const interval = Interval.fromISO(json.interval);
		return new Interruption(
			json.name,
			Interval.fromDateTimes(interval.start, interval.end.endOf('day')),
			json.weeksNumbering,
			json.groupsRotation,
		);
	}
}
