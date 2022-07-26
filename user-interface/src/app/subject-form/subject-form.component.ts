import { Component, Input, OnInit, OnChanges, OnDestroy } from '@angular/core';
import { AbstractControl, FormBuilder, ValidationErrors, Validators } from '@angular/forms';
import { Subscription } from 'rxjs';

import { Subject } from '../subject';
import { SettingsService } from '../settings.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-subject-form',
	templateUrl: './subject-form.component.html',
	styleUrls: ['./subject-form.component.scss']
})
export class SubjectFormComponent implements OnInit, OnChanges, OnDestroy {
	@Input() subject: Subject | undefined;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, (control: AbstractControl) => this.notUniqueValidator(control, 'name')]],
		shortName: ['', [Validators.required, (control: AbstractControl) => this.notUniqueValidator(control, 'shortName')]],
		frequency: [1, [Validators.required, Validators.min(1), Validators.pattern('^[0-9]*$')]],
		color: ['', Validators.required],
	});
	undoStackSubscription: Subscription | undefined;
	
	constructor(private settings: SettingsService, private undoStack: UndoStackService, private formBuilder: FormBuilder) {
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
		if (this.subject === undefined) {
			throw 'Subject cannot be undefined.';
		}
		
		this.form.setValue(this.subject);
	}
	
	formChange() {
		let subject = this.subject as any;
		for (let [key, control] of Object.entries(this.form.controls)) {
			if (control.valid && subject[key] !== this.getControlValue(key)) {
				this.undoStack.actions.update(`subjects[${this.settings.subjects.indexOf(subject)}].${key}`, this.getControlValue(key));
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

	protected notUniqueValidator(control: AbstractControl, property: keyof Subject): ValidationErrors | null {
		for (let subject of this.settings.subjects) {
			if (subject !== this.subject && subject[property] === control.value.trim()) {
				return {notUnique: {subject: subject, property: property}};
			}
		}
		
		return null;
	}
}
