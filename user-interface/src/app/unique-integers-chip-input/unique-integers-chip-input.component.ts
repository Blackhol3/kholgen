import { COMMA, ENTER, SPACE } from '@angular/cdk/keycodes';
import { Component, Input } from '@angular/core';
import { ControlValueAccessor, NG_VALUE_ACCESSOR } from '@angular/forms';
import { MatChipInputEvent } from '@angular/material/chips';

/** @link https://blog.angular-university.io/angular-custom-form-controls/ **/
@Component({
	selector: 'app-unique-integers-chip-input',
	templateUrl: './unique-integers-chip-input.component.html',
	styleUrls: ['./unique-integers-chip-input.component.scss'],
	providers: [{
		provide: NG_VALUE_ACCESSOR,
		multi: true,
		useExisting: UniqueIntegersChipInputComponent,
	}],
})
export class UniqueIntegersChipInputComponent implements ControlValueAccessor {
	@Input() label?: string;
	@Input() placeholder?: string;

	integers: Set<number> = new Set();
	disabled = false;
	readonly separatorKeysCodes = [COMMA, ENTER, SPACE] as const;
	
	protected onChange = (_: Set<Number>) => {};
	
	writeValue(integers: Set<number> | null) {
		this.integers = new Set(integers);
	}
	
	registerOnChange(onChange: typeof this.onChange) {
		this.onChange = onChange;
	}
	
	registerOnTouched() {}
	
	setDisabledState(disabled: boolean) {
		this.disabled = disabled;
	}

	/** @todo Show an error on invalid inputs */
	add(event: MatChipInputEvent) {
		const results = event.value.trim().match(/^([0-9]+)(?:-([0-9]+))?$/);
		if (results === null) {
			return;
		}

		let updated = false;
		if (results[2] === undefined) {
			const integer = parseInt(results[1]);
			updated = !this.integers.has(integer);
			this.integers.add(integer);
		}
		else {
			const minInteger = Math.min(parseInt(results[1]), parseInt(results[2]));
			const maxInteger = Math.max(parseInt(results[1]), parseInt(results[2]));

			for (let integer = minInteger; integer <= maxInteger; ++integer) {
				if (!this.integers.has(integer)) {
					updated = true;
				}
				this.integers.add(integer);
			}
		}

		if (updated) {
			const integersArray = [...this.integers];
			integersArray.sort();
			this.integers = new Set(integersArray);
			this.onChange(new Set(this.integers));
		}
		event.chipInput.clear();
	}

	remove(integer: number) {
		this.integers.delete(integer);
		this.onChange(new Set(this.integers));
	}
}
