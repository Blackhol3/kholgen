import { COMMA, ENTER, SPACE } from '@angular/cdk/keycodes';
import { ChangeDetectionStrategy, Component, input, signal } from '@angular/core';
import { type ControlValueAccessor, NG_VALUE_ACCESSOR } from '@angular/forms';

import { type MatChipInputEvent, MatChipsModule } from '@angular/material/chips';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';

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
	changeDetection: ChangeDetectionStrategy.OnPush,
	imports: [
		MatChipsModule,
		MatFormFieldModule,
		MatIconModule,
		MatInputModule,
	],
})
export class UniqueIntegersChipInputComponent implements ControlValueAccessor {
	label = input<string>();
	placeholder = input<string>();

	protected integers = signal(new Set<number>());
	protected disabled = signal(false);
	protected readonly separatorKeysCodes = [COMMA, ENTER, SPACE] as const;
	
	protected onChange = (_: Set<number>) => {};
	
	writeValue(integers: Set<number> | null) {
		this.integers.set(new Set(integers));
	}
	
	registerOnChange(onChange: typeof this.onChange) {
		this.onChange = onChange;
	}
	
	registerOnTouched() {}
	
	setDisabledState(disabled: boolean) {
		this.disabled.set(disabled);
	}

	/** @todo Show an error on invalid inputs */
	protected add(event: MatChipInputEvent) {
		const results = event.value.trim().match(/^([0-9]+)(?:-([0-9]+))?$/);
		if (results === null) {
			return;
		}

		const integers = new Set(this.integers());
		if (results[2] === undefined) {
			const integer = parseInt(results[1]);
			integers.add(integer);
		}
		else {
			const minInteger = Math.min(parseInt(results[1]), parseInt(results[2]));
			const maxInteger = Math.max(parseInt(results[1]), parseInt(results[2]));

			for (let integer = minInteger; integer <= maxInteger; ++integer) {
				integers.add(integer);
			}
		}

		if (!this.integers().isSupersetOf(integers)) {
			const integersArray = [...integers];
			integersArray.sort((a, b) => a - b);
			this.integers.set(new Set(integersArray));
			this.onChange(new Set(this.integers()));
		}
		event.chipInput.clear();
	}

	protected remove(integer: number) {
		this.integers.update(set => {
			set.delete(integer);
			return set;
		});
		this.onChange(new Set(this.integers()));
	}
}
