import { Component, inject } from '@angular/core';

import { MatButtonModule } from '@angular/material/button';
import { MAT_DIALOG_DATA, MatDialogRef, MatDialogModule } from '@angular/material/dialog';
import { MatIconModule } from '@angular/material/icon';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

export type DialogData =
	| { type: 'connection' }
	| { type: 'invalid-json', message: string }
;

@Component({
	selector: 'app-connection-dialog',
	templateUrl: './dialog.component.html',
	styleUrls: ['./dialog.component.scss'],
	standalone: true,
	imports: [
		MatButtonModule,
		MatDialogModule,
		MatIconModule,
		MatProgressSpinnerModule
	],
})
export class DialogComponent {
	readonly data = inject<DialogData>(MAT_DIALOG_DATA);
	protected readonly dialogRef = inject(MatDialogRef<this, boolean>);

	constructor() {
		if (this.data.type === 'invalid-json') {
			this.data.message = this.data.message.replaceAll(/'([^' ]*)'/g, '<code>$1</code>');

			const jsonParseHeader = 'JSON.parse: ';
			if (this.data.message.startsWith(jsonParseHeader)) {
				this.data.message = this.data.message.slice(jsonParseHeader.length);
				this.data.message = `${this.data.message[0].toLocaleUpperCase()}${this.data.message.slice(1)}.`;
			}
		}

		this.dialogRef.id = this.data.type;
	}
}
