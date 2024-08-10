import { immerable } from 'immer';
import { Interval } from 'luxon';
import { nanoid } from 'nanoid/non-secure';

import type { HumanJson, HumanJsonable } from './json';
import { intervalFromISO, intervaltoISO } from './misc';

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
			interval: intervaltoISO(this.interval),
			weeksNumbering: this.weeksNumbering as boolean | undefined,
			groupsRotation: this.groupsRotation as boolean | undefined,
		}
	}
	
	static fromHumanJson(json: HumanJson<Interruption>) {
		return new Interruption(
			json.name,
			intervalFromISO(json.interval, message => `L'intervalle de début et fin de la période d'interruption « ${json.name} » est invalide (${message}).`),
			json.weeksNumbering,
			json.groupsRotation,
		);
	}
}
