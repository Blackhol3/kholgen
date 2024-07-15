import { ChangeDetectionStrategy, Component, Input, type OnInit, type OnChanges } from '@angular/core';
import { AbstractControl, FormsModule, NonNullableFormBuilder, ReactiveFormsModule, type ValidationErrors, Validators } from '@angular/forms';

import { MatOptionModule } from '@angular/material/core';
import { MatSelectModule } from '@angular/material/select';
import { MatInputModule } from '@angular/material/input';
import { MatFormFieldModule } from '@angular/material/form-field';

import { type Entries, setErrors, trimValidator } from '../misc';
import { Teacher } from '../teacher';
import { Timeslot } from '../timeslot';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

import { WeeklyTimetableComponent } from '../weekly-timetable/weekly-timetable.component';

@Component({
	selector: 'app-teacher-form',
	templateUrl: './teacher-form.component.html',
	styleUrls: ['./teacher-form.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
	standalone: true,
	imports: [
		FormsModule,
		ReactiveFormsModule,

		MatFormFieldModule,
		MatInputModule,
		MatSelectModule,
		MatOptionModule,

		WeeklyTimetableComponent,
	],
})
export class TeacherFormComponent implements OnInit, OnChanges {
	@Input({required: true}) teacher!: Teacher;
	
	form = this.formBuilder.group({
		name: ['', [Validators.required, trimValidator]],
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
		this.form.setValue({
			name: this.teacher.name,
			subjectId: this.teacher.subjectId,
			availableTimeslots: this.teacher.availableTimeslots,
			weeklyAvailabilityFrequency: this.teacher.weeklyAvailabilityFrequency,
		}, {emitEvent: false});
	}
	
	formChange() {
		for (const [key, control] of Object.entries(this.form.controls) as Entries<typeof this.form.controls>) {
			if (control.valid && this.teacher[key] !== control.value) {
				this.undoStack.do(
					state => {
						(state.findId('teachers', this.teacher.id)![key] as unknown) = control.value;
					},
					key !== 'availableTimeslots'
				);
			}
			else {
				control.markAsTouched();
			}
		}
	}
	
	protected notUniqueValidator(control: AbstractControl): ValidationErrors | null {
		if (this.form === undefined) {
			return null;
		}
		
		for (const teacher of this.store.state.teachers) {
			if (teacher !== this.teacher && teacher.subjectId === this.form.controls.subjectId.value && teacher.name === this.form.controls.name.value) {
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
