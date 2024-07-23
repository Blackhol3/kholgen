import { DateTime } from 'luxon';

import { Week } from './week';

describe('Week', () => {
	it('should create an instance', () => {
		expect(new Week(42, DateTime.now())).toBeTruthy();
	});
});
