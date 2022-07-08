import { Component } from '@angular/core';

import { CommunicationService } from '../communication.service';
import { SettingsService } from '../settings.service';

@Component({
	selector: 'app-computation-page',
	templateUrl: './computation-page.component.html',
	styleUrls: ['./computation-page.component.scss']
})
export class ComputationPageComponent {
	constructor(private communication: CommunicationService, private settings: SettingsService) { }
	
	compute() {
		this.communication.connect().then(() => {
			this.communication.compute(this.settings).subscribe(colles => {
				this.settings.colles = colles;
			});
		});
	}
}
