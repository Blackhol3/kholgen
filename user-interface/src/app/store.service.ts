import { Injectable } from '@angular/core';
import { Draft, Patch, applyPatches, enablePatches, produceWithPatches } from 'immer';
import { Subject } from 'rxjs';

import { State } from './state';

@Injectable({
	providedIn: 'root'
})
export class StoreService {
	protected changeSubject = new Subject<void>();
	changeObservable = this.changeSubject.asObservable();
	
	state = new State();
	
	constructor() {
		enablePatches();
	}
	
	do(recipe: (state: Draft<State>) => any): [Patch[], Patch[]] {
		const [state, patches, inversePatches] = produceWithPatches(this.state, recipe);
		this.state = state;
		this.changeSubject.next();
		
		return [patches, inversePatches];
	}
	
	apply(patches: Patch[]) {
		this.state = applyPatches(this.state, patches);
		this.changeSubject.next();
	}
}
