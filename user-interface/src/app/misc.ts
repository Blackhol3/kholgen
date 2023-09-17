import { AbstractControl, ValidationErrors } from '@angular/forms';

export function notUniqueValidator<Type>(control: AbstractControl, property: keyof Type, formObject: Type, objects: readonly Type[]): ValidationErrors | null {
	for (let object of objects) {
		if (object !== formObject && object[property] === control.value.trim()) {
			return {notUnique: {object: object, property: property}};
		}
	}
	
	return null;
}

export function trimValidator(control: AbstractControl): null {
	if (typeof control.value !== 'string') {
		return null;
	}
	
	const trimmedValue = control.value.trim();
	if (control.value !== trimmedValue) {
		control.setValue(trimmedValue, {emitEvent: false, onlySelf: true, emitModelToViewChange: false});
	}

	return null;
}

export function setErrors(parentControl: AbstractControl, key: string, newError: {[key: string]: any}) {
	const control = parentControl.get(key)!;
	let errors: ValidationErrors | null = control.errors ?? {};
	
	for (let key in newError) {
		if (newError[key] === undefined) {
			delete errors[key];
		}
		else {
			errors[key] = newError[key];
		}
	}
	
	if (Object.values(errors).length === 0) {
		errors = null;
	}
	
	control.setErrors(errors);
}

export function equalIterables(x: Iterable<any>, y: Iterable<any>): boolean {
	const arrayX = [...x];
	const arrayY = [...y];

	return arrayX.length === arrayY.length && arrayX.every(value => arrayY.includes(value));
}

/** @link https://stackoverflow.com/questions/60141960/typescript-key-value-relation-preserving-object-entries-type/60142095#60142095 */
export type Entries<T> = {
    [K in keyof T]: [K, T[K]]
}[keyof T][];
