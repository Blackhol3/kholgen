import { Day, Timeslot } from './timeslot';

describe('Timeslot', () => {
	it('should create an instance', () => {
		expect(new Timeslot(Day.Monday, 13)).toBeTruthy();
	});

	it('should check equality', () => {
		const timeslot = new Timeslot(Day.Wednesday, 13);
		expect(timeslot.isEqual(new Timeslot(Day.Monday, 13))).toBeFalse();
		expect(timeslot.isEqual(new Timeslot(Day.Wednesday, 8))).toBeFalse();
		expect(timeslot.isEqual(new Timeslot(Day.Wednesday, 13))).toBeTrue();
	});

	it('should compare', () => {
		const timeslot = new Timeslot(Day.Wednesday, 13);
		expect(timeslot.compare(new Timeslot(Day.Monday, 15))).toBeGreaterThan(0);
		expect(timeslot.compare(new Timeslot(Day.Wednesday, 8))).toBeGreaterThan(0);
		expect(timeslot.compare(new Timeslot(Day.Wednesday, 13))).toBe(0);
		expect(timeslot.compare(new Timeslot(Day.Wednesday, 18))).toBeLessThan(0);
		expect(timeslot.compare(new Timeslot(Day.Friday, 8))).toBeLessThan(0);
	});
});
