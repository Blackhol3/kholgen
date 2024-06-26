import { Component, type OnInit, type OnDestroy } from '@angular/core';
import { FormsModule, NonNullableFormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';

import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';

import { Subscription } from 'rxjs';

import { type Entries } from '../misc';
import { Objective } from '../objective';
import { firstHour, lastHour } from '../timeslot';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

import { ObjectivesComponent } from '../objectives/objectives.component';

@Component({
	selector: 'app-options-page',
	templateUrl: './options-page.component.html',
	styleUrls: ['./options-page.component.scss'],
	standalone: true,
	imports: [
		FormsModule,
		ReactiveFormsModule,

		MatFormFieldModule,
		MatInputModule,

		ObjectivesComponent,
	],
})
export class OptionsPageComponent implements OnInit, OnDestroy {
	form = this.formBuilder.group({
		lunchTimeStart: [firstHour, [Validators.required, Validators.min(firstHour), Validators.max(lastHour + 1), Validators.pattern('^-?[0-9]*$')]],
		lunchTimeEnd: [lastHour + 1, [Validators.required, Validators.min(firstHour), Validators.max(lastHour + 1), Validators.pattern('^-?[0-9]*$')]],
		objectives: [[] as readonly Objective[], Validators.required],
	});
	storeSubscription: Subscription | undefined;
	
	firstHour = firstHour;
	lastHour = lastHour;
	
	constructor(public store: StoreService, private undoStack: UndoStackService, private formBuilder: NonNullableFormBuilder) {
		this.form.valueChanges.subscribe(() => this.formChange());
	}
	
	ngOnInit() {
		this.updateForm();
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.updateForm());
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}
	
	updateForm() {
		this.form.setValue({
			lunchTimeStart: this.store.state.lunchTimeRange[0],
			lunchTimeEnd: this.store.state.lunchTimeRange[1],
			objectives: this.store.state.objectives,
		});
	}
	
	formChange() {
		for (const [key, control] of Object.entries(this.form.controls) as Entries<typeof this.form.controls>) {
			if (!control.valid) {
				control.markAsTouched();
				continue;
			}
			
			if (key === 'objectives' && this.store.state.objectives !== control.value) {
				this.undoStack.do(state => { state.objectives = control.value as Objective[]; });
			}
			else if (key === 'lunchTimeStart' && this.store.state.lunchTimeRange[0] !== control.value) {
				this.undoStack.do(state => { state.lunchTimeRange[0] = control.value; });
			}
			else if (key === 'lunchTimeEnd' && this.store.state.lunchTimeRange[1] !== control.value) {
				this.undoStack.do(state => { state.lunchTimeRange[1] = control.value; });
			}
		}
	}
}
