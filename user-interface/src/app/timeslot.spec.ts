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

	it('should create an object from a string', () => {
		expect(Timeslot.fromString('Lundi 8h')).toEqual(new Timeslot(Day.Monday, 8));
		expect(Timeslot.fromString('Mardi 09h')).toEqual(new Timeslot(Day.Tuesday, 9));
		expect(Timeslot.fromString('mErcRedI 13')).toEqual(new Timeslot(Day.Wednesday, 13));
		expect(Timeslot.fromString('j15')).toEqual(new Timeslot(Day.Thursday, 15));

		expect(() => Timeslot.fromString('X12')).toThrow();
		expect(() => Timeslot.fromString('Lundi x8')).toThrow();
		expect(() => Timeslot.fromString('Lundi 5')).toThrow();
		expect(() => Timeslot.fromString('Lundi 22')).toThrow();
	});
});
