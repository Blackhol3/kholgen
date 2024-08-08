import { DateTime } from 'luxon';

import { Colle } from './colle';
import { Day, Timeslot } from './timeslot';
import { State } from './state';
import { Week } from './week';

function specificDay(day: DateTime) {
	return {
		asymmetricMatch: (actual: unknown) => {
			if (!(actual instanceof DateTime)) {
				return false;
			}

			return actual.hasSame(day, 'day');
		}
	};
}

describe('Colle', () => {
	it('should create an instance', () => {
		expect(new Colle('teacher', new Timeslot(Day.Monday, 13), 42, 1337)).toBeTruthy();
	});

	it('should check if it is during a working day', () => {
		const colle1 = new Colle('teacher', new Timeslot(Day.Tuesday, 13), 42, 1337);
		const colle2 = new Colle('teacher', new Timeslot(Day.Friday, 13), 42, 1337);

		const today = DateTime.now().startOf('week');
		const state = new State();
		const spy = spyOn(state.calendar, 'isWorkingDay');
		spyOn(state.calendar, 'getWeeks').and.returnValue([new Week(1337, 9, today)]);

		colle1.isDuringWorkingDay(state);
		expect(state.calendar.isWorkingDay).toHaveBeenCalledOnceWith(specificDay(today.plus({day: 1})));

		spy.calls.reset();
		colle2.isDuringWorkingDay(state);
		expect(state.calendar.isWorkingDay).toHaveBeenCalledOnceWith(specificDay(today.plus({day: 4})));
	});
});
