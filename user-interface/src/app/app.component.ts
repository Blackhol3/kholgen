import { Component } from '@angular/core';
/*
import { QWebChannel } from 'qwebchannel';

protected core: any;

constructor() {
	let websocket = new WebSocket('ws://localhost:4201');
	websocket.addEventListener('open', () => {
		new QWebChannel(websocket, (channel: any) => {
			this.core = channel.objects.core;
			this.core.receiveText('Ouverture de la session.');
			this.core.sendText.connect((text: string) => { this.text += text + "\n"; });
		});
	});
}
*/

@Component({
	selector: 'app-root',
	templateUrl: './app.component.html',
	styleUrls: ['./app.component.scss']
})
export class AppComponent {
}
