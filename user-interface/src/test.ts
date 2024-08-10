// This file is required by karma.conf.js and loads recursively all the .spec and framework files

import 'zone.js/testing';
import { getTestBed } from '@angular/core/testing';
import { BrowserDynamicTestingModule, platformBrowserDynamicTesting } from '@angular/platform-browser-dynamic/testing';
import { NoopAnimationsModule } from '@angular/platform-browser/animations';

import { provideLuxonDateAdapter, MAT_LUXON_DATE_ADAPTER_OPTIONS } from '@angular/material-luxon-adapter'; 
import { MAT_DATE_LOCALE } from '@angular/material/core'; 

import { DateTime, Settings } from 'luxon';

Settings.throwOnInvalid = true;
declare module 'luxon' {
	interface TSSettings {
		throwOnInvalid: true;
	}
}

getTestBed().initTestEnvironment(
	[BrowserDynamicTestingModule, NoopAnimationsModule],
	platformBrowserDynamicTesting([
		{provide: MAT_LUXON_DATE_ADAPTER_OPTIONS, useValue: {firstDayOfWeek: 1}},
		{provide: MAT_DATE_LOCALE, useValue: 'fr'},
		provideLuxonDateAdapter(),
	]),
);

declare global {
	// eslint-disable-next-line @typescript-eslint/no-namespace
	namespace jasmine {
		// eslint-disable-next-line @typescript-eslint/no-unused-vars
		interface Matchers<T> {
			toHaveSameDay(expected: DateTime): boolean;
		}
	}
}

beforeAll(() => {
	jasmine.addMatchers({
		toHaveSameDay: util => ({
			compare: (actual: unknown, expected: DateTime) => {
				if (actual instanceof DateTime === false) {
					return {pass: false, message: `Expected ${util.pp(actual)} to be an instance of DateTime.`};
				}

				return actual.hasSame(expected, 'day')
					? {pass: true}
					: {pass: false, message: `Expected ${actual.toISODate()} to be ${expected.toISODate()}.`}
				;
			},
		}),
	});
})
