import { Component, OnInit, OnDestroy } from '@angular/core';
import { Subscription } from 'rxjs';

import { Subject } from '../subject';
import { SettingsService } from '../settings.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-subjects-page',
	templateUrl: './subjects-page.component.html',
	styleUrls: ['./subjects-page.component.scss']
})
export class SubjectsPageComponent implements OnInit, OnDestroy {
	selectedSubjects: Subject[] = [];
	undoStackSubscription: Subscription | undefined;
	
	constructor(public settings: SettingsService, public undoStack: UndoStackService) { }
	
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
		/** @todo Supprimer les enseignants associés */
		const index = this.settings.subjects.indexOf(this.selectedSubjects[0]);
		this.undoStack.actions.splice('subjects', index);
		this.selectedSubjects = this.settings.subjects.length > 0 ? [this.settings.subjects[Math.max(0, index - 1)]] : [];
	}
	
	updateSelectedSubject() {
		const index = this.settings.subjects.indexOf(this.selectedSubjects[0]);
		if (index === -1) {
			this.selectedSubjects = this.settings.subjects.length > 0 ? [this.settings.subjects[this.settings.subjects.length - 1]] : [];
		}
	}
}
