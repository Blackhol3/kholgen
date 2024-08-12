import { Interval, Settings } from 'luxon';

Settings.throwOnInvalid = true;
declare module 'luxon' {
	interface TSSettings {
		throwOnInvalid: true;
	}
}

declare const FullDayInterval: unique symbol;
type FullDayInterval = Interval & {[FullDayInterval]: true};
type FullDayIntervalAlias = FullDayInterval;

declare module 'luxon/src/interval' {
	export interface Interval {
		toFullDay(): FullDayInterval;
		toFullDayISO(): string;
	}

	export type FullDayInterval = FullDayIntervalAlias;

	// eslint-disable-next-line @typescript-eslint/no-namespace
	namespace Interval {
		export function fromFullDayISO(text: string, errorMessage?: (message: string) => string): FullDayInterval;
	}
};

Interval.prototype.toFullDay = function (this: Interval) {
	return Interval.fromDateTimes(
		this.start.startOf('day'),
		this.end.equals(this.end.startOf('day')) ? this.end : this.end.endOf('day').plus({millisecond: 1}),
	) as FullDayInterval;
}

Interval.prototype.toFullDayISO = function (this: Interval) {
	const fullDayInterval = this.toFullDay();
	return Interval.fromDateTimes(fullDayInterval.start, fullDayInterval.end.minus({millisecond: 1})).toISODate();
}

Interval.fromFullDayISO = function (text, errorMessage = x => x) {
	try {
		const interval = Interval.fromISO(text);
		return text.includes('/P')
			? interval.toFullDay()
			: Interval.fromDateTimes(interval.start, interval.end.endOf('day').plus({millisecond: 1})).toFullDay()
		;
	}
	catch (exception) {
		if (exception instanceof Error) {
			throw new SyntaxError(errorMessage(exception.message.replace(/^Invalid[^:]*: /, '')), {cause: exception});
		}

		throw exception;
	}
}
