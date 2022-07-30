import { animate, style, transition, trigger } from '@angular/animations';
import { Component, OnInit, OnDestroy, ViewChild } from '@angular/core';
import { MatTable } from '@angular/material/table';
import { Subscription } from 'rxjs';
import * as FileSaver from 'file-saver-es';

import { CommunicationService } from '../communication.service';
import { StoreService } from '../store.service';

import { Trio } from '../trio';

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
export class ComputationPageComponent implements OnInit, OnDestroy {
	isRunning = false;
	storeSubscription: Subscription | undefined;
	@ViewChild(MatTable) objectivesTable: MatTable<any> | undefined;
	
	constructor(private communication: CommunicationService, public store: StoreService) {	}
	
	ngOnInit() {
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.objectivesTable!.renderRows());
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}
	
	compute() {
		this.store.do(state => {
			state.trios = [];
			for (let group of state.groups) {
				for (let i = 0; i < group.numberOfTrios; ++i) {
					state.trios.push(new Trio(state.trios.length, group.id));
				}
			}
		});
		
		this.communication.connect().then(() => {
			this.isRunning = true;
			this.communication.compute(this.store).subscribe({
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
