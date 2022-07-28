import { Injectable } from '@angular/core';

import { Colle } from './colle';
import { Objective } from './objective';
import { Subject } from './subject';
import { Teacher } from './teacher';
import { Trio } from './trio';
import { Week } from './week';

@Injectable({
	providedIn: 'root'
})
export class StateService {
	readonly defaultObjectives = [
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
	];
	
	colles: Colle[] = [];
	subjects: Subject[] = [];
	teachers: Teacher[] = [];
	trios: Trio[] = [];
	weeks: Week[] = [];
	objectives: Objective[] = this.defaultObjectives.slice();
	
	constructor() { }
	
	toHumanObject() {
		return {
			subjects: this.subjects,
			teachers: this.teachers.map(teacher => ({
				name: teacher.name,
				subject: teacher.subject.name,
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
}
