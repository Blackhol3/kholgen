import { Injectable, inject } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';

import { castDraft } from 'immer';
import { type ChannelObject, QWebChannel } from 'qwebchannel';
import { Observable, Subject } from 'rxjs';

import { Colle } from './colle';
import { toSolverJson } from './json';
import { Timeslot } from './timeslot';
import { StoreService } from './store.service';

import { DialogComponent } from './dialog/dialog.component';

type JsonColle = {
	teacherId: string,
	timeslot: {
		day: number,
		hour: number,
	},
	trioId: number,
	weekId: number,
};

type JsonObjectiveComputation = {
	objectiveName: string,
	value: number,
};

type Communication = {
	slots: {
		compute: (state: unknown) => Promise<void>,
		stopComputation: () => Promise<void>,
	},

	signals: {
		solutionFound: (colles: JsonColle[], objectiveComputations: JsonObjectiveComputation[]) => void,
		computationFinished: (success: boolean) => void,
	},
}

@Injectable({
	providedIn: 'root'
})
export class CommunicationService {
	protected readonly dialog = inject(MatDialog);

	protected websocket: WebSocket & {closedManually?: boolean} | undefined;
	protected communication: ChannelObject<Communication> | undefined;
	protected computeSubject: Subject<void> | undefined;
	
	async connect(): Promise<boolean> {
		return new Promise(resolve => {
			if (this.websocket !== undefined) {
				resolve(this.websocket.readyState === WebSocket.OPEN);
				return;
			}
			
			let dialogTimeout: number | undefined;
			if (this.dialog.getDialogById('connection') === undefined) {
				dialogTimeout = window.setTimeout(
					() => this.dialog.open(DialogComponent, { data: {type: 'connection'} }).afterClosed().subscribe(() => this.disconnect()),
					300
				);
			}
			const closeDialog = () => {
				const dialog = this.dialog.getDialogById('connection');
				if (dialog !== undefined) {
					dialog.close();
				}
				window.clearTimeout(dialogTimeout);
			};
			
			this.websocket = new WebSocket('ws://localhost:4201');
			this.websocket.addEventListener('open', () => {
				new QWebChannel<{communication: Communication}>(this.websocket!, channel => {
					this.communication = channel.objects.communication;
					
					closeDialog();
					resolve(true);
				});
			});
			this.websocket.addEventListener('close', () => {
				const tryToReconnect = this.communication === undefined && this.websocket?.closedManually !== true;
				this.websocket = undefined;
				this.communication = undefined;
				this.computeSubject = undefined;
				
				if (tryToReconnect) {
					resolve(this.connect());
				}
				else {
					closeDialog();
					resolve(false);
				}
			});
		});
	}
	
	disconnect(): void {
		if (this.websocket !== undefined) {
			this.websocket.closedManually = true;
			this.websocket.close();
		}
	}
	
	compute(store: StoreService): Observable<void> {
		this.checkIfOpen();
		
		if (this.computeSubject !== undefined) {
			return this.computeSubject.asObservable();
		}

		store.do(state => { state.computation = castDraft(store.state.prepareComputation()); });
		
		this.computeSubject = new Subject<void>();
		this.communication.solutionFound.connect((jsonColles, jsonObjectiveComputations) => {
			this.computeSubject?.next();
			this.importJsonColles(store, jsonColles);
			this.importJsonObjectiveComputations(store, jsonObjectiveComputations);
		});
		this.communication.computationFinished.connect(() => {
			this.computeSubject?.complete();
			this.computeSubject = undefined;
		});
		void this.communication.compute(toSolverJson(store.state));
		
		return this.computeSubject.asObservable();
	}
	
	stopComputation() {
		this.checkIfOpen();
		void this.communication.stopComputation();
	}
	
	protected checkIfOpen(): asserts this is {communication: ChannelObject<Communication>} {
		if (this.communication === undefined) {
			throw new Error('The communication is not opened.');
		}
	}
	
	protected importJsonColles(store: StoreService, jsonColles: JsonColle[]): void {
		store.do(state => {
			state.computation!.colles = jsonColles.map(colle => new Colle(
				colle.teacherId,
				new Timeslot(colle.timeslot.day, colle.timeslot.hour),
				colle.trioId,
				colle.weekId,
			));
		});
	}
	
	protected importJsonObjectiveComputations(store: StoreService, jsonObjectiveComputations: JsonObjectiveComputation[]): void {
		store.do(state => {
			for (const jsonObjectiveComputation of jsonObjectiveComputations) {
				state.computation!.objectiveComputations.find(x => x.objective.name === jsonObjectiveComputation.objectiveName)!.value = jsonObjectiveComputation.value;
			}
		});
	}
}
