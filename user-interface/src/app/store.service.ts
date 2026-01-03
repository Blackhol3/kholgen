import { Injectable, signal } from '@angular/core';
import { type Draft, type Patch, applyPatches, produceWithPatches } from 'immer';

import { State } from './state';

@Injectable({
	providedIn: 'root'
})
export class StoreService {
	#state = signal(new State());
	state = this.#state.asReadonly();
	
	do(recipe: (state: Draft<State>) => Draft<State> | void | undefined) {
		const [state, patches, inversePatches] = produceWithPatches(this.#state(), recipe);
		this.#state.set(state);
		
		return [patches, inversePatches] as const;
	}
	
	apply(patches: Patch[]) {
		this.#state.set(applyPatches(this.state(), patches));
	}
}
