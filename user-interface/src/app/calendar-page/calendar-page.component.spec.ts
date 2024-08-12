import { ComponentFixture, TestBed } from '@angular/core/testing';

import { castDraft } from 'immer';
import { Interval } from 'luxon';

import { Interruption } from '../interruption';
import { StoreService } from '../store.service';

import { CalendarPageComponent } from './calendar-page.component';

describe('CalendarPageComponent', () => {
	let component: CalendarPageComponent;
	let fixture: ComponentFixture<CalendarPageComponent>;
	let store: StoreService;

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [CalendarPageComponent],
		}).compileComponents();

		fixture = TestBed.createComponent(CalendarPageComponent);
		store = TestBed.inject(StoreService);
		component = fixture.componentInstance;
		fixture.detectChanges();
	});

	describe('should add a new interruption', () => {
		it('when there are none yet', () => {
			component.addNewInterruption();
			expect(store.state.calendar.interruptions).toHaveSize(1);
			expect(store.state.calendar.interruptions[0].name).toBe('Interruption 1');
			expect(store.state.calendar.interruptions[0].interval.start).toHaveSameDay(store.state.calendar.interval.start.plus({day: 1}));
			expect(store.state.calendar.interruptions[0].interval.toDuration().as('day')).toBe(1);
		});

		it('when there are already some', () => {
			const calendarStart = store.state.calendar.interval.start;
			castDraft(store).state.calendar.interruptions.push(
				new Interruption('Interruption 1', Interval.after(calendarStart.plus({weeks: 2}), {days: 2}).toFullDay()),
				new Interruption('Interruption 3', Interval.after(calendarStart.plus({weeks: 3}), {days: 5}).toFullDay()),
			);

			component.addNewInterruption();
			expect(store.state.calendar.interruptions).toHaveSize(3);
			expect(store.state.calendar.interruptions[2].name).toBe('Interruption 2');
			expect(store.state.calendar.interruptions[2].interval.start).toHaveSameDay(calendarStart.plus({weeks: 4}));
			expect(store.state.calendar.interruptions[2].interval.toDuration().as('day')).toBe(1);
		});
	});
});
