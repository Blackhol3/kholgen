import { Injectable } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { QWebChannel } from 'qwebchannel';
import { Observable, Subject } from 'rxjs';

import { ConnectionDialogComponent } from './connection-dialog/connection-dialog.component';
import { Colle } from './colle';
import { Teacher } from './teacher';
import { Timeslot } from './timeslot';
import { SettingsService } from './settings.service';

@Injectable({
	providedIn: 'root'
})
export class CommunicationService {
	protected websocket: WebSocket | undefined;
	protected communication: any;
	protected computeSubject: Subject<Colle[]> | undefined;
	
	constructor(private dialog: MatDialog) { }
	
	connect(): Promise<void> {
		return new Promise(resolve => {
			if (this.websocket !== undefined) {
				resolve();
				return;
			}
			
			let dialogTimeout: number | undefined;
			if (this.dialog.getDialogById('connection') === undefined) {
				dialogTimeout = window.setTimeout(
					() => this.dialog.open(ConnectionDialogComponent, { id: 'connection' }),
					500
				);
			}
			let closeDialog = () => {
				let dialog = this.dialog.getDialogById('connection');
				if (dialog !== undefined) {
					dialog.close();
				}
				window.clearTimeout(dialogTimeout);
			};
			 
			this.websocket = new WebSocket('ws://localhost:4201');
			this.websocket.addEventListener('open', () => {
				new QWebChannel(this.websocket!, (channel: any) => {
					this.communication = channel.objects.communication;
					
					closeDialog();
					resolve();
				});
			});
			this.websocket.addEventListener('close', (event: CloseEvent) => {
				let tryToReconnect = this.communication === undefined && !event.wasClean;
				this.websocket = undefined;
				this.communication = undefined;
				this.computeSubject = undefined;
				
				tryToReconnect ? resolve(this.connect()) : closeDialog();
			});
		});
	}
	
	disconnect(): void {
		if (this.websocket !== undefined) {
			this.websocket.close();
		}
	}
	
	compute(settings: SettingsService): Observable<Colle[]> {
		if (this.communication === undefined) {
			throw 'Cannot compute when the communication is not opened.';
		}
		
		if (this.computeSubject !== undefined) {
			return this.computeSubject.asObservable();
		}
		
		this.computeSubject = new Subject<Colle[]>();
		this.communication.newColles.connect((jsonColles: any[]) => this.computeSubject?.next(this.importJsonColles(settings, jsonColles)));
		this.communication.compute({
			subjects: settings.subjects,
			teachers: settings.teachers,
			numberOfTrios: settings.trios.length,
			numberOfWeeks: settings.weeks.length,
		});
		
		return this.computeSubject.asObservable();
	}
	
	protected importJsonColles(settings: SettingsService, jsonColles: any[]): Colle[] {
		return jsonColles.map((colle: any) => new Colle(
			settings.teachers.find(teacher => teacher.name === colle.teacherName) as Teacher,
			new Timeslot(colle.timeslot.day, colle.timeslot.hour),
			settings.trios[colle.trioId],
			settings.weeks[colle.weekId],
		));
	}
}
