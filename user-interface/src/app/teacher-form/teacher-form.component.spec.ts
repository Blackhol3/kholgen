import { ComponentFixture, TestBed } from '@angular/core/testing';

import { TeacherFormComponent } from './teacher-form.component';

import { Teacher } from '../teacher';

describe('TeacherFormComponent', () => {
	let component: TeacherFormComponent;
	let fixture: ComponentFixture<TeacherFormComponent>;
	const teacher = new Teacher('name', 'subject', [], 2);

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [TeacherFormComponent]
		}).compileComponents();

		fixture = TestBed.createComponent(TeacherFormComponent);
		component = fixture.componentInstance;
		component.teacher = teacher;
		fixture.detectChanges();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
