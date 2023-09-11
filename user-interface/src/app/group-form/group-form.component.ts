import { COMMA, ENTER, SPACE } from '@angular/cdk/keycodes';
import { ChangeDetectionStrategy, Component, Input, OnInit, OnChanges } from '@angular/core';
import { AbstractControl, NonNullableFormBuilder, ValidationErrors, Validators } from '@angular/forms';
import { MatChipInputEvent } from '@angular/material/chips';

import { Entries, notUniqueValidator, setErrors } from '../misc';
import { Group } from '../group';
import { Timeslot } from '../timeslot';
import { Trio } from '../trio';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-group-form',
	templateUrl: './group-form.component.html',
	styleUrls: ['./group-form.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
})
export class GroupFormComponent implements OnInit, OnChanges {
	@Input() group: Group | undefined;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, (control: AbstractControl) => notUniqueValidator(control, 'name', this.group!, this.store.state.groups)]],
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
		if (this.group === undefined) {
			throw 'Group cannot be undefined.';
		}
		
		this.form.setValue({
			name: this.group.name,
			trioIds: this.group.trioIds,
			availableTimeslots: this.group.availableTimeslots,
			nextGroupId: this.group.nextGroupId,
			duration: this.group.duration,
		});
	}
	
	formChange() {
		const group = this.group;
		if (group === undefined) {
			return;
		}
		
		for (let [key, control] of Object.entries(this.form.controls) as Entries<typeof this.form.controls>) {
			if (control.valid && group[key] !== this.getControlValue(key)) {
				this.undoStack.do(
					state => {
						(state.groups[state.groups.findIndex(g => g.id === group.id)] as any)[key] = this.getControlValue(key);
					},
					key !== 'availableTimeslots'
				);
			}
			else {
				control.markAsTouched();
			}
		}
		
		this.form.controls.duration[this.getControlValue('nextGroupId') === null ? 'disable' : 'enable']({emitEvent: false});
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
		if (trioIdsSet.size != this.form.controls.trioIds.value.size || [...trioIdsSet].some(id => !this.form.controls.trioIds.value.has(id))) {
			this.form.controls.trioIds.setValue(trioIdsSet);
		}
		event.chipInput.clear();
	}

	removeTrioId(trioId: number) {
		const trioIds = new Set(this.form.controls.trioIds.value);
		trioIds.delete(trioId);
		this.form.controls.trioIds.setValue(trioIds);
	}
	
	protected getControlValue(key: string) {
		const value = this.form?.controls[key as keyof typeof this.form.controls].value;
		return typeof value === 'string' ? value.trim() : value;
	}
	
	protected nextGroupRequiredValidator(control: AbstractControl): ValidationErrors | null {
		const nextGroupId = this.getControlValue('nextGroupId');
		const duration = this.getControlValue('duration');
		
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
