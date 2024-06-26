import { Injectable } from '@angular/core';
import { type Draft, type Patch, applyPatches, produceWithPatches } from 'immer';
import { Subject } from 'rxjs';

import { State } from './state';

@Injectable({
	providedIn: 'root'
})
export class StoreService {
	protected changeSubject = new Subject<void>();
	changeObservable = this.changeSubject.asObservable();
	
	state = new State();
	
	do(recipe: (state: Draft<State>) => Draft<State> | void | undefined) {
		const [state, patches, inversePatches] = produceWithPatches(this.state, recipe);
		this.state = state;
		this.changeSubject.next();
		
		return [patches, inversePatches] as const;
	}
	
	apply(patches: Patch[]) {
		this.state = applyPatches(this.state, patches);
		this.changeSubject.next();
	}
}
