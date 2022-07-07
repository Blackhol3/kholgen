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
	@Input('subject') subject: Subject | undefined;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, (control: AbstractControl) => this.existingNameValidator(control)]],
		shortName: ['', Validators.required],
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
			if (control.valid && subject[key] !== control.value) {
				this.undoStack.actions.update(`subjects[${this.settings.subjects.indexOf(subject)}].${key}`, control.value);
			}
			else {
				control.markAsTouched();
			}
		}
	}

	protected existingNameValidator(control: AbstractControl): ValidationErrors | null {
		for (let subject of this.settings.subjects) {
			if (subject !== this.subject && subject.name === control.value) {
				return {existingName: {subject: subject }};
			}
		}
		
		return null;
	}
}
