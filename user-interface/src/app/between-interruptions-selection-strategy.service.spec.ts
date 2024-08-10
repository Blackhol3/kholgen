import { TestBed } from '@angular/core/testing';
import { DateRange } from '@angular/material/datepicker';

import { castDraft, type Draft } from 'immer';
import { DateTime, Duration, Interval } from 'luxon';

import { Interruption } from './interruption';

import { BetweenInterruptionsSelectionStrategyService } from './between-interruptions-selection-strategy.service';
import { StoreService } from './store.service';

describe('ToNextInterruptionSelectionStrategyService', () => {
	let service: BetweenInterruptionsSelectionStrategyService;
	let store: Draft<StoreService>;
	let today: DateTime;

	beforeEach(() => {
		TestBed.configureTestingModule({});
		service = TestBed.inject(BetweenInterruptionsSelectionStrategyService);
		store = castDraft(TestBed.inject(StoreService));
		today = DateTime.now();
	});

	it('when nothing is selected', () => {
		const range = new DateRange<DateTime>(null, null);

		const preview = service.createPreview(today, range);
		expect(preview.start).toBeNull();
		expect(preview.end).toBeNull();

		const newRange = service.selectionFinished(today, range);
		expect(newRange.start).toHaveSameDay(today);
		expect(newRange.end).toBeNull();
	});

	it('when only a starting date is selected and a date before it is hovered', () => {
		const range = new DateRange(today.plus({days: 5}), null);

		const preview = service.createPreview(today, range);
		expect(preview.start).toBeNull();
		expect(preview.end).toBeNull();

		const newRange = service.selectionFinished(today, range);
		expect(newRange.start).toHaveSameDay(today);
		expect(newRange.end).toBeNull();
	});

	describe('when only a starting date is selected and a date after it is hovered', () => {
		let range: DateRange<DateTime>;
		beforeEach(() => {
			range = new DateRange(today.minus({days: 5}), null);
		});

		it('but before the next interruption', () => {
			const interruptionStart = today.plus({days: 5});
			store.state.calendar.interruptions = [new Interruption('', Interval.after(interruptionStart, {week: 1}))];

			const preview = service.createPreview(today, range);
			expect(preview.start).toHaveSameDay(range.start!);
			expect(preview.end).toHaveSameDay(today);

			const newRange = service.selectionFinished(today, range);
			expect(newRange).toEqual(preview);
		});

		describe('and after the next interruption', () => {
			let interruptionStart: DateTime;
			beforeEach(() => {
				interruptionStart = today.minus({days: 2});
				store.state.calendar.interruptions = [new Interruption('', Interval.after(interruptionStart, {week: 1}))];
			});
			
			it('nothing being ignored', () => {
				const preview = service.createPreview(today, range);
				expect(preview.start).toHaveSameDay(range.start!);
				expect(preview.end).toHaveSameDay(interruptionStart.minus({day: 1}));
	
				const newRange = service.selectionFinished(today, range);
				expect(newRange).toEqual(preview);
			});

			it('the interruption being ignored', () => {
				service.ignoredInterruptions = store.state.calendar.interruptions;

				const preview = service.createPreview(today, range);
				expect(preview.start).toHaveSameDay(range.start!);
				expect(preview.end).toHaveSameDay(today);

				const newRange = service.selectionFinished(today, range);
				expect(newRange).toEqual(preview);
			});
		});
	});

	describe('when a range is selected', () => {
		let range: DateRange<DateTime>;
		beforeEach(() => {
			range = new DateRange(today.minus({days: 5}), today.plus({days: 5}));
		});

		it('should preview and select', () => {
			const preview = service.createPreview(today, range);
			expect(preview.start).toBeNull();
			expect(preview.end).toBeNull();

			const newRange = service.selectionFinished(today, range);
			expect(newRange.start).toHaveSameDay(today);
			expect(newRange.end).toBeNull();
		});

		it('should drag the start until the previous interruption', () => {
			const interruptionEnd = today.minus({days: 10});
			store.state.calendar.interruptions = [new Interruption('', Interval.before(interruptionEnd, {week: 1}))];

			const afterInterruption = today.minus({days: 7});
			const afterInterruptionDrag = service.createDrag(range.start!, range, afterInterruption);
			expect(afterInterruptionDrag?.start).toHaveSameDay(afterInterruption);
			expect(afterInterruptionDrag?.end).toHaveSameDay(range.end!);

			const beforeInterruption = today.minus({days: 13});
			const beforeInterruptionDrag = service.createDrag(range.start!, range, beforeInterruption);
			expect(beforeInterruptionDrag?.start).toHaveSameDay(interruptionEnd.plus({day: 1}));
			expect(beforeInterruptionDrag?.end).toHaveSameDay(range.end!);
		});

		it('should drag the end until the next interruption', () => {
			const interruptionStart = today.plus({days: 10});
			store.state.calendar.interruptions = [new Interruption('', Interval.after(interruptionStart, {week: 1}))];

			const beforeInterruption = today.plus({days: 7});
			const beforeInterruptionDrag = service.createDrag(range.end!, range, beforeInterruption);
			expect(beforeInterruptionDrag?.start).toHaveSameDay(range.start!);
			expect(beforeInterruptionDrag?.end).toHaveSameDay(beforeInterruption);

			const afterInterruption = today.plus({days: 13});
			const afterInterruptionDrag = service.createDrag(range.end!, range, afterInterruption);
			expect(afterInterruptionDrag?.start).toHaveSameDay(range.start!);
			expect(afterInterruptionDrag?.end).toHaveSameDay(interruptionStart.minus({day: 1}));
		});

		it('should drag the whole range', () => {
			const difference = Duration.fromObject({days: 3});
			const drag = service.createDrag(today, range, today.plus(difference));
			expect(drag?.start).toHaveSameDay(range.start!.plus(difference));
			expect(drag?.end).toHaveSameDay(range.end!.plus(difference));
		})
	});
});
