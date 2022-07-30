import { ChangeDetectionStrategy, Component, Input, OnInit, OnChanges } from '@angular/core';
import { AbstractControl, FormControl, NonNullableFormBuilder , ValidationErrors, Validators } from '@angular/forms';

import { Teacher } from '../teacher';
import { Timeslot } from '../timeslot';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

/** @link https://stackoverflow.com/questions/60141960/typescript-key-value-relation-preserving-object-entries-type/60142095#60142095 */
type Entries<T> = {
    [K in keyof T]: [K, T[K]]
}[keyof T][];

@Component({
	selector: 'app-teacher-form',
	templateUrl: './teacher-form.component.html',
	styleUrls: ['./teacher-form.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
})
export class TeacherFormComponent implements OnInit, OnChanges {
	@Input() teacher: Teacher | undefined;
	
	form = this.formBuilder.group({
		name: ['', Validators.required],
		subjectId: ['', Validators.required],
		availableTimeslots: new FormControl([] as readonly Timeslot[], {validators: Validators.required}),
	}, {validators: control => this.notUniqueValidator(control)});
	
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
		if (this.teacher === undefined) {
			throw 'Teacher cannot be undefined.';
		}
		
		this.form.setValue({
			name: this.teacher.name,
			subjectId: this.teacher.subjectId,
			availableTimeslots: this.teacher.availableTimeslots,
		});
	}
	
	formChange() {
		let teacher = this.teacher;
		if (teacher === undefined) {
			return;
		}
		
		for (let [key, control] of Object.entries(this.form.controls) as Entries<typeof this.form.controls>) {
			if (control.valid && teacher[key] !== this.getControlValue(key)) {
				this.undoStack.do(
					state => {
						(state.teachers[this.store.state.teachers.indexOf(teacher!)] as any)[key] = this.getControlValue(key);
					},
					key !== 'availableTimeslots'
				);
			}
			else {
				control.markAsTouched();
			}
		}
	}
	
	protected getControlValue(key: string) {
		const value = this.form?.controls[key as keyof typeof this.form.controls].value;
		return typeof value === 'string' ? value.trim() : value;
	}
	
	protected notUniqueValidator(control: AbstractControl): ValidationErrors | null {
		let errors = {
			name: control.get('name')?.errors ?? {},
			subjectId: control.get('subjectId')?.errors ?? {},
		};
		
		for (let teacher of this.store.state.teachers) {
			if (teacher !== this.teacher && teacher.subjectId === this.getControlValue('subjectId') && teacher.name === this.getControlValue('name')) {
				const error = {notUnique: {teacher: teacher}};
				control.get('name')?.setErrors(Object.assign({}, errors.name, error));
				control.get('subjectId')?.setErrors(Object.assign({}, errors.subjectId, error));
				return error;
			}
		}
		
		delete errors.name['notUnique'];
		delete errors.subjectId['notUnique'];
		control.get('name')?.setErrors(Object.values(errors.name).length === 0 ? null : errors.name);
		control.get('subjectId')?.setErrors(Object.values(errors.subjectId).length === 0 ? null : errors.subjectId);
		
		return null;
	}
}
