import { Component, inject } from '@angular/core';

import { MatButtonModule } from '@angular/material/button';
import { MAT_DIALOG_DATA, MatDialogRef, MatDialogModule } from '@angular/material/dialog';
import { MatIconModule } from '@angular/material/icon';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

export type DialogData =
	| { type: 'connection' }
	| { type: 'error', message: string }
;

@Component({
	selector: 'app-connection-dialog',
	templateUrl: './connection-dialog.component.html',
	styleUrls: ['./connection-dialog.component.scss'],
	standalone: true,
	imports: [
		MatButtonModule,
		MatDialogModule,
		MatIconModule,
		MatProgressSpinnerModule
	],
})
export class ConnectionDialogComponent {
	readonly data = inject<DialogData>(MAT_DIALOG_DATA);
	protected readonly dialogRef = inject(MatDialogRef<this, boolean>);

	constructor() {
		if (this.data.type === 'connection') {
			this.dialogRef.disableClose = true;
		}

		this.dialogRef.id = this.data.type;
	}
}
