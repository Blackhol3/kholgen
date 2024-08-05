import { Component, type OnInit, type OnDestroy } from '@angular/core';
import { KeyValuePipe } from '@angular/common';
import { type CdkDragDrop, CdkDropList, CdkDrag, moveItemInArray } from '@angular/cdk/drag-drop';
import { FormsModule } from '@angular/forms';

import { MatButtonModule } from '@angular/material/button';
import { MatOptionModule } from '@angular/material/core';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatListModule } from '@angular/material/list';
import { MatSelectModule } from '@angular/material/select';
import { MatSnackBar, MatSnackBarModule } from '@angular/material/snack-bar';

import { Subscription } from 'rxjs';

import { listAnimation, slideAnimation } from '../animations';
import { type HumanJson } from '../json';
import { Subject } from '../subject';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

import { CopyDataDirective } from '../copy-data.directive';
import { SubjectFormComponent } from '../subject-form/subject-form.component';

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
	standalone: true,
	imports: [
		CdkDrag,
		CdkDropList,
		FormsModule,
		KeyValuePipe,

		MatButtonModule,
		MatFormFieldModule,
		MatIconModule,
		MatListModule,
		MatOptionModule,
		MatSelectModule,
		MatSnackBarModule,

		CopyDataDirective,
		SubjectFormComponent,
	],
})
export class SubjectsPageComponent implements OnInit, OnDestroy {
	selectedSubjectIds: string[] = [];
	storeSubscription: Subscription | undefined;
	
	selectedStandardClass: StandardClass | undefined;
	readonly standardClasses = standardClasses;
	
	constructor(public store: StoreService, private snackBar: MatSnackBar, private undoStack: UndoStackService) { }
	
	ngOnInit() {
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.updateSelectedSubject());
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}
	
	onDrop($event: CdkDragDrop<unknown[]>) {
		this.selectedSubjectIds = [this.store.state.subjects[$event.previousIndex].id];
		this.undoStack.do(state => { moveItemInArray(state.subjects, $event.previousIndex, $event.currentIndex) });
	}
	
	addNewSubject() {
		let name = '';
		for (let i = 1; name = `Matière ${i}`, this.store.state.subjects.some(subject => subject.name === name || subject.shortName === name); ++i) {
			// Empty
		}
		
		const subject = new Subject(name, name, 1, '#aaaaaa');
		this.undoStack.do(state => { state.subjects.push(subject) });
		this.selectedSubjectIds = [subject.id];
	}
	
	deleteSubject() {
		const subject = this.store.state.findId('subjects', this.selectedSubjectIds[0])!;
		const index = this.store.state.subjects.indexOf(subject);
		
		let hadAssociatedTeachers = false;
		let wasInForbiddenCombination = false;

		this.undoStack.do(state => {
			({hadAssociatedTeachers, wasInForbiddenCombination} = state.removeSubject(subject));
		});
		
		this.selectedSubjectIds = this.store.state.subjects.length > 0 ? [this.store.state.subjects[Math.max(0, index - 1)].id] : [];
		
		if (hadAssociatedTeachers || wasInForbiddenCombination) {
			this.snackBar.open(
				hadAssociatedTeachers ?
					`Un ou plusieurs enseignant·es de « ${subject.name} »${wasInForbiddenCombination ? ', ainsi que la combinaison interdite de matières dans laquelle elle se trouvait,' : ''} ont également été supprimé·es.` :
					`La combinaison interdite de matières, dans laquelle se trouvait « ${subject.name} », a été supprimée.`
				,
				'Annuler',
				{duration: 5000}
			).afterDismissed().subscribe(event => {
				if (event.dismissedByAction) {
					this.undoStack.undo();
				}
			});
		}
	}
	
	updateSelectedSubject() {
		if (this.store.state.findId('subjects', this.selectedSubjectIds[0]) === undefined) {
			this.selectedSubjectIds = this.store.state.subjects.length > 0 ? [this.store.state.subjects[this.store.state.subjects.length - 1].id] : [];
		}
	}
	
	useStandardClass() {
		this.undoStack.do(state => {
			if (this.selectedStandardClass === undefined) {
				return;
			}
			
			state.subjects = [];
			state.teachers = [];
			
			for (const subject of this.selectedStandardClass) {
				const standardSubject = standardSubjects[subject[0]];
				state.subjects.push(new Subject(standardSubject.name, standardSubject.shortName, subject[1], standardSubject.color));
			}
		});
	}
	
	onPaste($event: ClipboardEvent) {
		const jsonString = $event.clipboardData?.getData('application/json-subject') ?? '';
		if (jsonString === '') {
			return;
		}
		
		const jsonSubject = JSON.parse(jsonString) as HumanJson<Subject>;
		while (this.store.state.subjects.some(subject => subject.name === jsonSubject.name || subject.shortName === jsonSubject.shortName)) {
			jsonSubject.name += ' (copie)';
			jsonSubject.shortName += ' (copie)';
		}
		
		const subject = Subject.fromHumanJson(jsonSubject);
		this.undoStack.do(state => { state.subjects.push(subject) });
		this.selectedSubjectIds = [subject.id];
	}

	protected onKeydown($event: KeyboardEvent) {
		if ($event.key === 'Delete') {
			this.deleteSubject();
			$event.preventDefault();
		}
	}
}
