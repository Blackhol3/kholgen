import { animate, query, style, transition, trigger } from '@angular/animations';
import { Component, ElementRef, HostListener, ViewChild } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { QWebChannel } from 'qwebchannel';

import { ConnectionDialogComponent } from './connection-dialog/connection-dialog.component';

import { Colle } from './colle';
import { Subject } from './subject';
import { Teacher } from './teacher';
import { Timeslot } from './timeslot';
import { Trio } from './trio';
import { Week } from './week';

import { SettingsService } from './settings.service';
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
	
	constructor(private dialog: MatDialog, private settings: SettingsService, private undoStack: UndoStackService) {
		//this.connect();
	}
	
	startImportFile() {
		this.importFileInput.nativeElement.click();
	}
	
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
	
	/** @todo Throw better error messages **/
	import(jsonObject: any) {
		this.settings.subjects = jsonObject.subjects.map((subject: any) => new Subject(
			subject.name, subject.shortName, subject.frequency, subject.color
		));
		
		this.settings.teachers = jsonObject.teachers.map((teacher: any) => new Teacher(
			teacher.name,
			this.settings.subjects.find(subject => subject.name === teacher.subject) as Subject,
			teacher.availableTimeslots.map((timeslot: string) => Timeslot.fromString(timeslot)),
		));
		
		this.settings.trios = [];
		for (let i = 0; i < jsonObject.numberOfTrios; ++i) {
			this.settings.trios.push(new Trio(i));
		}
		
		this.settings.weeks = [];
		for (let i = 0; i < 20; ++i) {
			this.settings.weeks.push(new Week(i));
		}
		
		//this.connect();
		/** @todo Handle the undo stack better, or display a warning dialog **/
		this.undoStack.clear();
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
	
	protected connect() {
		if (this.dialog.getDialogById('connection') === undefined) {
			this.dialog.open(ConnectionDialogComponent, { id: 'connection' });
		}
		 
		let websocket = new WebSocket('ws://localhost:4201');
		websocket.addEventListener('open', () => {
			new QWebChannel(websocket, (channel: any) => {
				let communication = channel.objects.communication;
				communication.newColles.connect((jsonColles: any[]) => this.importJsonColles(jsonColles));
				
				communication.compute({
					subjects: this.settings.subjects,
					teachers: this.settings.teachers,
					numberOfTrios: this.settings.trios.length,
					numberOfWeeks: this.settings.weeks.length,
				});
			});
			
			let dialog = this.dialog.getDialogById('connection');
			if (dialog !== undefined) {
				dialog.close();
			}
		});
		websocket.addEventListener('close', () => { this.connect(); });
	}
	
	protected importJsonColles(jsonColles: any[]) {
		this.settings.colles = jsonColles.map((colle: any) => new Colle(
			this.settings.teachers.find(teacher => teacher.name === colle.teacherName) as Teacher,
			new Timeslot(colle.timeslot.day, colle.timeslot.hour),
			this.settings.trios[colle.trioId],
			this.settings.weeks[colle.weekId],
		));
	}
}
