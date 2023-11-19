import { Colle } from './colle';

import { Day, Timeslot } from './timeslot';

describe('Colle', () => {
	it('should create an instance', () => {
		expect(new Colle('teacher', new Timeslot(Day.Monday, 13), 42, 1337)).toBeTruthy();
	});
});
