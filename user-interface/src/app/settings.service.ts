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
export class SettingsService {
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
}
