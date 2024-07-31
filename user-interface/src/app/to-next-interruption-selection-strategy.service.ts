import { Injectable } from '@angular/core';
import { DateRange, type MatDateRangeSelectionStrategy } from '@angular/material/datepicker';

import { DateTime } from 'luxon';

import { type Interruption } from './interruption';
import { StoreService } from './store.service';

/** @todo Implement `createDrag` */
@Injectable({
	providedIn: 'root',
})
export class ToNextInterruptionSelectionStrategyService implements MatDateRangeSelectionStrategy<DateTime> {
	ignoredInterruptions: Interruption[] = [];

	constructor(private store: StoreService) {}

	createPreview(date: DateTime | null, currentRange: DateRange<DateTime>) {
		if (this.isRangeIncomplete(date, currentRange)) {
			return new DateRange<DateTime>(null, null);
		}

		return this.toNextInterruption(date!, currentRange.start!);
	}

	selectionFinished(date: DateTime | null, currentRange: DateRange<DateTime>) {
		if (this.isRangeIncomplete(date, currentRange)) {
			return new DateRange<DateTime>(date, null);
		}

		return this.toNextInterruption(date!, currentRange.start!);
	}

	protected isRangeIncomplete(date: DateTime | null, currentRange: DateRange<DateTime>) {
		return date === null || currentRange.start === null || currentRange.end !== null || date < currentRange.start;
	}

	protected toNextInterruption(date: DateTime, currentStart: DateTime) {
		const startingDates = this.store.state.calendar.interruptions
			.filter(x => !this.ignoredInterruptions.includes(x))
			.map(x => x.interval.start.minus({day: 1}))
			.filter(x => x >= currentStart)
		;
		const endDate = DateTime.min(date, ...startingDates);

		return new DateRange<DateTime>(currentStart, endDate);
	}
}