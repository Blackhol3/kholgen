import { ChangeDetectionStrategy, Component, Input, OnInit, OnChanges } from '@angular/core';
import { AbstractControl, NonNullableFormBuilder , ValidationErrors, Validators } from '@angular/forms';

import { Entries, setErrors } from '../misc';
import { Teacher } from '../teacher';
import { Timeslot } from '../timeslot';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

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
		availableTimeslots: [[] as readonly Timeslot[], Validators.required],
		weeklyAvailabilityFrequency: [1, [Validators.required, Validators.min(1), Validators.pattern('^-?[0-9]*$')]],
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
			weeklyAvailabilityFrequency: this.teacher.weeklyAvailabilityFrequency,
		});
	}
	
	formChange() {
		const teacher = this.teacher;
		if (teacher === undefined) {
			return;
		}
		
		for (let [key, control] of Object.entries(this.form.controls) as Entries<typeof this.form.controls>) {
			if (control.valid && teacher[key] !== this.getControlValue(key)) {
				this.undoStack.do(
					state => {
						(state.teachers[state.teachers.findIndex(t => t.id === teacher.id)] as any)[key] = this.getControlValue(key);
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
		for (let teacher of this.store.state.teachers) {
			if (teacher !== this.teacher && teacher.subjectId === this.getControlValue('subjectId') && teacher.name === this.getControlValue('name')) {
				const error = {notUnique: {teacher: teacher}};
				setErrors(control, 'name', error);
				setErrors(control, 'subjectId', error);
				return error;
			}
		}
		
		const error = {notUnique: undefined};
		setErrors(control, 'name', error);
		setErrors(control, 'subjectId', error);
		return null;
	}
}
