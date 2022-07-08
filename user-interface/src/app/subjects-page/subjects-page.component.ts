import { animate, query, stagger, style, transition, trigger } from '@angular/animations';
import { Component, OnInit, OnDestroy } from '@angular/core';
import { MatSnackBar } from '@angular/material/snack-bar';
import { firstValueFrom, Subscription } from 'rxjs';

import { Subject } from '../subject';
import { SettingsService } from '../settings.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-subjects-page',
	templateUrl: './subjects-page.component.html',
	styleUrls: ['./subjects-page.component.scss'],
	animations: [
		trigger('slideAnimation', [
			transition(':enter', [
				query('*', [
					style({left: '-100%', maxWidth: 0}),
					animate('200ms', style({left: 0, maxWidth: '210px'})),
				]),
			]),
			transition(':leave', [
				query('*', [
					style({left: 0, maxWidth: '210px'}),
					animate('150ms', style({left: '-100%', maxWidth: 0})),
				]),
			]),
		]),
		
		/** @link https://ultimatecourses.com/blog/angular-animations-how-to-animate-lists */
		trigger('listAnimation', [
			transition('* <=> *', [
				query(':enter', [
						style({opacity: 0}),
						stagger('50ms', animate('200ms ease-out', style({opacity: 1})))
					], {optional: true}
				),
				query(':leave',
					animate('150ms', style({opacity: 0})),
					{optional: true}
				),
			]),
		]),
	],
})
export class SubjectsPageComponent implements OnInit, OnDestroy {
	selectedSubjects: Subject[] = [];
	undoStackSubscription: Subscription | undefined;
	
	constructor(public settings: SettingsService, private snackBar: MatSnackBar, private undoStack: UndoStackService) { }
	
	ngOnInit() {
		this.undoStackSubscription = this.undoStack.changeObservable.subscribe(() => this.updateSelectedSubject());
	}
	
	ngOnDestroy() {
		this.undoStackSubscription?.unsubscribe();
	}
	
	addNewSubject() {
		let name = '';
		for (let i = 1; name = `Matière ${i}`, this.settings.subjects.some((subject: Subject) => subject.name === name || subject.shortName === name); ++i) {
		}
		
		this.undoStack.actions.push('subjects', new Subject(name, name, 1, '#aaaaaa'));
	}
	
	deleteSubject() {
		const subject = this.selectedSubjects[0];
		const index = this.settings.subjects.indexOf(subject);
		let hasAssociatedTeachers = false;
		
		this.undoStack.startGroup();
		for (let i = this.settings.teachers.length - 1; i >= 0; --i) {
			if (this.settings.teachers[i].subject === subject) {
				this.undoStack.actions.splice('teachers', i);
				hasAssociatedTeachers = true;
			}
		}
		this.undoStack.actions.splice('subjects', index);
		this.undoStack.endGroup();
		
		this.selectedSubjects = this.settings.subjects.length > 0 ? [this.settings.subjects[Math.max(0, index - 1)]] : [];
		
		if (hasAssociatedTeachers) {
			let observable = this.snackBar.open(
				`Un ou plusieurs enseignant·es de « ${subject.name} » ont également été supprimé·es.`,
				'Annuler',
				{duration: 3000}
			).afterDismissed();
			
			firstValueFrom(observable).then(event => {
				if (event?.dismissedByAction) {
					this.undoStack.undo();
				}
			});
		}
	}
	
	updateSelectedSubject() {
		const index = this.settings.subjects.indexOf(this.selectedSubjects[0]);
		if (index === -1) {
			this.selectedSubjects = this.settings.subjects.length > 0 ? [this.settings.subjects[this.settings.subjects.length - 1]] : [];
		}
	}
}
