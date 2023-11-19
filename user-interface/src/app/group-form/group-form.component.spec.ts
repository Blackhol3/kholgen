import { ComponentFixture, TestBed } from '@angular/core/testing';

import { GroupFormComponent } from './group-form.component';

import { Group } from '../group';

describe('GroupFormComponent', () => {
	let component: GroupFormComponent;
	let fixture: ComponentFixture<GroupFormComponent>;
	const group = new Group('name', [], new Set());

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [GroupFormComponent]
		})
		.compileComponents();

		fixture = TestBed.createComponent(GroupFormComponent);
		component = fixture.componentInstance;
		component.group = group;
		fixture.detectChanges();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
