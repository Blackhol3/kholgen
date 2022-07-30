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
	) {
		this.id = nanoid();
	}
	
	toHumanJsonObject(state: State) {
		return {
			name: this.name,
			subject: state.findId('subjects', this.subjectId).name,
			availableTimeslots: this.availableTimeslots.map(timeslot => timeslot.toString()),
		};
	}
	
	static fromJsonObject(json: ReturnType<Teacher['toHumanJsonObject']>, subjects: Subject[]) {
		return new Teacher(
			json.name,
			subjects.find(subject => subject.name === json.subject)!.id,
			json.availableTimeslots.map(timeslot => Timeslot.fromString(timeslot)),
		);
	}
}
