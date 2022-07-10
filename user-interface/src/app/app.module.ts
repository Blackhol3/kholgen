import { NgModule } from '@angular/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { BrowserModule } from '@angular/platform-browser';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { RouterModule } from '@angular/router';

import { MatButtonModule } from '@angular/material/button';
import { MatDialogModule } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatListModule } from '@angular/material/list';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';
import { MatSelectModule } from '@angular/material/select';
import { MatSidenavModule } from '@angular/material/sidenav';
import { MatSnackBarModule } from '@angular/material/snack-bar';
import { MatTableModule } from '@angular/material/table';
import { MatToolbarModule } from '@angular/material/toolbar';

import { AppComponent } from './app.component';
import { ColloscopeComponent } from './colloscope/colloscope.component';
import { ComputationPageComponent } from './computation-page/computation-page.component';
import { ConnectionDialogComponent } from './connection-dialog/connection-dialog.component';
import { SubjectFormComponent } from './subject-form/subject-form.component';
import { SubjectsPageComponent } from './subjects-page/subjects-page.component';
import { TeacherFormComponent } from './teacher-form/teacher-form.component';
import { TeachersPageComponent } from './teachers-page/teachers-page.component';
import { WeeklyTimetableComponent } from './weekly-timetable/weekly-timetable.component';

@NgModule({
	declarations: [
		AppComponent,
  		ColloscopeComponent,
		ComputationPageComponent,
    	ConnectionDialogComponent,
		SubjectFormComponent,
		SubjectsPageComponent,
		TeacherFormComponent,
		TeachersPageComponent,
		WeeklyTimetableComponent,
	],
	imports: [
		BrowserModule,
		BrowserAnimationsModule,
		FormsModule,
		ReactiveFormsModule,
		RouterModule.forRoot([
			{path: 'subjects', component: SubjectsPageComponent},
			{path: 'teachers', component: TeachersPageComponent},
			{path: 'computation', component: ComputationPageComponent},
			{path: '**', redirectTo: '/subjects'},
		]),
		
		MatButtonModule,
		MatDialogModule,
		MatFormFieldModule,
		MatIconModule,
		MatInputModule,
		MatListModule,
		MatProgressSpinnerModule,
		MatSelectModule,
		MatSidenavModule,
		MatSnackBarModule,
		MatTableModule,
		MatToolbarModule,
	],
	providers: [],
	bootstrap: [AppComponent]
})
export class AppModule { }
