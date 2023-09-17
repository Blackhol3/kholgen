import { NgModule } from '@angular/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { BrowserModule } from '@angular/platform-browser';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { DragDropModule } from '@angular/cdk/drag-drop';
import { RouterModule, RouteReuseStrategy } from '@angular/router';

import { MatButtonModule } from '@angular/material/button';
import { MatChipsModule } from '@angular/material/chips';
import { MatDialogModule } from '@angular/material/dialog';
import { MatFormFieldModule } from '@angular/material/form-field';
import { MatIconModule } from '@angular/material/icon';
import { MatInputModule } from '@angular/material/input';
import { MatListModule } from '@angular/material/list';
import { MatMenuModule } from '@angular/material/menu';
import { MatProgressBarModule } from '@angular/material/progress-bar';
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
import { GroupFormComponent } from './group-form/group-form.component';
import { GroupsGraphComponent } from './groups-graph/groups-graph.component';
import { GroupsPageComponent } from './groups-page/groups-page.component';
import { ObjectivesComponent } from './objectives/objectives.component';
import { OptionsPageComponent } from './options-page/options-page.component';
import { SubjectFormComponent } from './subject-form/subject-form.component';
import { SubjectsPageComponent } from './subjects-page/subjects-page.component';
import { TeacherFormComponent } from './teacher-form/teacher-form.component';
import { TeachersPageComponent } from './teachers-page/teachers-page.component';
import { UniqueIntegersChipInputComponent } from './unique-integers-chip-input/unique-integers-chip-input.component';
import { WeeklyTimetableComponent } from './weekly-timetable/weekly-timetable.component';

import { CopyDataDirective } from './copy-data.directive';

import { ReuseStrategy } from './reuse-strategy';

import { enableMapSet, enablePatches } from 'immer';

enableMapSet();
enablePatches();

@NgModule({
	declarations: [
		AppComponent,
		ColloscopeComponent,
		ComputationPageComponent,
    	ConnectionDialogComponent,
		CopyDataDirective,
		GroupFormComponent,
		GroupsGraphComponent,
		GroupsPageComponent,
		ObjectivesComponent,
		OptionsPageComponent,
		SubjectFormComponent,
		SubjectsPageComponent,
		TeacherFormComponent,
		TeachersPageComponent,
		UniqueIntegersChipInputComponent,
		WeeklyTimetableComponent,
	],
	imports: [
		BrowserModule,
		BrowserAnimationsModule,
		DragDropModule,
		FormsModule,
		ReactiveFormsModule,
		RouterModule.forRoot([
			{path: 'groups', component: GroupsPageComponent},
			{path: 'subjects', component: SubjectsPageComponent},
			{path: 'teachers', component: TeachersPageComponent},
			{path: 'options', component: OptionsPageComponent},
			{path: 'computation', component: ComputationPageComponent},
			{path: '**', redirectTo: '/groups'},
		]),
		
		MatButtonModule,
		MatChipsModule,
		MatDialogModule,
		MatFormFieldModule,
		MatIconModule,
		MatInputModule,
		MatListModule,
		MatMenuModule,
		MatProgressBarModule,
		MatProgressSpinnerModule,
		MatSelectModule,
		MatSidenavModule,
		MatSnackBarModule,
		MatTableModule,
		MatToolbarModule,
	],
	providers: [
		{ provide: RouteReuseStrategy, useClass: ReuseStrategy },
	],
	bootstrap: [AppComponent]
})
export class AppModule { }
