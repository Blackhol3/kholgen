import { ComponentFixture, TestBed } from '@angular/core/testing';
import { provideLuxonDateAdapter, MAT_LUXON_DATE_ADAPTER_OPTIONS } from '@angular/material-luxon-adapter'; 
import { MAT_DATE_LOCALE } from '@angular/material/core'; 

import { CalendarPageComponent } from './calendar-page.component';

describe('CalendarPageComponent', () => {
	let component: CalendarPageComponent;
	let fixture: ComponentFixture<CalendarPageComponent>;

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			providers: [
				{provide: MAT_LUXON_DATE_ADAPTER_OPTIONS, useValue: {firstDayOfWeek: 1}},
				{provide: MAT_DATE_LOCALE, useValue: 'fr'},
				provideLuxonDateAdapter(),
			],
			imports: [CalendarPageComponent],
		}).compileComponents();

		fixture = TestBed.createComponent(CalendarPageComponent);
		component = fixture.componentInstance;
		fixture.detectChanges();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
