import { immerable } from 'immer';

import type { HumanJsonable, SolverJsonable } from './json';

export class Objective implements HumanJsonable, SolverJsonable {
	[immerable] = true;
	
	protected value: number | undefined;
	
	constructor(
		readonly name: string,
		readonly readableName: string,
		protected valueToText: (value: number) => string,
	) {}
	
	setValue(value: number) {
		this.value = value;
	}
	
	getValueText(): string {
		return this.value === undefined ? '–' : this.valueToText(this.value);
	}
	
	toHumanJson() {
		return this.name;
	}

	toSolverJson(): unknown {
		return this.name;
	}
}
