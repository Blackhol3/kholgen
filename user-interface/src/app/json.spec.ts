import { toHumanString } from './json';
import { type State } from './state';

describe('toHumanString', () => {
	it('should works', () => {
		const child = {
			nonIncluded: 42,
			toHumanJson: (state: State & {parent?: unknown}) => ({
				child1: 1,
				child2: 2,
				parent: state.parent,
			}),
		};
		
		const state = {
			nonIncluded: 42,
			parent: 'parent',
			toHumanJson: () => ({
				child: child,
				value: 1337,
			}),
		} as unknown as State;

		vi.spyOn(child, 'toHumanJson');
		vi.spyOn(state, 'toHumanJson');

		const result = toHumanString(state);
		
		expect(state.toHumanJson).toHaveBeenCalledTimes(1);
		expect(child.toHumanJson).toHaveBeenCalledExactlyOnceWith(state);

		expect(typeof result).toBe('string');
		expect(JSON.parse(result)).toEqual({
			child: {
				child1: 1,
				child2: 2,
				parent: 'parent',
			},
			value: 1337,
		})
	});
});
