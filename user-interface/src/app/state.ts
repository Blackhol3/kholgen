import { immerable, type Draft } from 'immer';

import { Colle } from './colle';
import { Group } from './group';
import { Objective } from './objective';
import { Subject } from './subject';
import { Teacher } from './teacher';
import { firstHour, lastHour } from './timeslot';
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
		readonly lunchTimeRange: readonly [number, number] = [firstHour, lastHour + 1],
	) {}
	
	findId<S extends this | Draft<this>, P extends 'groups' | 'subjects' | 'teachers' | 'trios' | 'weeks'>(this: S, property: P, id: S[P][number]['id']): S[P][number] | undefined {
		for (const element of this[property]) {
			if (element.id === id) {
				return element;
			}
		}

		return undefined;
	}

	toHumanJsonObject() {
		return {
			groups: this.groups.map(group => group.toHumanJsonObject(this)),
			subjects: this.subjects.map(subject => subject.toHumanJsonObject()),
			teachers: this.teachers.map(teacher => teacher.toHumanJsonObject(this)),
			objectives: this.objectives.map(objective => objective.toHumanJsonObject()),
		};
	}
	
	toSolverJsonObject() {
		return {
			groups: this.groups,
			subjects: this.subjects,
			teachers: this.teachers,
			trios: this.trios,
			numberOfWeeks: this.weeks.length,
			objectives: this.objectives.map(objective => objective.name),
			lunchTimeRange: this.lunchTimeRange,
		};
	}
	
	/** @todo Throw better error messages **/
	static fromJsonObject(jsonObject: ReturnType<State['toHumanJsonObject']>) {
		const groups = jsonObject.groups.map(group => Group.fromJsonObject(group));
		for (let idGroup = 0; idGroup < groups.length; ++idGroup) {
			groups[idGroup] = groups[idGroup].setNextGroupFromJsonObject(jsonObject.groups[idGroup], groups);
		}
		
		const subjects = jsonObject.subjects.map(subject => Subject.fromJsonObject(subject));
		const teachers = jsonObject.teachers.map(teacher => Teacher.fromJsonObject(teacher, subjects)!);
		
		const weeks = [];
		for (let i = 0; i < 20; ++i) {
			weeks.push(new Week(i));
		}
		
		const objectives = [];
		for (const objective of jsonObject.objectives) {
			objectives.push(defaultObjectives.find(defaultObjective => defaultObjective.name === objective)!);
		}
		
		const lunchTimeRange = [11, 14] as const;
		
		return new State([], groups, subjects, teachers, [], weeks, objectives, lunchTimeRange);
	}
}
