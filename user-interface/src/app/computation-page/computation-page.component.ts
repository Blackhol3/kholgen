import { animate, style, transition, trigger } from '@angular/animations';
import { Component, type OnInit, type OnDestroy, ViewChild, inject } from '@angular/core';

import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatMenuModule } from '@angular/material/menu';
import { MatProgressBarModule } from '@angular/material/progress-bar';
import { MatTable, MatTableModule } from '@angular/material/table';

import { castDraft } from 'immer';
import { Subscription } from 'rxjs';
import * as FileSaver from 'file-saver-es';

import { Trio } from '../trio';

import { CommunicationService } from '../communication.service';
import { ICalExporterService } from '../ical-exporter.service';
import { SpreadsheetExporterService } from '../spreadsheet-exporter.service';
import { StoreService } from '../store.service';

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
		MatButtonModule,
		MatIconModule,
		MatMenuModule,
		MatProgressBarModule,
		MatTableModule,
		
		ColloscopeComponent,
	],
})
export class ComputationPageComponent implements OnInit, OnDestroy {
	readonly store = inject(StoreService);
	protected readonly communication = inject(CommunicationService);
	protected readonly iCalExporter = inject(ICalExporterService);
	protected readonly spreadsheetExporter = inject(SpreadsheetExporterService);

	isRunning = false;
	storeSubscription: Subscription | undefined;
	@ViewChild(MatTable) objectivesTable: MatTable<unknown> | undefined;
	
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
			for (const group of state.groups) {
				for (const trioId of group.trioIds) {
					if (trioInitialGroupIds[trioId] === undefined) {
						trioInitialGroupIds[trioId] = [];
					}
					trioInitialGroupIds[trioId].push(group.id);
				}
			}

			state.trios = [];
			for (const initialGroupIds of trioInitialGroupIds.filter(x => x !== undefined)) {
				state.trios.push(castDraft(new Trio(state.trios.length, initialGroupIds)));
			}
		});
		
		void this.communication.connect().then(() => {
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
		FileSaver.saveAs(await this.spreadsheetExporter.asCsv(this.store.state), 'Colloscope.csv');
	}
	
	async saveAsExcel() {
		FileSaver.saveAs(await this.spreadsheetExporter.asExcel(this.store.state), 'Colloscope.xlsx');
	}

	async saveAsICal() {
		FileSaver.saveAs(await this.iCalExporter.asZip(this.store.state), 'Test.zip');
	}
}
