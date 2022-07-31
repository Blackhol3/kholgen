import { immerable } from 'immer';
import { nanoid } from 'nanoid/non-secure';

import { toMutable } from './misc';
import { State } from './state';
import { Timeslot } from './timeslot';

export class Group {
	[immerable] = true;
	readonly id: string;
	readonly duration: number | null = null;
	readonly nextGroupId: string | null = null;
	
	constructor(
		readonly name: string,
		readonly availableTimeslots: Timeslot[],
		readonly numberOfTrios: number,
	) {
		this.id = nanoid();
	}
	
	setNextGroup(duration: number, nextGroup: Group) {
		toMutable(this).duration = duration;
		toMutable(this).nextGroupId = nextGroup.id;
	}
	
	setNextGroupFromJsonObject(json: ReturnType<Group['toHumanJsonObject']>, groups: Group[]) {
		const nextGroup = groups.find(group => group.name === json.nextGroup);
		if (json.duration !== undefined && nextGroup !== undefined) {
			this.setNextGroup(json.duration!, nextGroup);
		}
	}
	
	toHumanJsonObject(state: State) {
		return Object.assign(
			{
				name: this.name,
				availableTimeslots: this.availableTimeslots.map(timeslot => timeslot.toString()),
				numberOfTrios: this.numberOfTrios,
			},
			this.nextGroupId !== null ? {
				duration: this.duration,
				nextGroup: state.findId('groups', this.nextGroupId)!.name,
			} : {}
		);
	}
	
	static fromJsonObject(json: ReturnType<Group['toHumanJsonObject']>) {
		return new Group(
			json.name,
			json.availableTimeslots.map((timeslot: string) => Timeslot.fromString(timeslot)),
			json.numberOfTrios,
		);
	}
}
