import { ChangeDetectionStrategy, Component, Input, type OnInit, type OnChanges, inject } from '@angular/core';
import { type AbstractControl, FormsModule, NonNullableFormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';

import { MatCheckboxModule } from '@angular/material/checkbox'; 
import { MAT_DATE_RANGE_SELECTION_STRATEGY } from '@angular/material/datepicker';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';

import { DateTime, Interval } from 'luxon';

import { entries, notUniqueValidator, trimValidator } from '../misc';
import { Interruption } from '../interruption';

import { StoreService } from '../store.service';
import { BetweenInterruptionsSelectionStrategyService } from '../between-interruptions-selection-strategy.service'
import { UndoStackService } from '../undo-stack.service';

import { IntervalInputComponent } from '../interval-input/interval-input.component';

@Component({
	selector: 'app-interruption-form',
	templateUrl: './interruption-form.component.html',
	styleUrls: ['./interruption-form.component.scss'],
	providers: [{
		provide: MAT_DATE_RANGE_SELECTION_STRATEGY,
		useClass: BetweenInterruptionsSelectionStrategyService,
	}],
	changeDetection: ChangeDetectionStrategy.OnPush,
	standalone: true,
	imports: [
		FormsModule,
		ReactiveFormsModule,
		
		MatCheckboxModule,
		MatFormFieldModule,
		MatInputModule,

		IntervalInputComponent,
	],
})
export class InterruptionFormComponent implements OnInit, OnChanges {
	readonly store = inject(StoreService);
	protected readonly formBuilder = inject(NonNullableFormBuilder);
	protected readonly selectionStrategy = inject<BetweenInterruptionsSelectionStrategyService>(MAT_DATE_RANGE_SELECTION_STRATEGY);
	protected readonly undoStack = inject(UndoStackService);

	@Input({required: true}) interruption!: Interruption;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, trimValidator, (control: AbstractControl<string, string>) => notUniqueValidator(control, 'name', this.interruption, this.store.state.calendar.interruptions)]],
		interval: [Interval.fromDateTimes(DateTime.now(), DateTime.now()), [Validators.required]],
		weeksNumbering: [false],
		groupsRotation: [false],
	});
	
	constructor() {
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
			weeksNumbering: this.interruption.weeksNumbering,
			groupsRotation: this.interruption.groupsRotation,
		}, {emitEvent: false});
	}
	
	formChange() {
		for (const [key, control] of entries(this.form.controls)) {
			if (!control.valid) {
				control.markAsTouched();
				continue;
			}

			if (
				(key === 'interval' && !this.interruption.interval.equals(control.value)) ||
				(key !== 'interval' && this.interruption.name !== control.value)
			) {
				this.undoStack.do(state => {
					(state.calendar.findInterruptionId(this.interruption.id)![key] as unknown) = control.value;
					state.calendar.weeks = state.calendar.createWeeks();
				});
			}
		}
	}

	dateFilter = (date: DateTime | null) => {
		return date === null || this.store.state.calendar.isWorkingDay(date, this.interruption.id);
	}
}
