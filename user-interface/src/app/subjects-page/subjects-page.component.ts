import { Component } from '@angular/core';

import { SettingsService } from '../settings.service';

@Component({
	selector: 'app-subjects-page',
	templateUrl: './subjects-page.component.html',
	styleUrls: ['./subjects-page.component.scss']
})
export class SubjectsPageComponent {
	constructor(public settings: SettingsService) { }
}
