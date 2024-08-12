import { TestKey } from '@angular/cdk/testing';
import { TestbedHarnessEnvironment } from '@angular/cdk/testing/testbed';
import { ComponentFixture, ComponentFixtureAutoDetect, TestBed } from '@angular/core/testing';
import { MatChipGridHarness, MatChipInputHarness } from '@angular/material/chips/testing';

import { UniqueIntegersChipInputComponent } from './unique-integers-chip-input.component';

describe('UniqueIntegersChipInputComponent', () => {
	let component: UniqueIntegersChipInputComponent;
	let fixture: ComponentFixture<UniqueIntegersChipInputComponent>;
	let onChange: jasmine.Spy;

	let grid: MatChipGridHarness;
	let input: MatChipInputHarness;

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [UniqueIntegersChipInputComponent],
			providers: [{provide: ComponentFixtureAutoDetect, useValue: true}],
		}).compileComponents();
		fixture = TestBed.createComponent(UniqueIntegersChipInputComponent);

		const loader = TestbedHarnessEnvironment.loader(fixture);
		grid = await loader.getHarness(MatChipGridHarness);
		input = await loader.getHarness(MatChipInputHarness);
		component = fixture.componentInstance;

		onChange = jasmine.createSpy();
		component.registerOnChange(onChange);
		component.writeValue(new Set([1, 3, 5]));
	});

	it('should show the correct chips after a model update', async () => {
		const rows = await grid.getRows();

		expect(rows).toHaveSize(3);
		expect(await rows[0].getText()).toBe('1');
		expect(await rows[1].getText()).toBe('3');
		expect(await rows[2].getText()).toBe('5');
		expect(await input.getValue()).toBe('');
	});

	it('should update the model correctly when removing a chip', async () => {
		const rows = await grid.getRows();

		await (await rows[1].getRemoveButton()).click();
		expect(onChange).toHaveBeenCalledOnceWith(new Set([1, 5]));
	});

	it('should not update the model when writing an invalid input', async () => {
		await input.setValue('7x');
		await input.sendSeparatorKey(TestKey.ENTER);

		expect(onChange).not.toHaveBeenCalled();
		expect(await input.getValue()).toBe('7x');
	});

	it('should not update the model when writing a single new chip already existing', async () => {
		await input.setValue('3');
		await input.sendSeparatorKey(TestKey.ENTER);

		expect(onChange).not.toHaveBeenCalled();
		expect(await input.getValue()).toBe('');
	});

	it('should update the model correctly when writing a single new chip', async () => {
		await input.setValue('7');
		await input.sendSeparatorKey(TestKey.ENTER);

		expect(onChange).toHaveBeenCalledOnceWith(new Set([1, 3, 5, 7]));
	});

	it('should update the model correctly when writing several new chips', async () => {
		await input.setValue('4-7');
		await input.sendSeparatorKey(TestKey.ENTER);

		expect(onChange).toHaveBeenCalledOnceWith(new Set([1, 3, 4, 5, 6, 7]));
	});
});
