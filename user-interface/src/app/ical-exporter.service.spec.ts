import { TestBed } from '@angular/core/testing';

import { ICalExporterService } from './ical-exporter.service';

describe('SpreadsheetExporterService', () => {
	let service: ICalExporterService;

	beforeEach(() => {
		TestBed.configureTestingModule({});
		service = TestBed.inject(ICalExporterService);
	});

	it('should be created', () => {
		expect(service).toBeTruthy();
	});
});
