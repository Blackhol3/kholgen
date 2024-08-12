import { DateTime, Interval } from 'luxon';

import { Interruption } from './interruption';

describe('Interruption', () => {
	it('should create an instance', () => {
		const createHoliday = () => new Interruption('name', Interval.after(DateTime.now(), {week: 1}).toFullDay());
		const interruption1 = createHoliday();
		const interruption2 = createHoliday();

		expect(interruption1).toBeTruthy();
		expect(interruption2).toBeTruthy();
		expect(interruption1.id).not.toBe(interruption2.id);
	});
});
