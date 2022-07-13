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
		this.checkIfOpen();
		
		if (this.computeSubject !== undefined) {
			return this.computeSubject.asObservable();
		}
		
		this.computeSubject = new Subject<Colle[]>();
		this.communication.newColles.connect((jsonColles: any[]) => this.computeSubject?.next(this.importJsonColles(settings, jsonColles)));
		this.communication.computationFinished.connect(() => {
			this.computeSubject?.complete();
			this.computeSubject = undefined;
		});
		this.communication.compute({
			subjects: settings.subjects,
			teachers: settings.teachers,
			numberOfTrios: settings.trios.length,
			numberOfWeeks: settings.weeks.length,
		});
		
		return this.computeSubject.asObservable();
	}
	
	stopComputation() {
		this.checkIfOpen();
		this.communication.stopComputation();
	}
	
	async exportAsCsv(): Promise<Blob> {
		this.checkIfOpen();
		const csv = await this.communication.exportAsCsv() as string;
		
		const blob = new Blob(
			[csv],
			{type: 'text/csv'},
		);
		
		return blob;
	}
	
	async exportAsExcel(): Promise<Blob> {
		this.checkIfOpen();
		const base64ByteArray = await this.communication.exportAsExcel() as string;
		
		/** @link https://stackoverflow.com/questions/16245768/creating-a-blob-from-a-base64-string-in-javascript/2057033#2057033 */
		const byteCharacters = atob(base64ByteArray);
		const byteNumbers = new Array(byteCharacters.length);
		for (let i = 0; i < byteCharacters.length; ++i) {
			byteNumbers[i] = byteCharacters.charCodeAt(i);
		}
		const byteArray = new Uint8Array(byteNumbers);
		
		const blob = new Blob(
			[byteArray],
			{type: 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet'},
		);
		
		return blob;
	}
	
	protected checkIfOpen() {
		if (this.communication === undefined) {
			throw 'The communication is not opened.';
		}
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
