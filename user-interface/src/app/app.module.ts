import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';

import { MatDialogModule } from '@angular/material/dialog';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatTableModule } from '@angular/material/table';
import { MatToolbarModule } from '@angular/material/toolbar';
import { ColloscopeComponent } from './colloscope/colloscope.component';
import { ConnectionDialogComponent } from './connection-dialog/connection-dialog.component';

@NgModule({
	declarations: [
		AppComponent,
  		ColloscopeComponent,
    	ConnectionDialogComponent,
	],
	imports: [
		BrowserModule,
		BrowserAnimationsModule,
		MatDialogModule,
		MatProgressSpinnerModule,
		MatTableModule,
		MatToolbarModule,
	],
	providers: [],
	bootstrap: [AppComponent]
})
export class AppModule { }
