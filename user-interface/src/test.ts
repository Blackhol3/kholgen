// This file is required by karma.conf.js and loads recursively all the .spec and framework files

import './luxon';

import { NgModule, provideZonelessChangeDetection } from '@angular/core';
import { getTestBed } from '@angular/core/testing';
import { BrowserTestingModule, platformBrowserTesting } from '@angular/platform-browser/testing';

import { provideLuxonDateAdapter, MAT_LUXON_DATE_ADAPTER_OPTIONS } from '@angular/material-luxon-adapter'; 
import { MAT_DATE_LOCALE } from '@angular/material/core'; 
import { MAT_DIALOG_DEFAULT_OPTIONS } from '@angular/material/dialog'; 

import { enableMapSet, enablePatches } from 'immer';
import { DateTime } from 'luxon';

@NgModule({
	providers: [provideZonelessChangeDetection()],
})
class ZonelessChangeDetectionModule {}

enableMapSet();
enablePatches();

getTestBed().initTestEnvironment(
	[BrowserTestingModule, ZonelessChangeDetectionModule],
	platformBrowserTesting([
		{provide: MAT_LUXON_DATE_ADAPTER_OPTIONS, useValue: {firstDayOfWeek: 1}},
		{provide: MAT_DATE_LOCALE, useValue: 'fr'},
		{provide: MAT_DIALOG_DEFAULT_OPTIONS, useValue: {enterAnimationDuration: 0, exitAnimationDuration: 0}},
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
