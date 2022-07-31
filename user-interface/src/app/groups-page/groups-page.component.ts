import { Component, OnInit, OnDestroy } from '@angular/core';
import { CdkDragDrop, moveItemInArray } from '@angular/cdk/drag-drop';
import { Subscription } from 'rxjs';

import { listAnimation, slideAnimation } from '../animations';
import { Group } from '../group';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-groups-page',
	templateUrl: './groups-page.component.html',
	styleUrls: ['./groups-page.component.scss'],
	animations: [
		listAnimation,
		slideAnimation,
	],
})
export class GroupsPageComponent implements OnInit, OnDestroy {
	selectedGroupIds: string[] = [];
	storeSubscription: Subscription | undefined;
	
	constructor(public store: StoreService, private undoStack: UndoStackService) { }
	
	ngOnInit() {
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.updateSelectedGroup());
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}
	
	onDrop($event: CdkDragDrop<any[]>) {
		this.selectedGroupIds = [this.store.state.groups[$event.previousIndex].id];
		this.undoStack.do(state => { moveItemInArray(state.subjects, $event.previousIndex, $event.currentIndex) });
	}
	
	addNewGroup() {
		let name = '';
		for (let i = 1; name = `Groupe ${i}`, this.store.state.groups.some(group => group.name === name); ++i) {
		}
		
		this.undoStack.do(state => { state.groups.push(new Group(name, [], 0)) });
	}
	
	deleteGroup() {
		const group = this.store.state.findId('groups', this.selectedGroupIds[0])!;
		const index = this.store.state.groups.indexOf(group);
		
		this.undoStack.do(state => { state.groups.splice(index, 1) });
		this.selectedGroupIds = this.store.state.groups.length > 0 ? [this.store.state.groups[Math.max(0, index - 1)].id] : [];
	}
	
	updateSelectedGroup() {
		if (this.store.state.findId('groups', this.selectedGroupIds[0]) === undefined) {
			this.selectedGroupIds = this.store.state.groups.length > 0 ? [this.store.state.groups[this.store.state.groups.length - 1].id] : [];
		}
	}
	
	onPaste($event: ClipboardEvent) {
		const jsonString = $event.clipboardData?.getData('application/json-group') ?? '';
		if (jsonString === '') {
			return;
		}
		
		const jsonGroup = JSON.parse(jsonString) as ReturnType<Group['toHumanJsonObject']>;
		while (this.store.state.groups.some(subject => subject.name === jsonGroup.name)) {
			jsonGroup.name += ' (copie)';
		}
		
		this.undoStack.do(state => {
			const group = Group.fromJsonObject(jsonGroup);
			group.setNextGroupFromJsonObject(jsonGroup, state.groups);
			state.groups.push(group);
		});
	}
}