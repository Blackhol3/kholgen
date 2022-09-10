import { immerable } from 'immer';
import { nanoid } from 'nanoid/non-secure';

import { State } from './state';
import { Subject } from './subject';
import { Timeslot } from './timeslot';

export class Teacher {
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
	
	toHumanJsonObject(state: State) {
		return {
			name: this.name,
			subject: state.findId('subjects', this.subjectId)!.name,
			availableTimeslots: this.availableTimeslots.map(timeslot => timeslot.toString()),
			weeklyAvailabilityFrequency: this.weeklyAvailabilityFrequency > 1 ? this.weeklyAvailabilityFrequency : undefined,
		};
	}
	
	static fromJsonObject(json: ReturnType<Teacher['toHumanJsonObject']>, subjects: readonly Subject[]) {
		const subject = subjects.find(subject => subject.name === json.subject);
		return subject === undefined ? undefined : new Teacher(
			json.name,
			subject.id,
			json.availableTimeslots.map(timeslot => Timeslot.fromString(timeslot)),
			json.weeklyAvailabilityFrequency,
		);
	}
}
