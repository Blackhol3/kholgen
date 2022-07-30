import { ChangeDetectionStrategy, Component, Input, OnInit, OnChanges } from '@angular/core';
import { AbstractControl, NonNullableFormBuilder, ValidationErrors, Validators } from '@angular/forms';

import { Subject } from '../subject';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

/** @link https://stackoverflow.com/questions/60141960/typescript-key-value-relation-preserving-object-entries-type/60142095#60142095 */
type Entries<T> = {
    [K in keyof T]: [K, T[K]]
}[keyof T][];

@Component({
	selector: 'app-subject-form',
	templateUrl: './subject-form.component.html',
	styleUrls: ['./subject-form.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
})
export class SubjectFormComponent implements OnInit, OnChanges {
	@Input() subject: Subject | undefined;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, (control: AbstractControl) => this.notUniqueValidator(control, 'name')]],
		shortName: ['', [Validators.required, (control: AbstractControl) => this.notUniqueValidator(control, 'shortName')]],
		frequency: [1, [Validators.required, Validators.min(1), Validators.pattern('^[0-9]*$')]],
		color: ['', Validators.required],
	});
	
	constructor(private store: StoreService, private undoStack: UndoStackService, private formBuilder: NonNullableFormBuilder ) {
		this.form.valueChanges.subscribe(() => this.formChange());
	}

	ngOnInit() {
		this.updateForm();
	}
	
	ngOnChanges() {
		this.updateForm();
	}
	
	updateForm() {
		if (this.subject === undefined) {
			throw 'Subject cannot be undefined.';
		}
		
		this.form.setValue({
			name: this.subject.name,
			shortName: this.subject.shortName,
			frequency: this.subject.frequency,
			color: this.subject.color,
		});
	}
	
	formChange() {
		if (this.subject === undefined) {
			return;
		}
		
		for (let [key, control] of Object.entries(this.form.controls) as Entries<typeof this.form.controls>) {
			if (control.valid && this.subject[key] !== this.getControlValue(key)) {
				this.undoStack.do(state => {
					(state.findId('subjects', this.subject!.id) as any)[key] = this.getControlValue(key);
				});
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
		for (let subject of this.store.state.subjects) {
			if (subject !== this.subject && subject[property] === control.value.trim()) {
				return {notUnique: {subject: subject, property: property}};
			}
		}
		
		return null;
	}
}
