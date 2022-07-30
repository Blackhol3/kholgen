import { immerable } from 'immer';

export class Objective {
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
}
