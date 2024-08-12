import { FormControl } from '@angular/forms';

import { trimValidator } from './misc';

describe('trimValidator', () => {
	it('should trim value when necessary', () => {
		const control = new FormControl(' untrimmed ', {nonNullable: true});
		spyOn(control, 'setValue').and.callThrough();

		trimValidator(control);
		expect(control.value).toBe('untrimmed');
		expect(control.setValue).toHaveBeenCalledTimes(1);
	});

	it('should not trim value when not necessary', () => {
		const control = new FormControl('trimmed', {nonNullable: true});
		spyOn(control, 'setValue').and.callThrough();

		trimValidator(control);
		expect(control.value).toBe('trimmed');
		expect(control.setValue).not.toHaveBeenCalled();
	});
});
