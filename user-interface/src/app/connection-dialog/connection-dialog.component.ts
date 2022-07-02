import { Component } from '@angular/core';
import { MatDialogRef } from '@angular/material/dialog';

@Component({
	selector: 'app-connection-dialog',
	templateUrl: './connection-dialog.component.html',
	styleUrls: ['./connection-dialog.component.scss']
})
export class ConnectionDialogComponent {
	constructor(dialogRef: MatDialogRef<ConnectionDialogComponent>) {
		dialogRef.disableClose = true;
	}
}
