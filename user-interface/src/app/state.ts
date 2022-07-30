import { immerable } from 'immer';

import { Colle } from './colle';
import { Group } from './group';
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
		readonly groups: readonly Group[] = [],
		readonly subjects: readonly Subject[] = [],
		readonly teachers: readonly Teacher[] = [],
		readonly trios: readonly Trio[] = [],
		readonly weeks: readonly Week[] = [],
		readonly objectives: readonly Objective[] = defaultObjectives.slice(),
	) {}
	
	findId(property: 'groups', id: string) : Group;
	findId(property: 'subjects', id: string) : Subject;
	findId(property: 'teachers', id: string) : Teacher;
	findId(property: 'trios', id: number) : Trio;
	findId(property: 'weeks', id: number) : Week;
	findId(property: keyof State, id: any) {
		return (this[property] as any[]).find(element => element.id === id);
	}
	
	toHumanObject() {
		return {
			groups: this.groups.map(group => group.nextGroupId === null ?
				{
					name: group.name,
					availableTimeslots: group.availableTimeslots.map(timeslot => timeslot.toString()),
					numberOfTrios: group.numberOfTrios,
				} :
				{
					name: group.name,
					availableTimeslots: group.availableTimeslots.map(timeslot => timeslot.toString()),
					numberOfTrios: group.numberOfTrios,
					duration: group.duration,
					nextGroup: this.findId('groups', group.nextGroupId).name,
				}
			),
			subjects: this.subjects,
			teachers: this.teachers.map(teacher => ({
				name: teacher.name,
				subject: this.findId('subjects', teacher.subjectId).name,
				availableTimeslots: teacher.availableTimeslots.map(timeslot => timeslot.toString()),
			})),
			objectives: this.objectives.map(objective => objective.name),
		};
	}
	
	toSolverObject() {
		return {
			groups: this.groups,
			subjects: this.subjects,
			teachers: this.teachers,
			trios: this.trios,
			numberOfWeeks: this.weeks.length,
			objectives: this.objectives.map(objective => objective.name),
		};
	}
	
	/** @todo Throw better error messages **/
	static fromJsonObject(jsonObject: any): State {
		let groups = [];
		for (let group of jsonObject.groups) {
			groups.push(new Group(
				group.name,
				group.availableTimeslots.map((timeslot: string) => Timeslot.fromString(timeslot)),
				group.numberOfTrios,
			));
		}
		for (let idGroup = 0; idGroup < groups.length; ++idGroup) {
			if (jsonObject.groups[idGroup].duration !== undefined && jsonObject.groups[idGroup].nextGroup !== undefined) {
				groups[idGroup].setNextGroup(
					jsonObject.groups[idGroup].duration,
					groups.find(group => group.name === jsonObject.groups[idGroup].nextGroup)!,
				);
			}
		}
		
		let subjects = [];
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
		
		let weeks = [];
		for (let i = 0; i < 20; ++i) {
			weeks.push(new Week(i));
		}
		
		let objectives = [];
		for (let objective of jsonObject.objectives) {
			objectives.push(defaultObjectives.find(defaultObjective => defaultObjective.name === objective)!);
		}
		
		return new State([], groups, subjects, teachers, [], weeks, objectives);
	}
}
