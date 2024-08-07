import { Component } from '@angular/core';
import { type CdkDragDrop, CdkDropList, CdkDrag, moveItemInArray } from '@angular/cdk/drag-drop';
import { type ControlValueAccessor, NG_VALUE_ACCESSOR } from '@angular/forms';

import { MatListModule } from '@angular/material/list';

import { Objective } from '../objective';

/** @link https://blog.angular-university.io/angular-custom-form-controls/ **/
@Component({
	selector: 'app-objectives',
	templateUrl: './objectives.component.html',
	styleUrls: ['./objectives.component.scss'],
	providers: [{
		provide: NG_VALUE_ACCESSOR,
		multi: true,
		useExisting: ObjectivesComponent,
	}],
	standalone: true,
	imports: [
		CdkDrag,
		CdkDropList,
		MatListModule,
	],
})
export class ObjectivesComponent implements ControlValueAccessor {
	objectives: Objective[] = [];
	
	protected onChange = (_: Objective[]) => {};
	
	onDrop($event: CdkDragDrop<unknown[]>) {
		if ($event.previousIndex !== $event.currentIndex) {
			moveItemInArray(this.objectives, $event.previousIndex, $event.currentIndex);
			this.onChange(this.objectives.slice());
		}
	}
	
	writeValue(objectives: Objective[] | null) {
		this.objectives = objectives === null ? [] : objectives.slice();
	}
	
	registerOnChange(onChange: typeof this.onChange) {
		this.onChange = onChange;
	}
	
	registerOnTouched() {}
}
