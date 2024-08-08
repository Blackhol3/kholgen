import { immerable, produce } from 'immer';
import { nanoid } from 'nanoid/non-secure';

import type { HumanJson, HumanJsonable } from './json';
import { State } from './state';
import { Timeslot } from './timeslot';

export class Group implements HumanJsonable {
	[immerable] = true;
	readonly id: string;
	readonly duration: number | null = null;
	readonly nextGroupId: string | null = null;
	
	constructor(
		readonly name: string,
		readonly availableTimeslots: readonly Timeslot[],
		readonly trioIds: ReadonlySet<number>,
	) {
		this.id = nanoid();
	}
	
	setNextGroup(duration: number, nextGroup: Group): Group {
		return produce(this, group => {
			group.duration = duration;
			group.nextGroupId = nextGroup.id;
		});
	}
	
	setNextGroupFromHumanJson(json: HumanJson<Group>, groups: readonly Group[]): Group {
		const nextGroup = groups.find(group => group.name === json.nextGroup);
		if (json.duration !== undefined && nextGroup !== undefined) {
			return this.setNextGroup(json.duration, nextGroup);
		}
		
		return this;
	}
	
	toHumanJson(state: State) {
		return {
			name: this.name,
			availableTimeslots: this.availableTimeslots.map(timeslot => timeslot.toString()),
			trios: [...this.trioIds],
			duration: this.duration ?? undefined,
			nextGroup: this.nextGroupId !== null ? state.findId('groups', this.nextGroupId)!.name : undefined,
		};
	}
	
	static fromHumanJson(json: HumanJson<Group>) {
		return new Group(
			json.name,
			json.availableTimeslots.map((timeslot: string) => Timeslot.fromString(timeslot)),
			new Set(json.trios),
		);
	}
}
