import { Component, HostListener } from '@angular/core';
import { ControlValueAccessor, NG_VALUE_ACCESSOR } from '@angular/forms';

import { MatTableModule } from '@angular/material/table';

import { dayNames, firstHour, lastHour, Day, Timeslot } from '../timeslot';

/** @link https://blog.angular-university.io/angular-custom-form-controls/ **/
@Component({
	selector: 'app-weekly-timetable',
	templateUrl: './weekly-timetable.component.html',
	styleUrls: ['./weekly-timetable.component.scss'],
	providers: [{
		provide: NG_VALUE_ACCESSOR,
		multi: true,
		useExisting: WeeklyTimetableComponent,
	}],
	standalone: true,
	imports: [MatTableModule],
})
export class WeeklyTimetableComponent implements ControlValueAccessor {
	timeslots: Timeslot[] = [];
	editable = true;
	protected currentEdit: 'select' | 'unselect' | null = null;
	
	dayNames = dayNames;
	hours: readonly number[] = Array(lastHour - firstHour + 1).fill(null).map((_, i) => firstHour + i);
	
	protected onChange = (_: Timeslot[]) => {};
	
	isTimeslotSelected(day: Day, hour: number): boolean {
		let timeslot = new Timeslot(day, hour);
		return this.timeslots.some(t => timeslot.isEqual(t));
	}
	
	onMousedown($event: MouseEvent) {
		if (this.isTargetEditable($event)) {
			const timeslot = this.getTimeslotFromTarget($event.target);
			this.currentEdit = this.timeslots.some(t => timeslot.isEqual(t)) ? 'unselect' : 'select';
			this.edit(timeslot);
			$event.preventDefault();
		}
	}
	
	onMouseover($event: MouseEvent) {
		if (this.isTargetEditable($event)) {
			this.edit(this.getTimeslotFromTarget($event.target));
		}
	}
	
	@HostListener('window:mouseup')
	onMouseup() {
		if (this.currentEdit !== null) {
			this.onChange(this.timeslots.slice());
		}
		this.currentEdit = null;
	}
	
	writeValue(timeslots: Timeslot[] | null) {
		this.timeslots = timeslots === null ? [] : timeslots.slice();
	}
	
	registerOnChange(onChange: typeof this.onChange) {
		this.onChange = onChange;
	}
	
	registerOnTouched() {}
	
	setDisabledState(disabled: boolean) {
		this.editable = !disabled;
	}
	
	protected isTargetEditable($event: MouseEvent): $event is MouseEvent & {target: HTMLElement} {
		return $event.target instanceof HTMLElement && $event.target.classList.contains('editable-cell');
	}
	
	protected getTimeslotFromTarget(target: HTMLElement) {
		return new Timeslot(+target.dataset['day']!, +target.dataset['hour']!);
	}
	
	protected edit(timeslot: Timeslot) {
		const index = this.timeslots.findIndex(t => timeslot.isEqual(t));
		if (this.currentEdit === 'select' && index === -1) {
			this.timeslots.push(timeslot);
		}
		else if (this.currentEdit === 'unselect' && index !== -1) {
			this.timeslots.splice(index, 1);
		}
	}
}
