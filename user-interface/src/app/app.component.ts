import { Component } from '@angular/core';
import { QWebChannel } from 'qwebchannel';

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
	
	constructor() {
		let websocket = new WebSocket('ws://localhost:4201');
		websocket.addEventListener('open', () => {
			new QWebChannel(websocket, (channel: any) => {
				let communication = channel.objects.communication;
				communication.sendJsonColles.connect((jsonColles: any[]) => this.importJsonColles(jsonColles));
				communication.sendJsonSettings.connect((jsonSettings: any) => this.importJsonSettings(jsonSettings));
			});
		});
	}
	
	protected importJsonColles(jsonColles: any[]) {
		this.colles = jsonColles.map((colle: any) => new Colle(
			this.teachers.find(teacher => teacher.name === colle.teacherName) as Teacher,
			new Timeslot(colle.timeslot.day, colle.timeslot.hour),
			this.trios[colle.trioId],
			this.weeks[colle.weekId],
		));
	}
	
	protected importJsonSettings(jsonSettings: any) {
		this.subjects = jsonSettings.subjects.map((subject: any) => new Subject(
			subject.name, subject.shortName, subject.frequency, subject.color
		));
		
		this.teachers = jsonSettings.teachers.map((teacher: any) => new Teacher(
			teacher.name,
			this.subjects.find(subject => subject.name === teacher.subjectName) as Subject,
			teacher.availableTimeslots.map((timeslot: any) => new Timeslot(timeslot.day, timeslot.hour)),
		));
		
		this.trios = jsonSettings.trios.map((trio: any) => new Trio(
			trio.id,
		));
		
		this.weeks = [];
		for (let i = 0; i < jsonSettings.nbWeeks; ++i) {
			this.weeks.push(new Week(i));
		}
		
		this.colles = [];
	}
}
