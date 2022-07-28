import { animate, query, style, transition, trigger } from '@angular/animations';
import { Component, ElementRef, HostListener, ViewChild } from '@angular/core';
import * as FileSaver from 'file-saver-es';

import { Subject } from './subject';
import { Teacher } from './teacher';
import { Timeslot } from './timeslot';
import { Trio } from './trio';
import { Week } from './week';

import { StateService } from './state.service';
import { UndoStackService } from './undo-stack.service';

@Component({
	selector: 'app-root',
	templateUrl: './app.component.html',
	styleUrls: ['./app.component.scss'],
	animations: [
		trigger('routeAnimation', [
			transition('* => *', [
				style({position: 'relative'}),
				query(':enter, :leave', style({position: 'absolute'}), {optional: true}),
				query(':enter', style({left: '-100%'}), {optional: true}),
				query(':leave', [
					animate('150ms', style({opacity: 0})),
					style({left: '-100%'}),
				], {optional: true}),
				query(':enter', [
					style({left: 'initial', opacity: 0}),
					animate('200ms', style({opacity: 1})),
				], {optional: true}),
			]),
		]),
	],
})
export class AppComponent {
	@ViewChild('importFileInput') importFileInput!: ElementRef<HTMLInputElement>;
	
	constructor(private state: StateService, public undoStack: UndoStackService) { }
	
	startImportFile() {
		this.importFileInput.nativeElement.click();
	}
	
	/** @todo Add file drag and drop */
	importFile() {
		let files = this.importFileInput.nativeElement.files;
		if (files === null || files[0] === undefined) {
			return;
		}
		
		files[0]
			.text()
			.then(JSON.parse)
			.then((jsonObject: any) => this.import(jsonObject))
			.catch((exception: any) => {
				/** @todo Display an error dialog **/
				if (exception instanceof SyntaxError) {
					console.error(exception.message);
				}
				else {
					throw exception;
				}
			})
		;
	}
	
	/** @todo Add a warning that this does not save the results, only the configuration */
	exportFile() {
		FileSaver.saveAs(
			new Blob(
				[JSON.stringify(this.state.toHumanObject(), undefined, "\t")],
				{type: 'application/json'},
			),
			'Colloscope.json',
		);
	}
	
	/** @todo Throw better error messages **/
	import(jsonObject: any) {
		this.undoStack.startGroup();
		
		this.undoStack.actions.clear('subjects');
		for (let subject of jsonObject.subjects) {
			this.undoStack.actions.push('subjects', new Subject(
				subject.name, subject.shortName, subject.frequency, subject.color
			));
		}
		
		this.undoStack.actions.clear('teachers');
		for (let teacher of jsonObject.teachers) {
			this.undoStack.actions.push('teachers', new Teacher(
				teacher.name,
				this.state.subjects.find(subject => subject.name === teacher.subject) as Subject,
				teacher.availableTimeslots.map((timeslot: string) => Timeslot.fromString(timeslot)),
			));
		}
		
		this.undoStack.actions.clear('trios');
		for (let i = 0; i < jsonObject.numberOfTrios; ++i) {
			this.undoStack.actions.push('trios', new Trio(i));
		}
		
		this.undoStack.actions.clear('weeks');
		for (let i = 0; i < 20; ++i) {
			this.undoStack.actions.push('weeks', new Week(i));
		}
		
		this.undoStack.actions.clear('objectives');
		for (let objective of jsonObject.objectives) {
			this.undoStack.actions.push('objectives', this.state.defaultObjectives.find(defaultObjective => defaultObjective.name === objective));
		}
		
		this.undoStack.endGroup();
	}
	
	@HostListener('window:keydown', ['$event'])
	protected onKeydown($event: KeyboardEvent) {
		if ($event.ctrlKey && $event.key === 'z') {
			this.undoStack.undo();
			$event.preventDefault();
		}
		else if ($event.ctrlKey && $event.key === 'y') {
			this.undoStack.redo();
			$event.preventDefault();
		}
	}
}
