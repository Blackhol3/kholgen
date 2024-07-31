import { ChangeDetectorRef, ChangeDetectionStrategy, Component, Input, effect, model } from '@angular/core';
import { type ControlValueAccessor, FormsModule, NG_VALUE_ACCESSOR } from '@angular/forms';

import { MatDatepickerModule, type MatDateRangeInput } from '@angular/material/datepicker';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';

import { DateTime, Interval } from 'luxon';

import { getFirstValidDate } from '../calendar';

/** @link https://blog.angular-university.io/angular-custom-form-controls/ **/
@Component({
	selector: 'app-interval-input',
	templateUrl: './interval-input.component.html',
	styleUrls: ['./interval-input.component.scss'],
	providers: [{
		provide: NG_VALUE_ACCESSOR,
		multi: true,
		useExisting: IntervalInputComponent,
	}],
	changeDetection: ChangeDetectionStrategy.OnPush,
	standalone: true,
	imports: [
		FormsModule,

		MatDatepickerModule,
		MatFormFieldModule,
		MatInputModule,
	],
})
export class IntervalInputComponent implements ControlValueAccessor {
	@Input() dateFilter: MatDateRangeInput<DateTime>['dateFilter'] = () => true;

	@Input() label?: string;
	@Input() startPlaceholder?: string;
	@Input() endPlaceholder?: string;

	start = model<DateTime | null>(null);
	end = model<DateTime | null>(null);
	modelToViewUpdate = false;

	firstValidDate = getFirstValidDate();
	disabled = false;

	constructor(private changeDetectorRef: ChangeDetectorRef) {
		effect(() => this.valueChanges(this.start(), this.end()));
	}

	protected onChange = (_: Interval) => {};

	valueChanges(start: DateTime | null, end: DateTime | null) {
		if (this.modelToViewUpdate) {
			this.modelToViewUpdate = false;
		}
		else if (start !== null && end !== null && start < end) {
			this.onChange(Interval.fromDateTimes(start.startOf('day'), end.endOf('day')));
		}
	}
	
	writeValue(interval: Interval) {
		this.modelToViewUpdate = true;
		this.start.set(interval.start);
		this.end.set(interval.end);
		this.changeDetectorRef.markForCheck();
	}
	
	registerOnChange(onChange: typeof this.onChange) {
		this.onChange = onChange;
	}
	
	registerOnTouched() {}
	
	setDisabledState(disabled: boolean) {
		this.disabled = disabled;
		this.changeDetectorRef.markForCheck();
	}
}
