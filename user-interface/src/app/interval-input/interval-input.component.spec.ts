import { TestbedHarnessEnvironment } from '@angular/cdk/testing/testbed';
import { ComponentFixture, ComponentFixtureAutoDetect, TestBed } from '@angular/core/testing';
import { MatDateRangeInputHarness } from '@angular/material/datepicker/testing';

import { DateTime, Interval, Settings } from 'luxon';

import { IntervalInputComponent } from './interval-input.component';

describe('IntervalInputComponent', () => {
	let component: IntervalInputComponent;
	let fixture: ComponentFixture<IntervalInputComponent>;
	let dateRangeInput: MatDateRangeInputHarness;

	const today = DateTime.local(2024, 12, 1);
	const start = DateTime.local(2024, 12, 25);
	let onChange: jasmine.Spy;

	beforeEach(async () => {
		Settings.now = () => today.valueOf();

		await TestBed.configureTestingModule({
			imports: [IntervalInputComponent],
			providers: [{provide: ComponentFixtureAutoDetect, useValue: true}],
		}).compileComponents();
		fixture = TestBed.createComponent(IntervalInputComponent);
		
		const loader = TestbedHarnessEnvironment.loader(fixture);
		dateRangeInput = await loader.getHarness(MatDateRangeInputHarness);
		component = fixture.componentInstance;

		onChange = jasmine.createSpy();
		component.registerOnChange(onChange);
		component.writeValue(Interval.after(start, {days: 3}).toFullDay());
	});

	afterEach(() => {
		Settings.now = () => new Date().valueOf();
	})

	it('should show the correct date after a model update', async () => {
		expect(await dateRangeInput.getValue()).toBe('25/12/2024 – 27/12/2024');
	});

	it('should update the model correctly after selection in the calendar', async () => {
		await dateRangeInput.openCalendar();

		const calendar = await dateRangeInput.getCalendar();
		await (await calendar.getCells({text: '10'}))[0].select();
		await (await calendar.getCells({text: '20'}))[0].select();

		expect(onChange).toHaveBeenCalledOnceWith(jasmine.any(Interval));

		const calledInterval = onChange.calls.mostRecent().args[0] as Interval;
		expect(calledInterval.start.toMillis()).toBe(DateTime.local(2024, 12, 10).toMillis());
		expect(calledInterval.end  .toMillis()).toBe(DateTime.local(2024, 12, 21).toMillis());
	});

	it('should allow to be disabled', async () => {
		component.setDisabledState(true);

		await dateRangeInput.openCalendar();
		expect(await dateRangeInput.isCalendarOpen()).toBeFalse();
	})
});
