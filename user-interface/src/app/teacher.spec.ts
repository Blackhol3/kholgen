import { Teacher } from './teacher';

describe('Teacher', () => {
	it('should create an instance', () => {
		const createTeacher = () => new Teacher('name', 'subject', [], 2, 1.5);
		const teacher1 = createTeacher();
		const teacher2 = createTeacher();

		expect(teacher1).toBeTruthy();
		expect(teacher2).toBeTruthy();
		expect(teacher1.id).not.toBe(teacher2.id);
	});
});
