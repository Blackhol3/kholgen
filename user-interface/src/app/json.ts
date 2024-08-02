import { State } from './state';

export interface HumanJsonable {
	toHumanJson(state: State): unknown;
}

export interface SolverJsonable {
	toSolverJson(): unknown;
}

function isHumanJsonable(value: unknown): value is HumanJsonable {
	return value instanceof Object && 'toHumanJson' in value && typeof value.toHumanJson === 'function';
}

function isSolverJsonable(value: unknown): value is SolverJsonable {
	return value instanceof Object && 'toSolverJson' in value && typeof value.toSolverJson === 'function';
}

export function toHumanString(state: State, space?: string | number) {
	return JSON.stringify(
		state,
		(_: string, value: unknown) => isHumanJsonable(value) ? value.toHumanJson(state) : value,
		space,
	);
}

export function toSolverJson(value: unknown): unknown {
	return JSON.parse(
		JSON.stringify(
			value,
			(_: string, value: unknown) => isSolverJsonable(value) ? value.toSolverJson() : value
		)
	);
}

export type HumanJson<T> =
	T extends HumanJsonable ? HumanJson<ReturnType<T['toHumanJson']>> :
	T extends object | unknown[] ? {[K in keyof T]: HumanJson<T[K]>} :
	T
;