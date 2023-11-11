import { animate, style, transition, trigger } from '@angular/animations';
import { Component, OnInit, OnDestroy, ViewChild } from '@angular/core';
import { NgIf } from '@angular/common';

import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatMenuModule } from '@angular/material/menu';
import { MatProgressBarModule } from '@angular/material/progress-bar';
import { MatTable, MatTableModule } from '@angular/material/table';

import { castDraft } from 'immer';
import { Subscription } from 'rxjs';
import * as FileSaver from 'file-saver-es';

import { CommunicationService } from '../communication.service';
import { StoreService } from '../store.service';

import { Trio } from '../trio';
import { ColloscopeComponent } from '../colloscope/colloscope.component';

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
	standalone: true,
	imports: [
		NgIf,

		MatButtonModule,
		MatIconModule,
		MatMenuModule,
		MatProgressBarModule,
		MatTableModule,
		
		ColloscopeComponent,
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
	
	/** @todo Do not renumbered trios */
	compute() {
		this.store.do(state => {
			const trioInitialGroupIds: string[][] = [];
			for (let group of state.groups) {
				for (let trioId of group.trioIds) {
					if (trioInitialGroupIds[trioId] === undefined) {
						trioInitialGroupIds[trioId] = [];
					}
					trioInitialGroupIds[trioId].push(group.id);
				}
			}

			state.trios = [];
			for (let initialGroupIds of trioInitialGroupIds.filter(x => x !== undefined)) {
				state.trios.push(castDraft(new Trio(state.trios.length, initialGroupIds)));
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
