import { type DateTime } from 'luxon';

interface CustomMatchers<T, R = unknown> {
	toBePending: T extends Promise<unknown> ? () => R : never
	toBeOrderedSet: T extends Set<unknown> ? (...expected: unknown[]) => R : never
	toHaveSameDay: T extends DateTime ? (expected: DateTime) => R : never
}

declare module 'vitest' {
	// eslint-disable-next-line @typescript-eslint/no-empty-object-type
	interface Matchers<T> extends CustomMatchers<T> {}
}
