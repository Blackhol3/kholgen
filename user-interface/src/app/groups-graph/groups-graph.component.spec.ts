import { ComponentFixture, TestBed } from '@angular/core/testing';

import { GroupsGraphComponent } from './groups-graph.component';

describe('GroupsGraphComponent', () => {
	let component: GroupsGraphComponent;
	let fixture: ComponentFixture<GroupsGraphComponent>;

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [GroupsGraphComponent]
		}).compileComponents();

		fixture = TestBed.createComponent(GroupsGraphComponent);
		component = fixture.componentInstance;
		await fixture.whenStable();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
