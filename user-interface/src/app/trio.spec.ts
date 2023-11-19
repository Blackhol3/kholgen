import { Trio } from './trio';

describe('Trio', () => {
	it('should create an instance', () => {
		expect(new Trio(42, ['test1', 'test2'])).toBeTruthy();
	});
});
