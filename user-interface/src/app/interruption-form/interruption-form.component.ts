import { ChangeDetectionStrategy, Component, Inject, Input, type OnInit, type OnChanges } from '@angular/core';
import { type AbstractControl, FormsModule, NonNullableFormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';

import { MAT_DATE_RANGE_SELECTION_STRATEGY } from '@angular/material/datepicker';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';

import { DateTime, Interval } from 'luxon';

import { type Entries, notUniqueValidator, trimValidator } from '../misc';
import { Interruption } from '../interruption';

import { StoreService } from '../store.service';
import { ToNextInterruptionSelectionStrategyService } from '../to-next-interruption-selection-strategy.service'
import { UndoStackService } from '../undo-stack.service';

import { IntervalInputComponent } from '../interval-input/interval-input.component';

@Component({
	selector: 'app-interruption-form',
	templateUrl: './interruption-form.component.html',
	styleUrls: ['./interruption-form.component.scss'],
	providers: [{
		provide: MAT_DATE_RANGE_SELECTION_STRATEGY,
		useClass: ToNextInterruptionSelectionStrategyService,
	}],
	changeDetection: ChangeDetectionStrategy.OnPush,
	standalone: true,
	imports: [
		FormsModule,
		ReactiveFormsModule,
		
		MatFormFieldModule,
		MatInputModule,

		IntervalInputComponent,
	],
})
export class InterruptionFormComponent implements OnInit, OnChanges {
	@Input({required: true}) interruption!: Interruption;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, trimValidator, (control: AbstractControl<string, string>) => notUniqueValidator(control, 'name', this.interruption, this.store.state.calendar.interruptions)]],
		interval: [Interval.fromDateTimes(DateTime.now(), DateTime.now()), [Validators.required]],
	});
	
	constructor(
		private store: StoreService,
		private undoStack: UndoStackService,
		private formBuilder: NonNullableFormBuilder,
		@Inject(MAT_DATE_RANGE_SELECTION_STRATEGY) private selectionStrategy: ToNextInterruptionSelectionStrategyService
	) {
		this.form.valueChanges.subscribe(() => this.formChange());
	}

	ngOnInit() {
		this.updateForm();
	}
	
	ngOnChanges() {
		this.updateForm();
	}
	
	updateForm() {
		this.selectionStrategy.ignoredInterruptions = [this.interruption];
		this.form.setValue({
			name: this.interruption.name,
			interval: this.interruption.interval,
		}, {emitEvent: false});
	}
	
	formChange() {
		for (const [key, control] of Object.entries(this.form.controls) as Entries<typeof this.form.controls>) {
			if (!control.valid) {
				control.markAsTouched();
				continue;
			}

			if (
				(key === 'name' && this.interruption.name !== control.value) ||
				(key === 'interval' && !this.interruption.interval.equals(control.value))
			) {
				this.undoStack.do(state => {
					(state.calendar.findInterruptionId(this.interruption.id)![key] as unknown) = control.value;
				});
			}
		}
	}

	dateFilter = (date: DateTime | null) => {
		return date === null || this.store.state.calendar.isWorkingDay(date, this.interruption.id);
	}
}
