import { Objective } from './objective';

describe('Objective', () => {
	it('should create an instance', () => {
		const objective = new Objective('name', 'readable name', value => `text: ${value}`);
		expect(objective).toBeTruthy();
	});
});
