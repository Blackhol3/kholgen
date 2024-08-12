import { ChangeDetectorRef, ChangeDetectionStrategy, Component, Input, effect, inject, model } from '@angular/core';
import { type ControlValueAccessor, FormsModule, NG_VALUE_ACCESSOR } from '@angular/forms';

import { MatDatepickerModule, type MatDateRangeInput } from '@angular/material/datepicker';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';

import { DateTime, type FullDayInterval, Interval } from 'luxon';

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
	protected readonly changeDetectorRef = inject(ChangeDetectorRef);

	@Input() dateFilter: MatDateRangeInput<DateTime>['dateFilter'] = () => true;

	@Input() label?: string;
	@Input() startPlaceholder?: string;
	@Input() endPlaceholder?: string;

	protected start = model<DateTime | null>(null);
	protected end = model<DateTime | null>(null);
	protected modelToViewUpdate = false;

	protected firstValidDate = getFirstValidDate();
	protected disabled = false;

	protected onChange = (_: FullDayInterval) => {};

	constructor() {
		effect(() => this.valueChanges(this.start(), this.end()));
	}

	writeValue(interval: FullDayInterval) {
		this.modelToViewUpdate = true;
		this.start.set(interval.start);
		this.end.set(interval.end.minus({day: 1}));
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

	protected valueChanges(start: DateTime | null, end: DateTime | null) {
		if (this.modelToViewUpdate) {
			this.modelToViewUpdate = false;
		}
		else if (start !== null && end !== null && start < end) {
			this.onChange(Interval.fromDateTimes(start, end.endOf('day')).toFullDay());
		}
	}
}
