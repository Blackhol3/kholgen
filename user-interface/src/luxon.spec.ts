import { DateTime, Interval } from 'luxon';

describe('toFullDay', () => {
	it('should works', () => {
		const interval1 = Interval.fromDateTimes(DateTime.local(2024, 12, 25, 5), DateTime.local(2024, 12, 28, 5)).toFullDay();
		expect(interval1.start).toEqual(DateTime.local(2024, 12, 25, 0, 0, 0, 0));
		expect(interval1.end).toEqual(DateTime.local(2024, 12, 29, 0, 0, 0, 0));

		const interval2 = Interval.fromDateTimes(DateTime.local(2024, 12, 25, 5), DateTime.local(2024, 12, 29, 0)).toFullDay();
		expect(interval2.start).toEqual(DateTime.local(2024, 12, 25, 0, 0, 0, 0));
		expect(interval2.end).toEqual(DateTime.local(2024, 12, 29, 0, 0, 0, 0));
	});
});

describe('toFullDayISO', () => {
	it('should works', () => {
		const interval1 = Interval.after(DateTime.local(2024, 12, 25), {days: 3});
		expect(interval1.toFullDayISO()).toBe('2024-12-25/2024-12-27');

		const interval2 = Interval.after(DateTime.local(2024, 12, 25, 5), {days: 2.5});
		expect(interval2.toFullDayISO()).toBe('2024-12-25/2024-12-27');
	});
});

describe('fromFullDayISO', () => {
	it('should works with start and end interval (both inclusive)', () => {
		const interval = Interval.fromFullDayISO('2024-12-25/2024-12-28');
		expect(interval.start).toEqual(DateTime.local(2024, 12, 25, 0, 0, 0, 0));
		expect(interval.end).toEqual(DateTime.local(2024, 12, 29, 0, 0, 0, 0));
	});

	it('should works with start and duration interval (half-open)', () => {
		const interval = Interval.fromFullDayISO('2024-12-25/P3D');
		expect(interval.start).toEqual(DateTime.local(2024, 12, 25, 0, 0, 0, 0));
		expect(interval.end).toEqual(DateTime.local(2024, 12, 28, 0, 0, 0, 0));
	});

	it('should throws with an invalid interval', () => {
		expect(() => Interval.fromFullDayISO('2024-99-25/P3D')).toThrowMatching(e => e instanceof SyntaxError);
	});
});
