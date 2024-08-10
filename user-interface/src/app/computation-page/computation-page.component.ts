import { animate, style, transition, trigger } from '@angular/animations';
import { ChangeDetectionStrategy, ChangeDetectorRef, Component, type OnDestroy, type OnInit, inject } from '@angular/core';

import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatProgressBarModule } from '@angular/material/progress-bar';
import { MatTableModule } from '@angular/material/table';

import { Subscription } from 'rxjs';

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
	protected readonly changeDetectorRef = inject(ChangeDetectorRef);
	protected readonly communication = inject(CommunicationService);

	objectiveComputations: ObjectiveComputation[] = [];

	isRunning = false;
	storeSubscription: Subscription | undefined;
	
	ngOnInit() {
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.update());
		this.update();
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}

	protected update() {
		const computation = this.store.state.computation ?? this.store.state.prepareComputation();
		this.objectiveComputations = [...computation.objectiveComputations];
		this.changeDetectorRef.markForCheck();
	}
	
	async compute() {
		await this.communication.connect();
		this.isRunning = true;
		this.communication.compute(this.store).subscribe({
			complete: () => {
				this.isRunning = false;
				this.changeDetectorRef.markForCheck();
			},
		});
	}
	
	stopComputation() {
		this.communication.stopComputation();
	}
}
