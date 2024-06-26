import { ComponentFixture, TestBed } from '@angular/core/testing';

import { SubjectFormComponent } from './subject-form.component';

import { Subject } from '../subject';

describe('SubjectFormComponent', () => {
	let component: SubjectFormComponent;
	let fixture: ComponentFixture<SubjectFormComponent>;
	const subject = new Subject('name', 'short name', 2, '#ff0000');

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [SubjectFormComponent]
		}).compileComponents();

		fixture = TestBed.createComponent(SubjectFormComponent);
		component = fixture.componentInstance;
		component.subject = subject;
		fixture.detectChanges();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
