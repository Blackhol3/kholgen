import { immerable, type Draft } from 'immer';

import { Calendar } from './calendar';
import { Colle } from './colle';
import { Group } from './group';
import { Objective } from './objective';
import { Subject } from './subject';
import { Teacher } from './teacher';
import { Trio } from './trio';

import type { HumanJson, HumanJsonable, SolverJsonable } from './json';

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

const defaultLunchTimeRange = [11, 14] as const;

export class State implements HumanJsonable, SolverJsonable {
	[immerable] = true;
	
	constructor(
		readonly colles: readonly Colle[] = [],
		readonly groups: readonly Group[] = [],
		readonly subjects: readonly Subject[] = [],
		readonly teachers: readonly Teacher[] = [],
		readonly trios: readonly Trio[] = [],
		readonly objectives: readonly Objective[] = defaultObjectives.slice(),
		readonly lunchTimeRange: readonly [number, number] = defaultLunchTimeRange,
		readonly forbiddenSubjectIdsCombination: ReadonlySet<string> = new Set(),
		readonly calendar = new Calendar(),
	) {}
	
	findId<S extends this | Draft<this>, P extends 'groups' | 'subjects' | 'teachers' | 'trios'>(this: S, property: P, id: S[P][number]['id']): S[P][number] | undefined {
		for (const element of this[property]) {
			if (element.id === id) {
				return element;
			}
		}

		return undefined;
	}

	removeSubject(this: Draft<this>, subjectToRemove: Subject) {
		const nbInitialTeachers = this.teachers.length;
		const nbForbiddenSubjectsCombination = this.forbiddenSubjectIdsCombination.size;

		this.teachers = this.teachers.filter(teacher => teacher.subjectId !== subjectToRemove.id);
		this.subjects = this.subjects.filter(subject => subject.id !== subjectToRemove.id);
		if (this.forbiddenSubjectIdsCombination.has(subjectToRemove.id)) {
			this.forbiddenSubjectIdsCombination = new Set();
		}

		return {
			hadAssociatedTeachers: nbInitialTeachers !== this.teachers.length,
			wasInForbiddenCombination: nbForbiddenSubjectsCombination !== this.forbiddenSubjectIdsCombination.size,
		}
	}

	toHumanJson() {
		return {
			groups: this.groups,
			subjects: this.subjects,
			teachers: this.teachers,
			objectives: this.objectives,
			lunchTimeRange: this.lunchTimeRange,
			forbiddenSubjectsCombination:
				this.forbiddenSubjectIdsCombination.size === 0
					? undefined
					: [...this.forbiddenSubjectIdsCombination].map(subjectId => this.findId('subjects', subjectId)!.name),
			calendar: this.calendar,
		};
	}
	
	toSolverJson() {
		return {
			groups: this.groups,
			subjects: this.subjects,
			teachers: this.teachers,
			trios: this.trios,
			weeks: this.calendar.getWorkingWeeks(),
			objectives: this.objectives,
			lunchTimeRange: this.lunchTimeRange,
			forbiddenSubjectIdsCombination: [...this.forbiddenSubjectIdsCombination],
		};
	}
	
	/** @todo Throw better error messages **/
	static fromHumanJson(json: HumanJson<State>) {
		const groups = json.groups.map(group => Group.fromHumanJson(group));
		for (let idGroup = 0; idGroup < groups.length; ++idGroup) {
			groups[idGroup] = groups[idGroup].setNextGroupFromHumanJson(json.groups[idGroup], groups);
		}
		
		const subjects = json.subjects.map(subject => Subject.fromHumanJson(subject));
		const teachers = json.teachers.map(teacher => Teacher.fromHumanJson(teacher, subjects)!);
		
		const objectives = [];
		for (const objective of json.objectives) {
			objectives.push(defaultObjectives.find(defaultObjective => defaultObjective.name === objective)!);
		}
		
		const lunchTimeRange = json.lunchTimeRange ?? defaultLunchTimeRange;

		const forbiddenSubjectIdsCombination = new Set(
			json.forbiddenSubjectsCombination === undefined ? [] : json.forbiddenSubjectsCombination.map(
				subjectName => subjects.find(subject => subject.name === subjectName)!.id
			)
		);

		const calendar = Calendar.fromHumanJson(json.calendar);
		
		return new State([], groups, subjects, teachers, [], objectives, lunchTimeRange, forbiddenSubjectIdsCombination, calendar);
	}
}
