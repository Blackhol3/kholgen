import { Component } from '@angular/core';
import { AbstractControl, FormBuilder, ValidationErrors, Validators } from '@angular/forms';
import { MatSelectionListChange } from '@angular/material/list';

import { Subject } from '../subject';
import { SettingsService } from '../settings.service';

@Component({
	selector: 'app-subjects-page',
	templateUrl: './subjects-page.component.html',
	styleUrls: ['./subjects-page.component.scss']
})
export class SubjectsPageComponent {
	private existingNameValidator = (control: AbstractControl): ValidationErrors | null => {
		for (let subject of this.settings.subjects) {
			if (subject !== this.selectedSubject && subject.name === control.value) {
				return {existingName: {subject: subject }};
			}
		}
		
		return null;
	};
	
	selectedSubject: Subject | null = null;
	subjectForm = this.formBuilder.group({
		name: ['', [Validators.required, this.existingNameValidator]],
		shortName: ['', Validators.required],
		frequency: [1, [Validators.required, Validators.min(1), Validators.pattern('^[0-9]*$')]],
		color: ['', Validators.required],
	});
	subscription = this.subjectForm.valueChanges.subscribe(() => this.formChange());
	
	constructor(public settings: SettingsService, private formBuilder: FormBuilder) { }
	
	selectionChange($event: MatSelectionListChange) {
		this.selectedSubject = $event.options[0].value as Subject;
		this.subjectForm.setValue(this.selectedSubject);
	}
	
	formChange() {
		for (let [key, control] of Object.entries(this.subjectForm.controls)) {
			if (control.valid) {
				(this.selectedSubject as any)[key] = control.value;
			}
			else {
				control.markAsTouched();
			}
		}
	}
}
