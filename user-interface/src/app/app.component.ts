import { animate, query, style, transition, trigger } from '@angular/animations';
import { Component, ElementRef, HostListener, ViewChild, inject } from '@angular/core';
import { RouterLink, RouterLinkActive, RouterOutlet } from '@angular/router';

import { MatButtonModule } from '@angular/material/button';
import { MatDialog } from '@angular/material/dialog';
import { MatIconModule } from '@angular/material/icon';
import { MatListModule } from '@angular/material/list';
import { MatMenuModule } from '@angular/material/menu';
import { MatSidenavModule } from '@angular/material/sidenav';
import { MatToolbarModule } from '@angular/material/toolbar';

import * as FileSaver from 'file-saver-es';
import { castDraft } from 'immer';

import { toHumanString } from './json';
import { State } from './state';
import { validateHumanJson } from './state.schema';

import { CalendarService } from './calendar.service';
import { ICalExporterService } from './ical-exporter.service';
import { SpreadsheetExporterService } from './spreadsheet-exporter.service';
import { StoreService } from './store.service';
import { UndoStackService } from './undo-stack.service';

import { DialogComponent } from './dialog/dialog.component';

type ExportType = 'json' | 'xlsx' | 'csv' | 'ics';

@Component({
	selector: 'app-root',
	templateUrl: './app.component.html',
	styleUrls: ['./app.component.scss'],
	animations: [
		trigger('routeAnimation', [
			transition('* => *', [
				style({position: 'relative'}),
				query(':enter, :leave', style({position: 'absolute'}), {optional: true}),
				query(':enter', style({left: '-100%'}), {optional: true}),
				query(':leave', [
					animate('150ms', style({opacity: 0})),
					style({left: '-100%'}),
				], {optional: true}),
				query(':enter', [
					style({left: 'initial', opacity: 0}),
					animate('200ms', style({opacity: 1})),
				], {optional: true}),
			]),
		]),
	],
	standalone: true,
	imports: [
		RouterLink,
		RouterLinkActive,
		RouterOutlet,

		MatButtonModule,
		MatIconModule,
		MatListModule,
		MatMenuModule,
		MatSidenavModule,
		MatToolbarModule,
	],
})
export class AppComponent {
	readonly undoStack = inject(UndoStackService);
	protected readonly calendarService = inject(CalendarService);
	protected readonly dialog = inject(MatDialog);
	protected readonly iCalExporter = inject(ICalExporterService);
	protected readonly spreadsheetExporter = inject(SpreadsheetExporterService);
	protected readonly store = inject(StoreService);

	@ViewChild('importFileInput') importFileInput!: ElementRef<HTMLInputElement>;
	
	startImportFile() {
		this.importFileInput.nativeElement.click();
	}
	
	/** @todo Add file drag and drop */
	async importFile() {
		const files = this.importFileInput.nativeElement.files;
		if (files === null || files[0] === undefined) {
			return;
		}
		
		try {
			const json = JSON.parse(await files[0].text()) as unknown;
			if (!validateHumanJson(json)) {
				const error = validateHumanJson.errors![0];
				throw new SyntaxError(`'${error.instancePath}' ${error.message}.`);
			}
			
			const newState = await State.fromHumanJson(json, this.calendarService);
			this.undoStack.do(() => castDraft(newState));
		}
		catch (exception) {
			if (exception instanceof SyntaxError) {
				this.dialog.open(DialogComponent, {data: {type: 'invalid-json', message: exception.message}});
			}
			else {
				throw exception;
			}
		}
	}
	
	/** @todo Add a warning that this does not save the results, only the configuration */
	async exportFile(extension: ExportType) {
		let data: Blob;
		let filename: string;
		switch (extension) {
			case 'json':
				data = new Blob(
					[toHumanString(this.store.state, "\t")],
					{type: 'application/json'},
				);
				filename = 'Colloscope.json';
				break;
			
			case 'xlsx':
				data = await this.spreadsheetExporter.asExcel(this.store.state.computation!);
				filename = 'Colloscope.xlsx';
				break;
			
			case 'csv':
				data = await this.spreadsheetExporter.asCsv(this.store.state.computation!);
				filename = 'Colloscope.csv';
				break;
			
			case 'ics':
				data = await this.iCalExporter.asZip(this.store.state.computation!);
				filename = 'Calendrier.zip';
				break;
		}

		FileSaver.saveAs(data, filename);
	}
	
	@HostListener('window:keydown', ['$event'])
	protected onKeydown($event: KeyboardEvent) {
		if ($event.ctrlKey && $event.key === 'z') {
			this.undoStack.undo();
			$event.preventDefault();
		}
		else if ($event.ctrlKey && $event.key === 'y') {
			this.undoStack.redo();
			$event.preventDefault();
		}
	}
}
