import { ChangeDetectorRef, ChangeDetectionStrategy, Component, Input } from '@angular/core';
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

	start: DateTime | null = null;
	end: DateTime | null = null;
	
	firstValidDate = getFirstValidDate();
	disabled = false;

	constructor(private changeDetectorRef: ChangeDetectorRef) {}

	protected onChange = (_: Interval) => {};

	valueChanges() {
		if (this.start !== null && this.end !== null && this.start < this.end) {
			this.onChange(Interval.fromDateTimes(this.start.startOf('day'), this.end.endOf('day')));
		}
	}
	
	writeValue(interval: Interval) {
		this.start = interval.start;
		this.end = interval.end;
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
