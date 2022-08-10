import { ChangeDetectionStrategy, Component, Input, OnChanges } from '@angular/core';

import { Group } from '../group';

@Component({
	selector: 'app-groups-graph',
	templateUrl: './groups-graph.component.html',
	styleUrls: ['./groups-graph.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
})
export class GroupsGraphComponent implements OnChanges {
	@Input() groups: readonly Group[] = [];
	
	readonly radius = 35;
	readonly individualRadius = 10;
	
	constructor() { }
	
	ngOnChanges() {
		
	}
	
	getTransform(index: number) {
		const angle = this.getAngle(index, 'deg');
		const transform = [
			'rotate(', angle, ')',
			'translate(', this.radius, ')',
			'rotate(', -angle, ')',
		];
		
		return transform.join(' ');
	}
	
	getPath(index: number) {
		const [deltaDistance,] = this.getNextGroupDelta(index);
		const path = [
			'M', this.individualRadius, this.individualRadius*3/4,
			'c', deltaDistance/4, deltaDistance/5, deltaDistance*3/4, deltaDistance/5, deltaDistance, 0,
		];
		
		return path.join(' ');
	}
	
	getNextGroupTextTransform(index: number) {
		const [deltaDistance, deltaAngle] = this.getNextGroupDelta(index);
		const transform = [
			'rotate(', deltaAngle, ')',
			'translate(', this.individualRadius + deltaDistance/2, deltaDistance/4.5, ')',
			'rotate(', -deltaAngle, ')',
		];
		
		return transform.join(' ');
	}
	
	getNumberOfTrios() {
		let numberOfTrios = 0;
		for (let group of this.groups) {
			numberOfTrios += group.numberOfTrios;
		}
		
		return numberOfTrios;
	}
	
	getTriosText(numberOfTrios: number) {
		return `${numberOfTrios} trinôme${numberOfTrios > 1 ? 's' : ''}`;
	}
	
	getStudentsText() {
		const numberOfTrios = this.getNumberOfTrios();
		return numberOfTrios === 0 ? `0 étudiant` : `${numberOfTrios - 2} à ${numberOfTrios} étudiants`;
	}
	
	protected getAngle(index: number, unit: 'deg' | 'rad') {
		const angle = -index * 360/(this.groups.length);
		return unit === 'deg' ? angle : (angle * Math.PI/180);
	}
	
	protected getNextGroupDelta(index: number) {
		const nextGroupIndex = this.groups.findIndex(g => g.id === this.groups[index].nextGroupId)!;
		
		const angle = this.getAngle(index, 'rad');
		const nextGroupAngle = this.getAngle(nextGroupIndex, 'rad');
		
		const position = [Math.cos(angle) * this.radius, Math.sin(angle) * this.radius];
		const nextGroupPosition = [Math.cos(nextGroupAngle) * this.radius, Math.sin(nextGroupAngle) * this.radius];
		const deltaPosition = [nextGroupPosition[0] - position[0], nextGroupPosition[1] - position[1]];
		const deltaDistance = Math.hypot(...deltaPosition) - 2*this.individualRadius;
		const deltaAngle = Math.atan2(deltaPosition[1], deltaPosition[0]) * 180/Math.PI;
		
		return [deltaDistance, deltaAngle];
	}
}
