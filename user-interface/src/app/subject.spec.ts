import { Subject } from './subject';

describe('Subject', () => {
	it('should create an instance', () => {
		const createSubject = () => new Subject('name', 'short name', 2, '#ff0000');
		const subject1 = createSubject();
		const subject2 = createSubject();

		expect(subject1).toBeTruthy();
		expect(subject2).toBeTruthy();
		expect(subject1.id !== subject2.id).toBeTrue();
	});
});
