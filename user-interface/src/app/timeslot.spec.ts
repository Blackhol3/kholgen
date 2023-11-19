import { Day, Timeslot } from './timeslot';

describe('Timeslot', () => {
	it('should create an instance', () => {
		expect(new Timeslot(Day.Monday, 13)).toBeTruthy();
	});
});
