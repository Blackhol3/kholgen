import { ChangeDetectionStrategy, Component, type OnInit, type OnDestroy } from '@angular/core';
import { FormsModule, NonNullableFormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';

import { MatDatepickerModule } from '@angular/material/datepicker'; 
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatInputModule } from '@angular/material/input';
import { MatSelectModule } from '@angular/material/select';

import { Subscription } from 'rxjs';
import { DateTime, type Interval } from 'luxon';

import { type Entries } from '../misc';
import { nbDaysInWeek } from '../timeslot';
import { getFirstValidDate } from '../calendar';
import { CalendarService } from '../calendar.service';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-subjects-page',
	templateUrl: './calendar-page.component.html',
	styleUrls: ['./calendar-page.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
	standalone: true,
	imports: [
		FormsModule,
		ReactiveFormsModule,

		MatDatepickerModule,
		MatFormFieldModule,
		MatInputModule,
		MatSelectModule,
	],
})
export class CalendarPageComponent implements OnInit, OnDestroy {
	form = this.formBuilder.group({
		academie: ['', [Validators.required]],
		start: [DateTime.now(), [Validators.required]],
		end: [DateTime.now(), [Validators.required]],
		firstWeekNumber: [1, [Validators.required, Validators.min(0), Validators.pattern(/^-?[0-9]*$/)]],
	});
	selectedSubjectIds: string[] = [];
	storeSubscription: Subscription | undefined;

	firstValidDate = getFirstValidDate();
	academies: string[] | undefined;
	holidays: Interval[] | undefined;

	constructor(public store: StoreService, private undoStack: UndoStackService, private formBuilder: NonNullableFormBuilder, private calendarService: CalendarService) {
		this.form.valueChanges.subscribe(() => void this.formChange());
	}
	
	async ngOnInit() {
		this.academies = await this.calendarService.getAcademies();
		this.holidays = await this.calendarService.getHolidays(this.store.state.calendar.academie);
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.updateForm());
		this.updateForm();
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}
	
	updateForm() {
		this.form.setValue({
			academie: this.store.state.calendar.academie,
			start: this.store.state.calendar.start,
			end: this.store.state.calendar.end,
			firstWeekNumber: this.store.state.calendar.firstWeekNumber,
		}, {emitEvent: false});
	}
	
	async formChange() {
		for (const [key, control] of Object.entries(this.form.controls) as Entries<typeof this.form.controls>) {
			if (!control.valid) {
				control.markAsTouched();
				continue;
			}

			if (key === 'academie' && this.store.state.calendar.academie !== control.value) {
				this.holidays = await this.calendarService.getHolidays(control.value);
				this.undoStack.do(state => {
					state.calendar.academie = control.value;
					state.weeks = state.calendar.createWeeks(this.holidays!);
				});
			}

			if (key === 'start' && !this.store.state.calendar.start.equals(control.value)) {
				this.undoStack.do(state => {
					state.calendar.start = control.value;
					state.weeks = state.calendar.createWeeks(this.holidays!);
				});
			}

			if (key === 'end' && !this.store.state.calendar.end.equals(control.value)) {
				this.undoStack.do(state => {
					state.calendar.end = control.value;
					state.weeks = state.calendar.createWeeks(this.holidays!);
				});
			}

			if (key === 'firstWeekNumber' && this.store.state.calendar.firstWeekNumber !== control.value) {
				this.undoStack.do(state => {
					state.calendar.firstWeekNumber = control.value;
					state.weeks = state.calendar.createWeeks(this.holidays!);
				});
			}
		}
	}

	dateFilter = (date: DateTime | null) => {
		return date === null || (date.weekday <= nbDaysInWeek && (this.holidays === undefined || !this.holidays.some(interval => interval.contains(date))));
	}
}
