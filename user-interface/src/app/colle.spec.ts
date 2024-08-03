import { castDraft } from 'immer';
import { DateTime } from 'luxon';

import { Colle } from './colle';
import { Day, Timeslot } from './timeslot';
import { State } from './state';
import { Week } from './week';

describe('Colle', () => {
	it('should create an instance', () => {
		expect(new Colle('teacher', new Timeslot(Day.Monday, 13), 42, 1337)).toBeTruthy();
	});

	it('should check if it is during a working day', () => {
		const colle1 = new Colle('teacher', new Timeslot(Day.Tuesday, 13), 42, 1337);
		const colle2 = new Colle('teacher', new Timeslot(Day.Friday, 13), 42, 1337);

		const today = DateTime.now().startOf('week');
		const state = new State();
		castDraft(state).calendar.weeks = [new Week(1337, 42, today)];
		castDraft(state).calendar.publicHolidays = [today.plus({day: 1})];

		const spy = spyOn(state.calendar, 'isWorkingDay').and.callThrough();
		expect(colle1.isDuringWorkingDay(state)).toBeFalse();
		expect(state.calendar.isWorkingDay).toHaveBeenCalledTimes(1);

		spy.calls.reset();
		expect(colle2.isDuringWorkingDay(state)).toBeTrue();
		expect(state.calendar.isWorkingDay).toHaveBeenCalledTimes(1);
	});
});
