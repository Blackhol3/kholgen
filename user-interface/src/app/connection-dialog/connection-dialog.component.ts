import { Component } from '@angular/core';
import { MatDialogRef, MatDialogModule } from '@angular/material/dialog';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

@Component({
	selector: 'app-connection-dialog',
	templateUrl: './connection-dialog.component.html',
	styleUrls: ['./connection-dialog.component.scss'],
	standalone: true,
	imports: [MatDialogModule, MatProgressSpinnerModule],
})
export class ConnectionDialogComponent {
	constructor(dialogRef: MatDialogRef<ConnectionDialogComponent>) {
		dialogRef.disableClose = true;
	}
}
