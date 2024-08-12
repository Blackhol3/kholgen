import { DateTime, Interval, Settings } from 'luxon';

import { Calendar } from './calendar';
import { Interruption } from './interruption';

import { type CalendarService } from './calendar.service';

function withNow<R>(newNow: DateTime, f: () => R) {
	const oldNow = Settings.now;
	Settings.now = () => +newNow;

	try { return f(); }
	finally { Settings.now = oldNow; }
}

describe('Calendar', () => {
	it('should create an instance', () => {
		const calendar = withNow(
			DateTime.local(2024, 12, 25), // Wednesday
			() => new Calendar(),
		);

		expect(calendar.interval.start).toEqual(DateTime.local(2024, 12, 30)); // Monday
		expect(calendar.interval.length('days')).toBeCloseTo(7*10 - 2);
	});

	it('should update weeks', async () => {
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
			).toFullDay(),
			101,
			[
				new Interruption('', Interval.after(firstMonday.plus({weeks: 3, days: 5}), {weeks: 2}).toFullDay(), false, true),
				new Interruption('', Interval.after(firstMonday.plus({weeks: 7         }), {weeks: 1}).toFullDay(), true, false),
			],
		);
		//
		const calendarService = jasmine.createSpyObj<CalendarService>('calendarService', ['getSchoolHolidays', 'getPublicHolidays']);
		calendarService.getSchoolHolidays.and.resolveTo([
			Interval.after(firstMonday.plus({weeks: 1, days: 2}), {weeks: 1}).toFullDay(),
		]);
		calendarService.getPublicHolidays.and.resolveTo([
			firstMonday.plus({days: 3}),
			firstMonday.plus({days: 4}),
		]);

		await calendar.updateWeeksAndHolidays(calendarService);
		
		const expectedData = [
			{number: 101,  weeks: 1},
			{number: 102,  weeks: 2},
			{number: 103,  weeks: 3},
			{number: null, weeks: 4},
			{number: null, weeks: 5},
			{number: 104,  weeks: 6},
			{number: 106,  weeks: 8},
		];

		for (const [i, week] of calendar.getWeeks().entries()) {
			expect(week.id).toBe(i);
			expect(week.number).toBe(expectedData[i].number);
			expect(week.start).toHaveSameDay(firstMonday.plus({weeks: expectedData[i].weeks}));
		}
	});
});
