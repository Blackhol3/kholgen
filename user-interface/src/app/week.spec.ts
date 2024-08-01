import { DateTime } from 'luxon';

import { Week } from './week';

describe('Week', () => {
	it('should check if it is a working week', () => {
		const week1 = new Week(1, 42, DateTime.now());
		const week2 = new Week(2, null, DateTime.now());

		expect(week1.isWorking()).toBeTrue();
		expect(week2.isWorking()).toBeFalse();
	});
});
