import { Component, ElementRef, ViewChild } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { QWebChannel } from 'qwebchannel';

import { ConnectionDialogComponent } from './connection-dialog/connection-dialog.component';

import { Colle } from './colle';
import { Subject } from './subject';
import { Teacher } from './teacher';
import { Timeslot } from './timeslot';
import { Trio } from './trio';
import { Week } from './week';

@Component({
	selector: 'app-root',
	templateUrl: './app.component.html',
	styleUrls: ['./app.component.scss']
})
export class AppComponent {
	colles: Colle[] = [];
	subjects: Subject[] = [];
	teachers: Teacher[] = [];
	trios: Trio[] = [];
	weeks: Week[] = [];
	
	@ViewChild('importFileInput') importFileInput!: ElementRef<HTMLInputElement>;
	
	constructor(public dialog: MatDialog) {
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
		this.subjects = jsonObject.subjects.map((subject: any) => new Subject(
			subject.name, subject.shortName, subject.frequency, subject.color
		));
		
		this.teachers = jsonObject.teachers.map((teacher: any) => new Teacher(
			teacher.name,
			this.subjects.find(subject => subject.name === teacher.subject) as Subject,
			teacher.availableTimeslots.map((timeslot: string) => Timeslot.fromString(timeslot)),
		));
		
		this.trios = [];
		for (let i = 0; i < jsonObject.numberOfTrios; ++i) {
			this.trios.push(new Trio(i));
		}
		
		this.weeks = [];
		for (let i = 0; i < 20; ++i) {
			this.weeks.push(new Week(i));
		}
		
		this.connect();
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
					subjects: this.subjects,
					teachers: this.teachers,
					numberOfTrios: this.trios.length,
					numberOfWeeks: this.weeks.length,
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
		this.colles = jsonColles.map((colle: any) => new Colle(
			this.teachers.find(teacher => teacher.name === colle.teacherName) as Teacher,
			new Timeslot(colle.timeslot.day, colle.timeslot.hour),
			this.trios[colle.trioId],
			this.weeks[colle.weekId],
		));
	}
}
