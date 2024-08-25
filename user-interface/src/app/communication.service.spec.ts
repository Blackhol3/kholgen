import { TestBed } from '@angular/core/testing';
import { MatDialog } from '@angular/material/dialog';

import qwebchannel from 'qwebchannel';

import { CommunicationService } from './communication.service';

describe('CommunicationService', () => {
	let service: CommunicationService;
	let dialog: MatDialog;

	beforeEach(() => {
		TestBed.configureTestingModule({});
		service = TestBed.inject(CommunicationService);
		dialog = TestBed.inject(MatDialog);
	});

	describe('should try to connect', () => {
		let webSocket: WebSocket;

		beforeEach(() => {
			jasmine.clock().install();

			webSocket = new EventTarget() as WebSocket;
			Object.defineProperty(webSocket, 'readyState', {get: () => WebSocket.CLOSED, configurable: true});
			webSocket.close = jasmine.createSpy().and.callFake(() => webSocket.dispatchEvent(new CloseEvent('close')));

			spyOn(window, 'WebSocket').and.returnValue(webSocket);
			spyOn(qwebchannel, 'QWebChannel').and.callFake(function (_, callback) {
				callback({objects: {communication: {}}});
				return {};
			});
		});

		afterEach(() => {
			jasmine.clock().uninstall();
		});

		it('and resolve positively without dialog if it can connect quickly', async () => {
			const promise = service.connect();
			await expectAsync(promise).toBePending();
			expect(dialog.openDialogs).toHaveSize(0);

			webSocket.dispatchEvent(new Event('open'));
			expect(await promise).toBeTrue();
			expect(dialog.openDialogs).toHaveSize(0);
			expect(window.WebSocket).toHaveBeenCalledTimes(1);
		});

		it('and resolve positively and close the dialog if it can connect after a long time', async () => {
			const promise = service.connect();
			jasmine.clock().tick(300);
			await expectAsync(promise).toBePending();
			expect(dialog.openDialogs).toHaveSize(1);
			expect(dialog.openDialogs[0].id).toBe('connection');
			
			webSocket.dispatchEvent(new Event('open'));
			expect(await promise).toBeTrue();
			expect(dialog.openDialogs).toHaveSize(0);
			expect(window.WebSocket).toHaveBeenCalledTimes(1);
		});

		it('and resolve negatively if the user closes the dialog', async () => {
			const promise = service.connect();
			jasmine.clock().tick(300);
			await expectAsync(promise).toBePending();
			expect(dialog.openDialogs).toHaveSize(1);
			expect(dialog.openDialogs[0].id).toBe('connection');
			
			dialog.closeAll();
			jasmine.clock().tick(10);

			expect(await promise).toBeFalse();
			expect(dialog.openDialogs).toHaveSize(0);
			expect(window.WebSocket).toHaveBeenCalledTimes(1);
		});

		it('and try to connect again if the socket timed out', async () => {
			const promise = service.connect();
			webSocket.dispatchEvent(new CloseEvent('close'));
			await expectAsync(promise).toBePending();
			expect(window.WebSocket).toHaveBeenCalledTimes(2);
		});

		it('and resolve positively if it is already connected', async () => {
			const promise = service.connect();
			webSocket.dispatchEvent(new Event('open'));
			spyOnProperty(webSocket, 'readyState', 'get').and.returnValue(WebSocket.OPEN);
			await promise;

			expect(await service.connect()).toBeTrue();
		});
	});
});
