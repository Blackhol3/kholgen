import { immerable } from 'immer';

import { Colle } from './colle';
import { Objective } from './objective';
import { Subject } from './subject';
import { Teacher } from './teacher';
import { Timeslot } from './timeslot';
import { Trio } from './trio';
import { Week } from './week';

const defaultObjectives = [
	new Objective(
		'No consecutive colles',
		`Un trinôme ne doit pas avoir deux colles consécutives`,
		value => `${value} colle(s) consécutive(s)`,
	),
	new Objective(
		'Only one colle per day',
		`Un trinôme ne doit pas avoir deux colles le même jour`,
		value => `${value} colle(s) le même jour`,
	),
	new Objective(
		'Same slot only once in cycle',
		`Au cours d'un cycle, un trinôme n'utilise jamais deux fois le même créneau`,
		value => `${value} répétition(s)`,
	),
	new Objective(
		'Even distribution between teachers',
		`Les colles avec un même enseignant doivent être bien répartis dans le temps`,
		value => `Score de ${value}`,
	),
	new Objective(
		'Minimal number of slots',
		`Le nombre de créneaux de colles doit être minimal`,
		value => `${value} créneaux`,
	),
] as const;

export class State {
	[immerable] = true;
	
	constructor(
		readonly colles: readonly Colle[] = [],
		readonly subjects: readonly Subject[] = [],
		readonly teachers: readonly Teacher[] = [],
		readonly trios: readonly Trio[] = [],
		readonly weeks: readonly Week[] = [],
		readonly objectives: readonly Objective[] = defaultObjectives.slice(),
	) {}
	
	findId(property: 'subjects', id: string) : Subject;
	findId(property: 'teachers', id: string) : Teacher;
	findId(property: 'trios', id: number) : Trio;
	findId(property: 'weeks', id: number) : Week;
	findId(property: keyof State, id: any) {
		return (this[property] as any[]).find(element => element.id === id);
	}
	
	toHumanObject() {
		return {
			subjects: this.subjects,
			teachers: this.teachers.map(teacher => ({
				name: teacher.name,
				subject: this.findId('subjects', teacher.subjectId).name,
				availableTimeslots: teacher.availableTimeslots.map(timeslot => timeslot.toString()),
			})),
			objectives: this.objectives.map(objective => objective.name),
			numberOfTrios: this.trios.length,
		};
	}
	
	toSolverObject() {
		return {
			subjects: this.subjects,
			teachers: this.teachers,
			numberOfTrios: this.trios.length,
			numberOfWeeks: this.weeks.length,
			objectives: this.objectives.map(objective => objective.name),
		};
	}
	
	/** @todo Throw better error messages **/
	static fromJsonObject(jsonObject: any): State {
		let subjects = []
		for (let subject of jsonObject.subjects) {
			subjects.push(new Subject(
				subject.name, subject.shortName, subject.frequency, subject.color
			));
		}
		
		let teachers = [];
		for (let teacher of jsonObject.teachers) {
			teachers.push(new Teacher(
				teacher.name,
				subjects.find(subject => subject.name === teacher.subject)!.id,
				teacher.availableTimeslots.map((timeslot: string) => Timeslot.fromString(timeslot)),
			));
		}
		
		let trios = [];
		for (let i = 0; i < jsonObject.numberOfTrios; ++i) {
			trios.push(new Trio(i));
		}
		
		let weeks = [];
		for (let i = 0; i < 20; ++i) {
			weeks.push(new Week(i));
		}
		
		let objectives = [];
		for (let objective of jsonObject.objectives) {
			objectives.push(defaultObjectives.find(defaultObjective => defaultObjective.name === objective)!);
		}
		
		return new State([], subjects, teachers, trios, weeks, objectives);
	}
}
