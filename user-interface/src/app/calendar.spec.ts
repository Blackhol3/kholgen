import { DateTime, Interval, Settings } from 'luxon';

import { Calendar } from './calendar';
import { Interruption } from './interruption';

describe('Calendar', () => {
	it('should create an instance', () => {
		Settings.now = () => +new Date(2024, 11, 25); // Wednesday
		const calendar = new Calendar();

		expect(calendar.interval.start.toMillis()).toBe(+new Date(2024, 11, 30)); // Monday
		expect(calendar.interval.length('days')).toBeCloseTo(7*10 - 2);
	});

	it('should create weeks', () => {
		//    xx-- ✗
		// --xxxxx ✓
		// xx----- ✓
		// -----xx ✓
		// xxxxxxx ✗
		// xxxxx-- ✗
		// ------- ✓

		const firstMonday = DateTime.now().startOf('week');
		const calendar = new Calendar(
			'',
			Interval.fromDateTimes(
				firstMonday.plus({days: 3}),
				firstMonday.plus({weeks: 6}).endOf('week'),
			),
			10,
			[
				new Interruption('', Interval.after(firstMonday.plus({weeks: 1, days: 2}), {weeks: 1})),
			],
			[
				Interval.after(firstMonday.plus({weeks: 3, days: 5}), {weeks: 2}),
			],
			[
				firstMonday.plus({days: 3}),
				firstMonday.plus({days: 4})
			],
		);
		
		const weeks = calendar.weeks;
		expect(weeks.map(w => w.id)).toEqual([10, 11, 12, 13]);

		expect(weeks[0].start.hasSame(firstMonday.plus({weeks: 1}), 'day')).toBeTrue();
		expect(weeks[1].start.hasSame(firstMonday.plus({weeks: 2}), 'day')).toBeTrue();
		expect(weeks[2].start.hasSame(firstMonday.plus({weeks: 3}), 'day')).toBeTrue();
		expect(weeks[3].start.hasSame(firstMonday.plus({weeks: 6}), 'day')).toBeTrue();
	});
});
