import { Component, Input, OnInit, OnChanges, OnDestroy } from '@angular/core';
import { AbstractControl, FormControl, FormBuilder, ValidationErrors, Validators } from '@angular/forms';
import { Subscription } from 'rxjs';

import { Subject } from '../subject';
import { Teacher } from '../teacher';
import { Timeslot } from '../timeslot';
import { SettingsService } from '../settings.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-teacher-form',
	templateUrl: './teacher-form.component.html',
	styleUrls: ['./teacher-form.component.scss']
})
export class TeacherFormComponent implements  OnInit, OnChanges, OnDestroy {
	@Input() teacher: Teacher | undefined;
	
	form = this.formBuilder.group({
		name: ['', Validators.required],
		subject: [new Subject('', '', 0, ''), Validators.required],
		availableTimeslots: new FormControl([] as Timeslot[], {validators: Validators.required, nonNullable: true}),
	}, {validators: control => this.notUniqueValidator(control)});
	undoStackSubscription: Subscription | undefined;
	
	constructor(public settings: SettingsService, private undoStack: UndoStackService, private formBuilder: FormBuilder) {
		this.form.valueChanges.subscribe(() => this.formChange());
	}

	ngOnInit() {
		this.updateForm();
		this.undoStackSubscription = this.undoStack.changeObservable.subscribe(() => this.updateForm());
	}
	
	ngOnChanges() {
		this.updateForm();
	}
	
	ngOnDestroy() {
		this.undoStackSubscription?.unsubscribe();
	}
	
	updateForm() {
		if (this.teacher === undefined) {
			throw 'Teacher cannot be undefined.';
		}
		
		this.form.setValue(this.teacher);
	}
	
	formChange() {
		let teacher = this.teacher as any;
		for (let [key, control] of Object.entries(this.form.controls)) {
			if (control.valid && teacher[key] !== this.getControlValue(key)) {
				this.undoStack.actions.update(`teachers[${this.settings.teachers.indexOf(teacher)}].${key}`, this.getControlValue(key), key !== 'availableTimeslots');
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
			subject: control.get('subject')?.errors ?? {},
		};
		
		for (let teacher of this.settings.teachers) {
			if (teacher !== this.teacher && teacher.subject === this.getControlValue('subject') && teacher.name === this.getControlValue('name')) {
				const error = {notUnique: {teacher: teacher}};
				control.get('name')?.setErrors(Object.assign({}, errors.name, error));
				control.get('subject')?.setErrors(Object.assign({}, errors.subject, error));
				return error;
			}
		}
		
		delete errors.name['notUnique'];
		delete errors.subject['notUnique'];
		control.get('name')?.setErrors(Object.values(errors.name).length === 0 ? null : errors.name);
		control.get('subject')?.setErrors(Object.values(errors.subject).length === 0 ? null : errors.subject);
		
		return null;
	}
}
