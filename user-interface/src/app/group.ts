import { immerable, produce } from 'immer';
import { nanoid } from 'nanoid/non-secure';

import { State } from './state';
import { Timeslot } from './timeslot';

export class Group {
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
	
	setNextGroupFromJsonObject(json: ReturnType<Group['toHumanJsonObject']>, groups: Group[]): Group {
		const nextGroup = groups.find(group => group.name === json.nextGroup);
		if (json.duration !== undefined && nextGroup !== undefined) {
			return this.setNextGroup(json.duration!, nextGroup);
		}
		
		return this;
	}
	
	toHumanJsonObject(state: State) {
		return Object.assign(
			{
				name: this.name,
				availableTimeslots: this.availableTimeslots.map(timeslot => timeslot.toString()),
				trioIds: [...this.trioIds],
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
			new Set(json.trioIds),
		);
	}
}
