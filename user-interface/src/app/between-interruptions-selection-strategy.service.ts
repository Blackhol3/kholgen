import { Injectable, inject } from '@angular/core';
import { DateRange, type MatDateRangeSelectionStrategy } from '@angular/material/datepicker';

import { DateTime } from 'luxon';

import { type Interruption } from './interruption';
import { StoreService } from './store.service';

@Injectable({
	providedIn: 'root',
})
export class BetweenInterruptionsSelectionStrategyService implements MatDateRangeSelectionStrategy<DateTime> {
	protected readonly store = inject(StoreService);

	ignoredInterruptions: Interruption[] = [];

	createPreview(date: DateTime | null, currentRange: DateRange<DateTime>) {
		if (this.isRangeIncomplete(date, currentRange)) {
			return new DateRange<DateTime>(null, null);
		}

		return this.clamp(currentRange.start!, date!);
	}

	selectionFinished(date: DateTime | null, currentRange: DateRange<DateTime>) {
		if (this.isRangeIncomplete(date, currentRange)) {
			return new DateRange<DateTime>(date, null);
		}

		return this.clamp(currentRange.start!, date!);
	}

	createDrag(dragOrigin: DateTime, originalRange: DateRange<DateTime>, newDate: DateTime) {
		if (originalRange.start === null || originalRange.end === null || dragOrigin.hasSame(newDate, 'day')) {
			return null;
		}

		if (dragOrigin.hasSame(originalRange.end, 'day') && newDate >= originalRange.start) {
			return this.clamp(originalRange.start, newDate);
		}

		if (dragOrigin.hasSame(originalRange.start, 'day') && newDate <= originalRange.end) {
			return this.clamp(newDate, originalRange.end, originalRange.end);
		}

		const difference = newDate.diff(dragOrigin);
		const start = originalRange.start.plus(difference);
		const end = originalRange.end.plus(difference);
		const range = this.clamp(start, end, originalRange.start);
		return range.start === null || range.end === null ? null : range;
	}

	protected isRangeIncomplete(date: DateTime | null, currentRange: DateRange<DateTime>) {
		return date === null || currentRange.start === null || currentRange.end !== null || date < currentRange.start;
	}

	protected clamp(currentStart: DateTime, currentEnd: DateTime, validDate: DateTime = currentStart) {
		const interruptions = this.store.state.calendar.interruptions.filter(x => !this.ignoredInterruptions.includes(x));
		const interruptionEnds   = interruptions.map(x => x.interval.end                  ).filter(x => x <= validDate);
		const interruptionStarts = interruptions.map(x => x.interval.start.minus({day: 1})).filter(x => x >= validDate);

		const start = DateTime.max(currentStart, ...interruptionEnds);
		const end   = DateTime.min(currentEnd,   ...interruptionStarts);

		return start <= end ? new DateRange(start, end) : new DateRange<DateTime>(null, null) ;
	}
}
