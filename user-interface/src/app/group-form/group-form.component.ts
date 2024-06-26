import { COMMA, ENTER, SPACE } from '@angular/cdk/keycodes';
import { ChangeDetectionStrategy, Component, Input, type OnInit, type OnChanges } from '@angular/core';
import { AbstractControl, FormsModule, ReactiveFormsModule, NonNullableFormBuilder, type ValidationErrors, Validators } from '@angular/forms';

import { type MatChipInputEvent } from '@angular/material/chips';
import { MatOptionModule } from '@angular/material/core';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';

import { type Entries, equalIterables, notUniqueValidator, setErrors, trimValidator } from '../misc';
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
	@Input({required: true}) group!: Group;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, trimValidator, (control: AbstractControl<string, string>) => notUniqueValidator(control, 'name', this.group, this.store.state.groups)]],
		trioIds: [new Set() as ReadonlySet<number>, Validators.required],
		availableTimeslots: [[] as readonly Timeslot[], Validators.required],
		nextGroupId: ['' as (string | null)],
		duration: [0 as (number | null)],
	}, {validators: control => this.nextGroupRequiredValidator(control)});
	readonly separatorKeysCodes = [COMMA, ENTER, SPACE] as const;
	
	constructor(public store: StoreService, private undoStack: UndoStackService, private formBuilder: NonNullableFormBuilder ) {
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
		});
	}
	
	formChange() {
		for (const [key, control] of Object.entries(this.form.controls) as Entries<typeof this.form.controls>) {
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
		
		this.form.controls.duration[this.form.controls.nextGroupId.value === null ? 'disable' : 'enable']({emitEvent: false});
	}

	/** @todo Show an error on invalid inputs */
	addTrioIds(event: MatChipInputEvent) {
		const results = event.value.trim().match(/^([0-9]+)(?:-([0-9]+))?$/);
		if (results === null) {
			return;
		}

		const trioIds = [...this.form.controls.trioIds.value];
		if (results[2] === undefined) {
			trioIds.push(parseInt(results[1]));
		}
		else {
			const minTrioId = Math.min(parseInt(results[1]), parseInt(results[2]));
			const maxTrioId = Math.max(parseInt(results[1]), parseInt(results[2]));

			for (let trioId = minTrioId; trioId <= maxTrioId; ++trioId) {
				trioIds.push(trioId);
			}
		}

		trioIds.sort((a, b) => a - b);
		const trioIdsSet = new Set(trioIds);
		if (!equalIterables(trioIdsSet, this.form.controls.trioIds.value)) {
			this.form.controls.trioIds.setValue(trioIdsSet);
		}
		event.chipInput.clear();
	}

	removeTrioId(trioId: number) {
		const trioIds = new Set(this.form.controls.trioIds.value);
		trioIds.delete(trioId);
		this.form.controls.trioIds.setValue(trioIds);
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
