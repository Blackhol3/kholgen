import { Objective } from './objective';
import { ObjectiveComputation } from './objective-computation';

describe('Objective', () => {
	it('should get the text value', () => {
		const objective = new Objective('name', 'readable name', value => `text: ${value}`);
		const objectiveComputation1 = new ObjectiveComputation(objective);
		const objectiveComputation2 = new ObjectiveComputation(objective, 42);

		expect(objectiveComputation1.getValueText()).toBe('–');
		expect(objectiveComputation2.getValueText()).toBe('text: 42');
	});
});
