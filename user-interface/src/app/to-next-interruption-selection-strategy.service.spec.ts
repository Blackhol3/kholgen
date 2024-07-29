import { TestBed } from '@angular/core/testing';
import { DateRange } from '@angular/material/datepicker';

import { castDraft, type Draft } from 'immer';
import { DateTime, Interval } from 'luxon';

import { Interruption } from './interruption';

import { ToNextInterruptionSelectionStrategyService } from './to-next-interruption-selection-strategy.service';
import { StoreService } from './store.service';

describe('ToNextInterruptionSelectionStrategyService', () => {
	let service: ToNextInterruptionSelectionStrategyService;
	let store: Draft<StoreService>;
	let today: DateTime;

	beforeEach(() => {
		TestBed.configureTestingModule({});
		service = TestBed.inject(ToNextInterruptionSelectionStrategyService);
		store = castDraft(TestBed.inject(StoreService));
		today = DateTime.now();
	});

	it('should works when nothing is selected', () => {
		const range = new DateRange<DateTime>(null, null);

		const preview = service.createPreview(today, range);
		expect(preview.start).toBeNull();
		expect(preview.end).toBeNull();

		const newRange = service.selectionFinished(today, range);
		expect(newRange.start?.hasSame(today, 'day')).toBeTrue();
		expect(newRange.end).toBeNull();
	});

	it('when only a starting date is selected and a date before it is hovered', () => {
		const range = new DateRange<DateTime>(today.plus({days: 5}), null);

		const preview = service.createPreview(today, range);
		expect(preview.start).toBeNull();
		expect(preview.end).toBeNull();

		const newRange = service.selectionFinished(today, range);
		expect(newRange.start?.hasSame(today, 'day')).toBeTrue();
		expect(newRange.end).toBeNull();
	});

	it('when only a starting date is selected and a date after it but before the next interruption is hovered', () => {
		const range = new DateRange<DateTime>(today.minus({days: 5}), null);
		const interruptionStart = today.plus({days: 5});
		store.state.calendar.interruptions = [new Interruption('', Interval.after(interruptionStart, {week: 1}))];

		const preview = service.createPreview(today, range);
		expect(preview.start?.hasSame(range.start!, 'day')).toBeTrue();
		expect(preview.end?.hasSame(today, 'day')).toBeTrue();

		const newRange = service.selectionFinished(today, range);
		expect(newRange).toEqual(preview);
	});

	it('when only a starting date is selected and a date after it and after the next interruption is hovered', () => {
		const range = new DateRange<DateTime>(today.minus({days: 5}), null);
		const interruptionStart = today.minus({days: 2});
		store.state.calendar.interruptions = [new Interruption('', Interval.after(interruptionStart, {week: 1}))];

		const preview = service.createPreview(today, range);
		expect(preview.start?.hasSame(range.start!, 'day')).toBeTrue();
		expect(preview.end?.hasSame(interruptionStart.minus({day: 1}), 'day')).toBeTrue();

		const newRange = service.selectionFinished(today, range);
		expect(newRange).toEqual(preview);
	});

	it('should works when a range is selected', () => {
		const range = new DateRange<DateTime>(today.minus({days: 5}), today.plus({days: 5}))

		const preview = service.createPreview(today, range);
		expect(preview.start).toBeNull();
		expect(preview.end).toBeNull();

		const newRange = service.selectionFinished(today, range);
		expect(newRange.start?.hasSame(today, 'day')).toBeTrue();
		expect(newRange.end).toBeNull();
	});
});
