import { enableProdMode } from '@angular/core';
import { bootstrapApplication } from '@angular/platform-browser';
import { provideAnimations } from '@angular/platform-browser/animations';
import { RouteReuseStrategy, provideRouter } from '@angular/router';

import { enableMapSet, enablePatches } from 'immer';

import { environment } from './environments/environment';
import { AppComponent } from './app/app.component';
import { ReuseStrategy } from './app/reuse-strategy';

import { GroupsPageComponent } from './app/groups-page/groups-page.component';
import { SubjectsPageComponent } from './app/subjects-page/subjects-page.component';
import { TeachersPageComponent } from './app/teachers-page/teachers-page.component';
import { OptionsPageComponent } from './app/options-page/options-page.component';
import { ComputationPageComponent } from './app/computation-page/computation-page.component';

enableMapSet();
enablePatches();

if (environment.production) {
	enableProdMode();
}

bootstrapApplication(AppComponent, {
	providers: [
		{provide: RouteReuseStrategy, useClass: ReuseStrategy},
		provideAnimations(),
		provideRouter([
			{path: 'groups', component: GroupsPageComponent},
			{path: 'subjects', component: SubjectsPageComponent},
			{path: 'teachers', component: TeachersPageComponent},
			{path: 'options', component: OptionsPageComponent},
			{path: 'computation', component: ComputationPageComponent},
			{path: '**', redirectTo: '/groups'},
		])
	]
}).catch(err => console.error(err));
