import { ChangeDetectionStrategy, Component, signal } from '@angular/core';
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
	changeDetection: ChangeDetectionStrategy.OnPush,
	imports: [
		CdkDrag,
		CdkDropList,
		MatListModule,
	],
})
export class ObjectivesComponent implements ControlValueAccessor {
	protected objectives = signal<readonly Objective[]>([]);
	
	protected onChange = (_: Objective[]) => {};
	
	writeValue(objectives: Objective[] | null) {
		this.objectives.set(objectives === null ? [] : [...objectives]);
	}
	
	registerOnChange(onChange: typeof this.onChange) {
		this.onChange = onChange;
	}
	
	registerOnTouched() {}

	protected onDrop($event: CdkDragDrop<unknown[]>) {
		if ($event.previousIndex !== $event.currentIndex) {
			const objectives = [...this.objectives()];
			moveItemInArray(objectives, $event.previousIndex, $event.currentIndex);
			
			this.objectives.set(objectives);
			this.onChange(objectives);
		}
	}
}
