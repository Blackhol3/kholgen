import { ComponentFixture, TestBed } from '@angular/core/testing';

import { DialogComponent } from './dialog.component';

import { MAT_DIALOG_DATA, MatDialogRef } from '@angular/material/dialog';

describe('DialogComponent', () => {
	let component: DialogComponent;
	let fixture: ComponentFixture<DialogComponent>;

	beforeEach(async () => {
		await TestBed.configureTestingModule({
			imports: [DialogComponent],
			providers: [{
				provide: MatDialogRef,
				useValue: jasmine.createSpyObj<MatDialogRef<DialogComponent>>,
			}, {
				provide: MAT_DIALOG_DATA,
				useValue: {},
			}]
		}).compileComponents();

		fixture = TestBed.createComponent(DialogComponent);
		component = fixture.componentInstance;
		fixture.detectChanges();
	});

	it('should create', () => {
		expect(component).toBeTruthy();
	});
});
