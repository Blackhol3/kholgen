import { immerable, type Draft } from 'immer';

import { Calendar } from './calendar';
import { Colle } from './colle';
import { Group } from './group';
import { Objective } from './objective';
import { Subject } from './subject';
import { Teacher } from './teacher';
import { Trio } from './trio';

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

export class State {
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

	toHumanJsonObject() {
		return {
			groups: this.groups.map(group => group.toHumanJsonObject(this)),
			subjects: this.subjects.map(subject => subject.toHumanJsonObject()),
			teachers: this.teachers.map(teacher => teacher.toHumanJsonObject(this)),
			objectives: this.objectives.map(objective => objective.toHumanJsonObject()),
			lunchTimeRange: this.lunchTimeRange,
			forbiddenSubjectsCombination:
				this.forbiddenSubjectIdsCombination.size === 0
					? undefined
					: [...this.forbiddenSubjectIdsCombination].map(subjectId => this.findId('subjects', subjectId)!.name),
		};
	}
	
	toSolverJsonObject() {
		return {
			groups: this.groups,
			subjects: this.subjects,
			teachers: this.teachers,
			trios: this.trios,
			weeks: this.calendar.getWorkingWeeks().map(week => week.toSolverJsonObject()),
			objectives: this.objectives.map(objective => objective.name),
			lunchTimeRange: this.lunchTimeRange,
			forbiddenSubjectIdsCombination: [...this.forbiddenSubjectIdsCombination],
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
		
		const objectives = [];
		for (const objective of jsonObject.objectives) {
			objectives.push(defaultObjectives.find(defaultObjective => defaultObjective.name === objective)!);
		}
		
		const lunchTimeRange = jsonObject.lunchTimeRange ?? defaultLunchTimeRange;

		const forbiddenSubjectIdsCombination = new Set(
			jsonObject.forbiddenSubjectsCombination === undefined ? [] : jsonObject.forbiddenSubjectsCombination.map(
				subjectName => subjects.find(subject => subject.name === subjectName)!.id
			)
		);
		
		return new State([], groups, subjects, teachers, [], objectives, lunchTimeRange, forbiddenSubjectIdsCombination);
	}
}
