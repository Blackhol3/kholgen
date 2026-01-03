import { animate, style, transition, trigger } from '@angular/animations';
import { ChangeDetectionStrategy, ChangeDetectorRef, Component, inject, signal } from '@angular/core';

import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatProgressBarModule } from '@angular/material/progress-bar';
import { MatTableModule } from '@angular/material/table';

import { effectOn } from '../misc';
import { ObjectiveComputation } from '../objective-computation';

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
	changeDetection: ChangeDetectionStrategy.OnPush,
	imports: [
		MatButtonModule,
		MatIconModule,
		MatProgressBarModule,
		MatTableModule,
		
		ColloscopeComponent,
	],
})
export class ComputationPageComponent {
	readonly store = inject(StoreService);
	protected readonly changeDetectorRef = inject(ChangeDetectorRef);
	protected readonly communication = inject(CommunicationService);

	objectiveComputations = signal<readonly ObjectiveComputation[]>([]);
	isRunning = signal(false);

	constructor() {
		effectOn(this.store.state, () => this.update());
	}

	protected update() {
		const computation = this.store.state().computation ?? this.store.state().prepareComputation();
		this.objectiveComputations.set([...computation.objectiveComputations]);
	}
	
	async compute() {
		if (await this.communication.connect() === false) {
			return;
		}
		
		this.isRunning.set(true);
		this.communication.compute(this.store).subscribe({
			complete: () => {
				this.isRunning.set(false);
			},
		});
	}
	
	stopComputation() {
		this.communication.stopComputation();
	}
}
