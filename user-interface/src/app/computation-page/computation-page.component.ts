import { animate, style, transition, trigger } from '@angular/animations';
import { Component, type OnInit, type OnDestroy, ViewChild, inject } from '@angular/core';

import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatProgressBarModule } from '@angular/material/progress-bar';
import { MatTable, MatTableModule } from '@angular/material/table';

import { castDraft } from 'immer';
import { Subscription } from 'rxjs';

import { CommunicationService } from '../communication.service';
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
		MatProgressBarModule,
		MatTableModule,
		
		ColloscopeComponent,
	],
})
export class ComputationPageComponent implements OnInit, OnDestroy {
	readonly store = inject(StoreService);
	protected readonly communication = inject(CommunicationService);

	isRunning = false;
	storeSubscription: Subscription | undefined;
	@ViewChild(MatTable) objectivesTable: MatTable<unknown> | undefined;
	
	ngOnInit() {
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.objectivesTable!.renderRows());
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}
	
	async compute() {
		this.store.do(state => { state.trios = castDraft(state.createTrios()); });
		
		await this.communication.connect();
		this.isRunning = true;
		this.communication.compute(this.store).subscribe({
			complete: () => { this.isRunning = false; },
		});
	}
	
	stopComputation() {
		this.communication.stopComputation();
	}
}
