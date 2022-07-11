import { Component, OnInit, OnDestroy } from '@angular/core';
import { CdkDragDrop } from '@angular/cdk/drag-drop';
import { MatSnackBar } from '@angular/material/snack-bar';
import { firstValueFrom, Subscription } from 'rxjs';

import { listAnimation, slideAnimation } from '../animations';
import { Subject } from '../subject';
import { SettingsService } from '../settings.service';
import { UndoStackService } from '../undo-stack.service';

const standardSubjects = {
	mathematics: new Subject("Mathématiques", "M", 1, "#B82035"),
	physics: new Subject("Physique", "φ", 1, "#E9D844"),
	biology: new Subject("Biologie", "B", 1, "#62AC49"),
	engineering: new Subject("Sciences de l'ingénieur", "SI", 1, "#95D7E4"),
	english: new Subject("Anglais", "A", 1, "#6F0989"),
	geography: new Subject("Géographie", "G", 1, "#2B3918"),
	historyAndGeography: new Subject("Histoire-Géographie", "HG", 1, "#2B3918"),
	informatics: new Subject("Informatique", "I", 1, "#2955A9"),
};

type StandardClass = [keyof typeof standardSubjects, number][];
const standardClasses: {[className: string]: StandardClass}[] = [
	{
		MPSI: [['mathematics', 1], ['physics', 2], ['english', 2]],
		'PCSI (commun)': [['mathematics', 2], ['physics', 2], ['engineering', 4], ['english', 2]],
		'PCSI (option PC)': [['mathematics', 2], ['physics', 1], ['english', 2]],
		'PCSI (option PSI)': [['mathematics', 2], ['physics', 2], ['engineering', 4], ['english', 2]],
		PTSI: [['mathematics', 2], ['physics', 2], ['engineering', 2], ['english', 2]],
		TSI: [['mathematics', 1], ['physics', 2], ['engineering', 2], ['english', 2]],
		TPC: [['mathematics', 1], ['physics', 2], ['engineering', 2], ['english', 2]],
		BCPST: [['mathematics', 2], ['physics', 2], ['biology', 2], ['english', 4], ['informatics', 4]],
		MT: [['mathematics', 1], ['physics', 2], ['english', 2]],
	},
	{
		MP: [['mathematics', 1], ['physics', 2], ['english', 2]],
		PC: [['mathematics', 2], ['physics', 1], ['english', 2]],
		PSI: [['mathematics', 2], ['physics', 2], ['engineering', 4], ['english', 2]],
		PT: [['mathematics', 2], ['physics', 2], ['engineering', 2], ['english', 2]],
		TSI: [['mathematics', 1], ['physics', 2], ['engineering', 2], ['english', 2]],
		TPC: [['mathematics', 2], ['physics', 1], ['engineering', 2], ['english', 2]],
		BCPST: [['mathematics', 2], ['physics', 2], ['biology', 2], ['english', 4], ['geography', 4], ['informatics', 4]],
		MT: [['mathematics', 1], ['physics', 2], ['engineering', 4], ['english', 2]],
		TB: [['mathematics', 2], ['physics', 2], ['biology', 2], ['engineering', 2], ['english', 4], ['historyAndGeography', 4]],
	},
];

@Component({
	selector: 'app-subjects-page',
	templateUrl: './subjects-page.component.html',
	styleUrls: ['./subjects-page.component.scss'],
	animations: [
		listAnimation,
		slideAnimation,
	],
})
export class SubjectsPageComponent implements OnInit, OnDestroy {
	selectedSubjects: Subject[] = [];
	undoStackSubscription: Subscription | undefined;
	
	selectedStandardClass: StandardClass | undefined;
	readonly standardClasses = standardClasses;
	
	constructor(public settings: SettingsService, private snackBar: MatSnackBar, private undoStack: UndoStackService) { }
	
	ngOnInit() {
		this.undoStackSubscription = this.undoStack.changeObservable.subscribe(() => this.updateSelectedSubject());
	}
	
	ngOnDestroy() {
		this.undoStackSubscription?.unsubscribe();
	}
	
	onDrop($event: CdkDragDrop<any[]>) {
		this.selectedSubjects = [this.settings.subjects[$event.previousIndex]];
		this.undoStack.actions.move('subjects', $event.previousIndex, $event.currentIndex);
	}
	
	addNewSubject() {
		let name = '';
		for (let i = 1; name = `Matière ${i}`, this.settings.subjects.some(subject => subject.name === name || subject.shortName === name); ++i) {
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
	
	useStandardClass() {
		if (this.selectedStandardClass === undefined) {
			return;
		}
		
		this.undoStack.startGroup();
		this.undoStack.actions.clear('subjects');
		this.undoStack.actions.clear('teachers');
		
		for (let subject of this.selectedStandardClass) {
			const standardSubject = standardSubjects[subject[0]];
			this.undoStack.actions.push('subjects', new Subject(standardSubject.name, standardSubject.shortName, subject[1], standardSubject.color));
		}
		
		this.undoStack.endGroup();
	}
}
