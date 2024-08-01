import { ChangeDetectionStrategy, Component, Input, type OnInit, type OnChanges } from '@angular/core';
import { AbstractControl, FormsModule, NonNullableFormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';

import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';

import { entries, notUniqueValidator, trimValidator } from '../misc';
import { Subject } from '../subject';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-subject-form',
	templateUrl: './subject-form.component.html',
	styleUrls: ['./subject-form.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
	standalone: true,
	imports: [
		FormsModule,
		ReactiveFormsModule,
		
		MatFormFieldModule,
		MatInputModule,
	],
})
export class SubjectFormComponent implements OnInit, OnChanges {
	@Input({required: true}) subject!: Subject;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, trimValidator, (control: AbstractControl<string, string>) => notUniqueValidator(control, 'name', this.subject, this.store.state.subjects)]],
		shortName: ['', [Validators.required, trimValidator, (control: AbstractControl<string, string>) => notUniqueValidator(control, 'shortName', this.subject, this.store.state.subjects)]],
		frequency: [1, [Validators.required, Validators.min(1), Validators.pattern('^-?[0-9]*$')]],
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
		this.form.setValue({
			name: this.subject.name,
			shortName: this.subject.shortName,
			frequency: this.subject.frequency,
			color: this.subject.color,
		}, {emitEvent: false});
	}
	
	formChange() {
		for (const [key, control] of entries(this.form.controls)) {
			if (control.valid && this.subject[key] !== control.value) {
				this.undoStack.do(state => {
					(state.findId('subjects', this.subject.id)![key] as unknown) = control.value;
				});
			}
			else {
				control.markAsTouched();
			}
		}
	}
}
