import { animate, style, transition, trigger } from '@angular/animations';
import { Component } from '@angular/core';
import * as FileSaver from 'file-saver';

import { CommunicationService } from '../communication.service';
import { SettingsService } from '../settings.service';

@Component({
	selector: 'app-computation-page',
	templateUrl: './computation-page.component.html',
	styleUrls: ['./computation-page.component.scss'],
	animations: [
		trigger('insertAnimation', [
			transition(':enter', [
				style({height: 0}),
				animate('200ms', style({height: '*'})),
			]),
			transition(':leave', [
				style({height: '*'}),
				animate('150ms', style({height: 0})),
			]),
		]),
	],
})
export class ComputationPageComponent {
	isRunning = false;
	
	constructor(private communication: CommunicationService, public settings: SettingsService) { }
	
	compute() {
		this.communication.connect().then(() => {
			this.isRunning = true;
			this.communication.compute(this.settings).subscribe({
				next: colles => { this.settings.colles = colles; },
				complete: () => { this.isRunning = false; },
			});
		});
	}
	
	stopComputation() {
		this.communication.stopComputation();
	}
	
	async saveAsCsv() {
		FileSaver.saveAs(await this.communication.exportAsCsv(), 'Colloscope.csv');
	}
	
	async saveAsExcel() {
		FileSaver.saveAs(await this.communication.exportAsExcel(), 'Colloscope.xlsx');
	}
}
