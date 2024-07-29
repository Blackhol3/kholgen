// This file is required by karma.conf.js and loads recursively all the .spec and framework files

import 'zone.js/testing';
import { getTestBed } from '@angular/core/testing';
import { BrowserDynamicTestingModule, platformBrowserDynamicTesting } from '@angular/platform-browser-dynamic/testing';
import { NoopAnimationsModule } from '@angular/platform-browser/animations';

import { provideLuxonDateAdapter, MAT_LUXON_DATE_ADAPTER_OPTIONS } from '@angular/material-luxon-adapter'; 
import { MAT_DATE_LOCALE } from '@angular/material/core'; 

import { Settings } from 'luxon';

Settings.throwOnInvalid = true;
declare module 'luxon' {
	interface TSSettings {
		throwOnInvalid: true;
	}
}

// First, initialize the Angular testing environment.
getTestBed().initTestEnvironment(
	[BrowserDynamicTestingModule, NoopAnimationsModule],
	platformBrowserDynamicTesting([
		{provide: MAT_LUXON_DATE_ADAPTER_OPTIONS, useValue: {firstDayOfWeek: 1}},
		{provide: MAT_DATE_LOCALE, useValue: 'fr'},
		provideLuxonDateAdapter(),
	]),
);
