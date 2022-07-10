import { Component, OnInit, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';

import { listAnimation, slideAnimation } from '../animations';
import { Teacher } from '../teacher';
import { UndoStackService } from '../undo-stack.service';
import { SettingsService } from '../settings.service';

@Component({
	selector: 'app-teachers-page',
	templateUrl: './teachers-page.component.html',
	styleUrls: ['./teachers-page.component.scss'],
	animations: [
		listAnimation,
		slideAnimation,
	],
})
export class TeachersPageComponent implements OnInit, OnDestroy {
	selectedTeachers: Teacher[] = [];
	undoStackSubscription: Subscription | undefined;
	
	constructor(public settings: SettingsService, private undoStack: UndoStackService) { }

	ngOnInit() {
		this.undoStackSubscription = this.undoStack.changeObservable.subscribe(() => this.updateSelectedTeacher());
	}
	
	ngOnDestroy() {
		this.undoStackSubscription?.unsubscribe();
	}
	
	addNewTeacher() {
		let name = '';
		for (let i = 1; name = `Enseignant ${i}`, this.settings.teachers.some(teacher => teacher.name === name); ++i) {
		}
		
		this.undoStack.actions.push('teachers', new Teacher(name, this.selectedTeachers[0]?.subject ?? this.settings.subjects[0], []));
	}
	
	deleteTeacher() {
		const teacher = this.selectedTeachers[0];
		const index = this.settings.teachers.indexOf(teacher);
		
		this.undoStack.actions.splice('teachers', index);
		this.selectedTeachers = this.settings.teachers.length > 0 ? [this.settings.teachers[Math.max(0, index - 1)]] : [];
	}
	
	updateSelectedTeacher() {
		const index = this.settings.teachers.indexOf(this.selectedTeachers[0]);
		if (index === -1) {
			this.selectedTeachers = this.settings.teachers.length > 0 ? [this.settings.teachers[this.settings.teachers.length - 1]] : [];
		}
	}
}
