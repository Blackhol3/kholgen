import { Component, OnInit, OnDestroy } from '@angular/core';
import { AbstractControl, FormBuilder, ValidationErrors, Validators } from '@angular/forms';
import { MatSelectionListChange } from '@angular/material/list';
import { Subscription } from 'rxjs';

import { SettingsService } from '../settings.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-subjects-page',
	templateUrl: './subjects-page.component.html',
	styleUrls: ['./subjects-page.component.scss']
})
export class SubjectsPageComponent implements OnInit, OnDestroy {
	private existingNameValidator = (control: AbstractControl): ValidationErrors | null => {
		for (let subject of this.settings.subjects) {
			if (subject !== this.settings.subjects[this.selectedSubjectIndex as number] && subject.name === control.value) {
				return {existingName: {subject: subject }};
			}
		}
		
		return null;
	};
	
	selectedSubjectIndex: number | null = null;
	subjectForm = this.formBuilder.group({
		name: ['', [Validators.required, this.existingNameValidator]],
		shortName: ['', Validators.required],
		frequency: [1, [Validators.required, Validators.min(1), Validators.pattern('^[0-9]*$')]],
		color: ['', Validators.required],
	});
	undoStackSubscription: Subscription | undefined;
	
	constructor(public settings: SettingsService, public undoStack: UndoStackService, private formBuilder: FormBuilder) {
		this.subjectForm.valueChanges.subscribe(() => this.formChange());
	}
	
	ngOnInit() {
		this.undoStackSubscription = this.undoStack.changeObservable.subscribe(() => this.updateForm());
	}
	
	ngOnDestroy() {
		this.undoStackSubscription!.unsubscribe();
	}
	
	selectionChange($event: MatSelectionListChange) {
		this.selectedSubjectIndex = $event.options[0].value as number;
		this.updateForm();
	}
	
	updateForm() {
		if (this.selectedSubjectIndex !== null) {
			this.subjectForm.setValue(this.settings.subjects[this.selectedSubjectIndex]);
		}
	}
	
	formChange() {
		let subject = this.settings.subjects[this.selectedSubjectIndex as number] as any;
		for (let [key, control] of Object.entries(this.subjectForm.controls)) {
			if (control.valid && subject[key] !== control.value) {
				subject[key] = control.value;
				this.undoStack.addUpdate(`subjects[${this.selectedSubjectIndex}].${key}`);
			}
			else {
				control.markAsTouched();
			}
		}
	}
}
