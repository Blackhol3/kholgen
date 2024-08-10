import { TestBed } from '@angular/core/testing';

import { SpreadsheetExporterService } from './spreadsheet-exporter.service';

describe('SpreadsheetExporterService', () => {
	let service: SpreadsheetExporterService;

	beforeEach(() => {
		TestBed.configureTestingModule({});
		service = TestBed.inject(SpreadsheetExporterService);
	});

	it('should be created', () => {
		expect(service).toBeTruthy();
	});
});
