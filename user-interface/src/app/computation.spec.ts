import { Computation } from './computation';
import { Group } from './group';
import { Trio } from './trio';

describe('Computation', () => {
	it('should create trios', () => {
		const groups = [
			new Group('A', [], new Set([1, 2, 3])),
			new Group('B', [], new Set([2, 5])),
			new Group('C', [], new Set([1, 2, 9])),
		];
		const groupIndexesToId = (...indexes: number[]) => indexes.map(i => groups[i].id);

		const computation = new Computation(groups);
		expect(computation.trios).toEqual([
			new Trio(1, groupIndexesToId(0, 2)),
			new Trio(2, groupIndexesToId(0, 1, 2)),
			new Trio(3, groupIndexesToId(0)),
			new Trio(5, groupIndexesToId(1)),
			new Trio(9, groupIndexesToId(2)),
		])
	});
});
