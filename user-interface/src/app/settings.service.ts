import { Injectable } from '@angular/core';

import { Colle } from './colle';
import { Subject } from './subject';
import { Teacher } from './teacher';
import { Trio } from './trio';
import { Week } from './week';

@Injectable({
	providedIn: 'root'
})
export class SettingsService {
	colles: Colle[] = [];
	subjects: Subject[] = [];
	teachers: Teacher[] = [];
	trios: Trio[] = [];
	weeks: Week[] = [];
	
	constructor() { }
}
