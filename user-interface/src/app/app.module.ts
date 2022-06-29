import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';

import { MatTableModule } from '@angular/material/table';
import { MatToolbarModule } from '@angular/material/toolbar';
import { ColloscopeComponent } from './colloscope/colloscope.component';

@NgModule({
	declarations: [
		AppComponent,
  		ColloscopeComponent
	],
	imports: [
		BrowserModule,
		BrowserAnimationsModule,
		MatTableModule,
		MatToolbarModule,
	],
	providers: [],
	bootstrap: [AppComponent]
})
export class AppModule { }
