import { Component } from '@angular/core';

import { SettingsService } from '../settings.service';

@Component({
	selector: 'app-computation-page',
	templateUrl: './computation-page.component.html',
	styleUrls: ['./computation-page.component.scss']
})
export class ComputationPageComponent {
	constructor(public settings: SettingsService) { }
	
	compute() {
		
	}
}
