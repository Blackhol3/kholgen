import { immerable } from 'immer';

import type { HumanJsonable, SolverJsonable } from './json';

export class Objective implements HumanJsonable, SolverJsonable {
	[immerable] = true;
	
	constructor(
		readonly name: string,
		readonly readableName: string,
		readonly valueToText: (value: number) => string,
	) {}
	
	toHumanJson() {
		return this.name;
	}

	toSolverJson(): unknown {
		return this.name;
	}
}
