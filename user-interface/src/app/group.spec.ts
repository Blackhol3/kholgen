import { Group } from './group';

describe('Group', () => {
	it('should create an instance', () => {
		const createGroup = () => new Group('name', [], new Set());
		const group1 = createGroup();
		const group2 = createGroup();

		expect(group1).toBeTruthy();
		expect(group2).toBeTruthy();
		expect(group1.id !== group2.id).toBeTrue();
	});
});
