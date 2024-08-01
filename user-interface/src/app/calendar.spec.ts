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
		// ┌──────────┬────────┐
		// │ Calendar │ Number │
		// ├──────────┼────────┤
		// │    xx--  │        │
		// │ --xxxxx  │   1    │
		// │ xx-----  │   2    │
		// │ -----xx  │   3    │
		// │ xxxxxxx  │   ✗    │
		// │ xxxxx--  │   ✗    │
		// │ -------  │   4    │
		// │ xxxxxxx  │        │
		// │ -------  │   6    │
		// └──────────┴────────┘

		const firstMonday = DateTime.now().startOf('week');
		const calendar = new Calendar(
			'',
			Interval.fromDateTimes(
				firstMonday.plus({days: 3}),
				firstMonday.plus({weeks: 8}).endOf('week'),
			),
			101,
			[
				new Interruption('', Interval.after(firstMonday.plus({weeks: 3, days: 5}), {weeks: 2}), false, true),
				new Interruption('', Interval.after(firstMonday.plus({weeks: 7         }), {weeks: 1}), true, false),
			],
			[
				Interval.after(firstMonday.plus({weeks: 1, days: 2}), {weeks: 1}),
			],
			[
				firstMonday.plus({days: 3}),
				firstMonday.plus({days: 4})
			],
		);
		
		const weeks = calendar.createWeeks();
		const expectedData = [
			{number: 101,  weeks: 1},
			{number: 102,  weeks: 2},
			{number: 103,  weeks: 3},
			{number: null, weeks: 4},
			{number: null, weeks: 5},
			{number: 104,  weeks: 6},
			{number: 106,  weeks: 8},
		];

		for (const [i, week] of weeks.entries()) {
			expect(week.id).toBe(i);
			expect(week.number).toBe(expectedData[i].number);
			expect(week.start.hasSame(firstMonday.plus({weeks: expectedData[i].weeks}), 'day')).toBeTrue();
		}
	});
});
