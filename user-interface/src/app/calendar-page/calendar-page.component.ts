import { ChangeDetectorRef, ChangeDetectionStrategy, Component, type OnInit, type OnDestroy } from '@angular/core';
import { FormsModule, NonNullableFormBuilder, ReactiveFormsModule, Validators } from '@angular/forms';

import { MatButtonModule } from '@angular/material/button';
import { MatDatepickerModule } from '@angular/material/datepicker';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatListModule } from '@angular/material/list';
import { MatSelectModule } from '@angular/material/select';
import { MatTooltipModule } from '@angular/material/tooltip'; 

import { castDraft } from 'immer';
import { Subscription } from 'rxjs';
import { DateTime, Interval } from 'luxon';

import { listAnimation, slideAnimation } from '../animations';
import { Interruption } from '../interruption';
import { entries } from '../misc';

import { CalendarService } from '../calendar.service';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

import { CopyDataDirective } from '../copy-data.directive';
import { InterruptionFormComponent } from '../interruption-form/interruption-form.component';
import { IntervalInputComponent } from '../interval-input/interval-input.component';

@Component({
	selector: 'app-calendar-page',
	templateUrl: './calendar-page.component.html',
	styleUrls: ['./calendar-page.component.scss'],
	animations: [
		listAnimation,
		slideAnimation,
	],
	changeDetection: ChangeDetectionStrategy.OnPush,
	standalone: true,
	imports: [
		FormsModule,
		ReactiveFormsModule,

		MatButtonModule,
		MatDatepickerModule,
		MatFormFieldModule,
		MatIconModule,
		MatInputModule,
		MatListModule,
		MatSelectModule,
		MatTooltipModule,

		CopyDataDirective,
		InterruptionFormComponent,
		IntervalInputComponent,
	],
})
export class CalendarPageComponent implements OnInit, OnDestroy {
	form = this.formBuilder.group({
		academie: [null as string | null, [Validators.required]],
		interval: [Interval.fromDateTimes(DateTime.now(), DateTime.now()), [Validators.required]],
		firstWeekNumber: [1, [Validators.required, Validators.min(0), Validators.pattern(/^-?[0-9]*$/)]],
	});
	selectedInterruptionIds: string[] = [];
	storeSubscription: Subscription | undefined;

	academies: string[] | undefined;

	constructor(
		public store: StoreService,
		private undoStack: UndoStackService,
		private formBuilder: NonNullableFormBuilder,
		private calendarService: CalendarService,
		private changeDetectorRef: ChangeDetectorRef,
	) {
		this.form.valueChanges.subscribe(() => void this.formChange());
	}
	
	async ngOnInit() {
		this.academies = await this.calendarService.getAcademies();
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.updateForm());
		this.updateForm();
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}
	
	updateForm() {
		this.form.setValue({
			academie: this.store.state.calendar.academie,
			interval: this.store.state.calendar.interval,
			firstWeekNumber: this.store.state.calendar.firstWeekNumber,
		}, {emitEvent: false});
	}
	
	async formChange() {
		for (const [key, control] of entries(this.form.controls)) {
			if (!control.valid) {
				control.markAsTouched();
				continue;
			}

			if (key === 'academie' && this.store.state.calendar.academie !== control.value) {
				const schoolHolidays = control.value === null ? [] : await this.calendarService.getSchoolHolidays(control.value);
				const publicHolidays = control.value === null ? [] : await this.calendarService.getPublicHolidays(control.value);

				this.undoStack.do(state => {
					state.calendar.academie = control.value;
					state.calendar.schoolHolidays = schoolHolidays;
					state.calendar.publicHolidays = publicHolidays;
					state.calendar.weeks = state.calendar.createWeeks();
				});

				this.changeDetectorRef.markForCheck();
			}

			if (key === 'interval' && !this.store.state.calendar.interval.equals(control.value)) {
				this.undoStack.do(state => {
					state.calendar.interval = control.value;
					state.calendar.weeks = state.calendar.createWeeks();
				});
			}

			if (key === 'firstWeekNumber' && this.store.state.calendar.firstWeekNumber !== control.value) {
				this.undoStack.do(state => {
					state.calendar.firstWeekNumber = control.value;
					state.calendar.weeks = state.calendar.createWeeks();
				});
			}
		}
	}

	dateFilter = (date: DateTime | null) => {
		return date === null || this.store.state.calendar.isWorkingDay(date);
	}

	addNewInterruption() {
		let name = '';
		for (let i = 1; name = `Interruption ${i}`, this.store.state.calendar.interruptions.some(interruption => interruption.name === name); ++i) {
			// Empty
		}

		const interruption = new Interruption(name, Interval.after(this.store.state.calendar.interval.start, {day: 1}));
		this.undoStack.do(state => {
			state.calendar.interruptions.push(castDraft(interruption));
			state.calendar.weeks = state.calendar.createWeeks();
		});
		this.selectedInterruptionIds = [interruption.id];
	}
	
	deleteInterruption() {
		const interruption = this.store.state.calendar.findInterruptionId(this.selectedInterruptionIds[0])!;
		const index = this.store.state.calendar.interruptions.indexOf(interruption);
		
		this.undoStack.do(state => {
			state.calendar.interruptions.splice(index, 1);
			state.calendar.weeks = state.calendar.createWeeks();
		});
		this.selectedInterruptionIds = this.store.state.calendar.interruptions.length > 0 ? [this.store.state.calendar.interruptions[Math.max(0, index - 1)].id] : [];
	}

	getSortedInterruptions() {
		return this.store.state.calendar.interruptions.toSorted((a, b) => a.interval.start.toMillis() - b.interval.start.toMillis());
	}

	onPaste($event: ClipboardEvent) {
		const jsonString = $event.clipboardData?.getData('application/json-interruption') ?? '';
		if (jsonString === '') {
			return;
		}
		
		const jsonInterruption = JSON.parse(jsonString) as ReturnType<Interruption['toHumanJsonObject']>;
		const interruption = Interruption.fromJsonObject(jsonInterruption);
		this.undoStack.do(state => {
			state.calendar.interruptions.push(interruption);
			state.calendar.weeks = state.calendar.createWeeks();
		});
		this.selectedInterruptionIds = [interruption.id];
	}
}
