import { immerable } from 'immer';
import { nanoid } from 'nanoid/non-secure';

import type { HumanJson, HumanJsonable } from './json';
import { State } from './state';
import { Subject } from './subject';
import { Timeslot } from './timeslot';

export class Teacher implements HumanJsonable {
	[immerable] = true;
	readonly id: string;
	
	constructor(
		readonly name: string,
		readonly subjectId: string,
		readonly availableTimeslots: readonly Timeslot[],
		readonly weeklyAvailabilityFrequency: number = 1,
	) {
		this.id = nanoid();
	}
	
	toHumanJson(state: State) {
		return {
			name: this.name,
			subject: state.findId('subjects', this.subjectId)!.name,
			availableTimeslots: this.availableTimeslots.map(timeslot => timeslot.toString()),
			weeklyAvailabilityFrequency: this.weeklyAvailabilityFrequency > 1 ? this.weeklyAvailabilityFrequency : undefined,
		};
	}
	
	static fromHumanJson(json: HumanJson<Teacher>, subjects: readonly Subject[]) {
		const subject = subjects.find(subject => subject.name === json.subject);
		if (subject === undefined) {
			throw new SyntaxError(`La matière « ${json.subject} », enseignée par « ${json.name} », n'a pas été définie.`);
		}

		return new Teacher(
			json.name,
			subject.id,
			json.availableTimeslots.map(timeslot => Timeslot.fromString(timeslot)).sort((a, b) => a.compare(b)),
			json.weeklyAvailabilityFrequency,
		);
	}
}
