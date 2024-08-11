import { ChangeDetectionStrategy, Component, Input, type OnInit, type OnChanges, inject } from '@angular/core';
import { AbstractControl, FormsModule, ReactiveFormsModule, NonNullableFormBuilder, type ValidationErrors, Validators } from '@angular/forms';

import { MatOptionModule } from '@angular/material/core';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';

import { entries, equalIterables, notUniqueValidator, setErrors, trimValidator } from '../misc';
import { Group } from '../group';
import { Timeslot } from '../timeslot';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

import { WeeklyTimetableComponent } from '../weekly-timetable/weekly-timetable.component';
import { UniqueIntegersChipInputComponent } from '../unique-integers-chip-input/unique-integers-chip-input.component';

@Component({
	selector: 'app-group-form',
	templateUrl: './group-form.component.html',
	styleUrls: ['./group-form.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
	standalone: true,
	imports: [
		FormsModule,
		ReactiveFormsModule,

		MatFormFieldModule,
		MatInputModule,
		MatOptionModule,
		MatSelectModule,

		UniqueIntegersChipInputComponent,
		WeeklyTimetableComponent,
	],
})
export class GroupFormComponent implements OnInit, OnChanges {
	readonly store = inject(StoreService);
	protected readonly formBuilder = inject(NonNullableFormBuilder);
	protected readonly undoStack = inject(UndoStackService);

	@Input({required: true}) group!: Group;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, trimValidator, (control: AbstractControl<string, string>) => notUniqueValidator(control, 'name', this.group, this.store.state.groups)]],
		trioIds: [new Set() as ReadonlySet<number>],
		availableTimeslots: [[] as readonly Timeslot[]],
		nextGroupId: ['' as (string | null)],
		duration: [0 as (number | null), [Validators.min(1), Validators.pattern('^-?[0-9]*$')]],
	}, {validators: control => this.nextGroupRequiredValidator(control)});
	
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
		this.form.setValue({
			name: this.group.name,
			trioIds: this.group.trioIds,
			availableTimeslots: this.group.availableTimeslots,
			nextGroupId: this.group.nextGroupId,
			duration: this.group.duration,
		}, {emitEvent: false});

		this.updateDurationFormControlStatus();
	}
	
	formChange() {
		for (const [key, control] of entries(this.form.controls)) {
			if (control.valid && this.controlValueUpdated(key)) {
				this.undoStack.do(
					state => {
						(state.findId('groups', this.group.id)![key] as unknown) = control.value;
					},
					key !== 'availableTimeslots'
				);
			}
			else {
				control.markAsTouched();
			}
		}
		
		this.updateDurationFormControlStatus();
	}

	protected updateDurationFormControlStatus() {
		this.form.controls.duration[this.form.controls.nextGroupId.value === null ? 'disable' : 'enable']({emitEvent: false});
	}

	protected controlValueUpdated(key: keyof typeof this.form.controls): boolean {
		if (key !== 'trioIds') {
			return this.group[key] !== this.form.controls[key].value;
		}

		return !equalIterables(this.group[key], this.form.controls[key].value);
	}
	
	protected nextGroupRequiredValidator(control: AbstractControl): ValidationErrors | null {
		if (this.form === undefined) {
			return null;
		}

		const nextGroupId = this.form.controls.nextGroupId.value;
		const duration = this.form.controls.duration.value;
		
		if (nextGroupId !== null && duration === null) {
			const error = {nextGroupRequired: {nextGroupId: nextGroupId, duration: duration}};
			setErrors(control, 'nextGroupId', error);
			setErrors(control, 'duration', error);
			return error;
		}
		
		const error = {nextGroupRequired: undefined};
		setErrors(control, 'nextGroupId', error);
		setErrors(control, 'duration', error);
		return null;
	}
}
