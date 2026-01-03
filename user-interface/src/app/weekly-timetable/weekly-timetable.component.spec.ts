import { ComponentFixture, TestBed } from '@angular/core/testing';

import { WeeklyTimetableComponent } from './weekly-timetable.component';

describe('WeeklyTimetableComponent', () => {
	let component: WeeklyTimetableComponent;
	let fixture: ComponentFixture<WeeklyTimetableComponent>;

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [WeeklyTimetableComponent]
		}).compileComponents();

		fixture = TestBed.createComponent(WeeklyTimetableComponent);
		component = fixture.componentInstance;
		await fixture.whenStable();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
