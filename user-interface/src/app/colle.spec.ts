import { DateTime } from 'luxon';

import { Calendar } from './calendar';
import { Colle } from './colle';
import { Day, Timeslot } from './timeslot';
import { Week } from './week';

describe('Colle', () => {
	it('should create an instance', () => {
		expect(new Colle('teacher', new Timeslot(Day.Monday, 13), 42, 1337)).toBeTruthy();
	});

	it('should get the start date', () => {
		const colle1 = new Colle('teacher', new Timeslot(Day.Tuesday, 13), 42, 1337);
		const colle2 = new Colle('teacher', new Timeslot(Day.Wednesday, 13), 42, 1337);
		const colle3 = new Colle('teacher', new Timeslot(Day.Friday, 13), 42, 35);

		const today = DateTime.now().startOf('week');
		const calendar = jasmine.createSpyObj<Calendar>('calendar', ['getWeeks']);
		calendar.getWeeks.and.returnValue([
			new Week(1337, 9, today),
			new Week(35, 9, today.plus({week: 1})),
		]);

		expect(colle1.getStartDate(calendar)).toHaveSameDay(today.plus({days: 1}));
		expect(colle2.getStartDate(calendar)).toHaveSameDay(today.plus({days: 2}));
		expect(colle3.getStartDate(calendar)).toHaveSameDay(today.plus({week: 1, days: 4}));
	});
});
