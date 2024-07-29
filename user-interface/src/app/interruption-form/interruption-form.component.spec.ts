import { ComponentFixture, TestBed } from '@angular/core/testing';

import { DateTime, Interval } from 'luxon';

import { InterruptionFormComponent } from './interruption-form.component';

import { Interruption } from '../interruption';

describe('HolidayFormComponent', () => {
	let component: InterruptionFormComponent;
	let fixture: ComponentFixture<InterruptionFormComponent>;
	const interruption = new Interruption('name', Interval.after(DateTime.now(), {week: 1}));

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [InterruptionFormComponent],
		}).compileComponents();

		fixture = TestBed.createComponent(InterruptionFormComponent);
		component = fixture.componentInstance;
		component.interruption = interruption;
		fixture.detectChanges();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
