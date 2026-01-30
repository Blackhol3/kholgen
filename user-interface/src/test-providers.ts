import './luxon';

import { provideZonelessChangeDetection } from '@angular/core';

import { provideLuxonDateAdapter, MAT_LUXON_DATE_ADAPTER_OPTIONS } from '@angular/material-luxon-adapter'; 
import { MAT_DATE_LOCALE } from '@angular/material/core'; 
import { MAT_DIALOG_DEFAULT_OPTIONS } from '@angular/material/dialog'; 

export default [
	provideZonelessChangeDetection(),
	{provide: MAT_LUXON_DATE_ADAPTER_OPTIONS, useValue: {firstDayOfWeek: 1}},
	{provide: MAT_DATE_LOCALE, useValue: 'fr'},
	{provide: MAT_DIALOG_DEFAULT_OPTIONS, useValue: {enterAnimationDuration: 0, exitAnimationDuration: 0}},
	provideLuxonDateAdapter(),
];
