import { DateTime, Interval } from 'luxon';

import { intervaltoISO, intervalFromISO } from './misc';

describe('intervaltoISO', () => {
	it('should works', () => {
		const interval1 = Interval.after(DateTime.local(2023, 12, 25), {days: 3});
		expect(intervaltoISO(interval1)).toBe('2023-12-25/2023-12-27');

		const interval2 = Interval.after(DateTime.local(2023, 12, 25, 5), {days: 2.5});
		expect(intervaltoISO(interval2)).toBe('2023-12-25/2023-12-27');
	});
});

describe('intervalFromISO', () => {
	it('should works with start and end interval (both inclusive)', () => {
		const interval = intervalFromISO('2023-12-25/2023-12-28', x => x);
		expect(interval.start).toEqual(DateTime.local(2023, 12, 25, 0, 0, 0, 0));
		expect(interval.end).toEqual(DateTime.local(2023, 12, 29, 0, 0, 0, 0));
	});

	it('should works with start and duration interval (half-open)', () => {
		const interval = intervalFromISO('2023-12-25/P3D', x => x);
		expect(interval.start).toEqual(DateTime.local(2023, 12, 25, 0, 0, 0, 0));
		expect(interval.end).toEqual(DateTime.local(2023, 12, 28, 0, 0, 0, 0));
	});

	it('should throws with an invalid interval', () => {
		expect(() => intervalFromISO('2023-15-25/P3D', x => x)).toThrowMatching(e => e instanceof SyntaxError);
	});
});
