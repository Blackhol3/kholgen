import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ConnectionDialogComponent } from './connection-dialog.component';

import { MatDialogRef } from '@angular/material/dialog';

describe('ConnectionDialogComponent', () => {
	let component: ConnectionDialogComponent;
	let fixture: ComponentFixture<ConnectionDialogComponent>;

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [ConnectionDialogComponent],
			providers: [{
				provide: MatDialogRef,
				useValue: jasmine.createSpyObj<MatDialogRef<ConnectionDialogComponent>>,
			}]
		}).compileComponents();

		fixture = TestBed.createComponent(ConnectionDialogComponent);
		component = fixture.componentInstance;
		fixture.detectChanges();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
