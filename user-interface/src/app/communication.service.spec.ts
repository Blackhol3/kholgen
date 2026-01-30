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
			vi.useFakeTimers();
			
			webSocket = new EventTarget() as WebSocket;
			Object.defineProperty(webSocket, 'readyState', {get: () => WebSocket.CLOSED, configurable: true});
			webSocket.close = vi.fn(() => webSocket.dispatchEvent(new CloseEvent('close')));

			vi.stubGlobal('WebSocket', vi.fn(function () { return webSocket; }));
			vi.mock('qwebchannel', { spy: true });
			vi.mocked(qwebchannel.QWebChannel).mockImplementation(function (_, callback) {
				callback({objects: {communication: {}}});
				return {};
			});
		});

		afterEach(() => {
			vi.useRealTimers();
			vi.unstubAllGlobals();
		});

		it('and resolve positively without dialog if it can connect quickly', async () => {
			const promise = service.connect();
			await expect(promise).toBePending();
			expect(dialog.openDialogs).toHaveLength(0);

			webSocket.dispatchEvent(new Event('open'));
			
			expect(await promise).toBe(true);
			expect(dialog.openDialogs).toHaveLength(0);
			expect(window.WebSocket).toHaveBeenCalledTimes(1);
		});

		it('and resolve positively and close the dialog if it can connect after a long time', async () => {
			const promise = service.connect();
			vi.advanceTimersByTime(300);
			await expect(promise).toBePending();
			expect(dialog.openDialogs).toHaveLength(1);
			expect(dialog.openDialogs[0].id).toBe('connection');
			
			webSocket.dispatchEvent(new Event('open'));
			vi.advanceTimersByTime(10);

			expect(await promise).toBe(true);
			expect(dialog.openDialogs).toHaveLength(0);
			expect(window.WebSocket).toHaveBeenCalledTimes(1);
		});

		it('and resolve negatively if the user closes the dialog', async () => {
			const promise = service.connect();
			vi.advanceTimersByTime(300);
			await expect(promise).toBePending();
			expect(dialog.openDialogs).toHaveLength(1);
			expect(dialog.openDialogs[0].id).toBe('connection');
			
			dialog.closeAll();
			vi.advanceTimersByTime(10);

			expect(await promise).toBe(false);
			expect(dialog.openDialogs).toHaveLength(0);
			expect(window.WebSocket).toHaveBeenCalledTimes(1);
		});

		it('and try to connect again if the socket timed out', async () => {
			const promise = service.connect();
			webSocket.dispatchEvent(new CloseEvent('close'));
			await expect(promise).toBePending();
			expect(window.WebSocket).toHaveBeenCalledTimes(2);
		});

		it('and resolve positively if it is already connected', async () => {
			const promise = service.connect();
			webSocket.dispatchEvent(new Event('open'));
			vi.spyOn(webSocket, 'readyState', 'get').mockReturnValue(WebSocket.OPEN);
			await promise;

			expect(await service.connect()).toBe(true);
		});
	});
});
