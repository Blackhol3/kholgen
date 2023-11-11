import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ConnectionDialogComponent } from './connection-dialog.component';

describe('ConnectionDialogComponent', () => {
	let component: ConnectionDialogComponent;
	let fixture: ComponentFixture<ConnectionDialogComponent>;

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [ConnectionDialogComponent]
		})
		.compileComponents();

		fixture = TestBed.createComponent(ConnectionDialogComponent);
		component = fixture.componentInstance;
		fixture.detectChanges();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
