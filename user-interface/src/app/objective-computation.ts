import { immerable } from 'immer';

import { type Objective } from './objective';

export class ObjectiveComputation {
	[immerable] = true;
	
	constructor(
		readonly objective: Objective,
		readonly value: number | null = null,
	) {}
	
	getValueText(): string {
		return this.value === null ? '–' : this.objective.valueToText(this.value);
	}
}
