import { immerable, type Draft } from 'immer';

import { Calendar } from './calendar';
import { type Colle } from './colle';
import { type Group } from './group';
import { type ObjectiveComputation } from './objective-computation';
import { type Subject } from './subject';
import { type Teacher } from './teacher';
import { Trio } from './trio';

export class Computation {
	[immerable] = true;

	readonly trios: readonly Trio[];
	
	constructor(
		readonly groups: readonly Group[] = [],
		readonly subjects: readonly Subject[] = [],
		readonly teachers: readonly Teacher[] = [],
		readonly calendar = new Calendar(),
		readonly colles: readonly Colle[] = [],
		readonly objectiveComputations: readonly ObjectiveComputation[] = [],
	) {
		this.trios = this.createTrios();
	}

	findId<S extends this | Draft<this>, P extends 'groups' | 'subjects' | 'teachers'>(this: S, property: P, id: S[P][number]['id']): S[P][number] | undefined {
		for (const element of this[property]) {
			if (element.id === id) {
				return element;
			}
		}

		return undefined;
	}

	protected createTrios() {
		const trioIds = [...new Set(this.groups.flatMap(group => [...group.trioIds]))].sort((a, b) => a - b);
		return trioIds.map(trioId => new Trio(
			trioId,
			this.groups.filter(group => group.trioIds.has(trioId)).map(group => group.id),
		));
	}
}
