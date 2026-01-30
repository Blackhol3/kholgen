import { enableMapSet, enablePatches } from 'immer';
import { DateTime } from 'luxon';

enableMapSet();
enablePatches();

const pendingSymbol = Symbol();
expect.extend({
	async toBePending(actual: Promise<unknown>) {
		const result = await Promise.race([actual, Promise.resolve(pendingSymbol)]);
		return {
			pass: result === pendingSymbol,
			message: () => `The promise is ${this.isNot ? 'still': 'not'} pending.`,
		}
	},
	toBeOrderedSet(actual: Set<unknown>, ...expected: unknown[]) {
		return {
			pass: expected.length === actual.size && [...actual].every((value, index) => value === expected[index]),
			message: () => `Set should${this.isNot ? ' not' : ''} be equal.`,
			actual: actual,
			expected: expected,
		};
	},
	toHaveSameDay(actual: DateTime, expected: DateTime) {
		return {
			pass: actual.hasSame(expected, 'day'),
			message: () => `DateTime should${this.isNot ? ' not' : ''} have the same day.`,
			actual: actual.toISODate(),
			expected: expected.toISODate(),
		};
	},
});
