import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppComponent } from './app.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';

import { MatButtonModule } from '@angular/material/button';
import { MatDialogModule } from '@angular/material/dialog';
import { MatIconModule } from '@angular/material/icon';
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
		MatButtonModule,
		MatDialogModule,
		MatIconModule,
		MatProgressSpinnerModule,
		MatTableModule,
		MatToolbarModule,
	],
	providers: [],
	bootstrap: [AppComponent]
})
export class AppModule { }
