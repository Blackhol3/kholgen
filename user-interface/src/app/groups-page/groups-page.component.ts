import { ChangeDetectionStrategy, Component, inject, signal } from '@angular/core';
import { type CdkDragDrop, moveItemInArray, CdkDropList, CdkDrag } from '@angular/cdk/drag-drop';
import { FormsModule } from '@angular/forms';

import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MatListModule } from '@angular/material/list';

import { castDraft } from 'immer';

import { Group } from '../group';
import { type HumanJson } from '../json';
import { effectOn } from '../misc';
import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

import { CopyDataDirective } from '../copy-data.directive';
import { EnterLeaveAnimationDirective } from '../enter-leave-animation.directive';
import { GroupFormComponent } from '../group-form/group-form.component';
import { GroupsGraphComponent } from '../groups-graph/groups-graph.component';

@Component({
	selector: 'app-groups-page',
	templateUrl: './groups-page.component.html',
	styleUrls: ['./groups-page.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
	imports: [
		CdkDrag,
		CdkDropList,
		FormsModule,
		MatButtonModule,
		MatIconModule,
		MatListModule,

		CopyDataDirective,
		EnterLeaveAnimationDirective,
		GroupsGraphComponent,
		GroupFormComponent,
	],
})
export class GroupsPageComponent {
	readonly store = inject(StoreService);
	protected readonly undoStack = inject(UndoStackService);

	protected selectedGroupIds = signal<readonly string[]>([]);

	constructor() {
		effectOn(this.store.state, () => this.updateSelectedGroup());
	}
	
	onDrop($event: CdkDragDrop<unknown[]>) {
		this.selectedGroupIds.set([this.store.state().groups[$event.previousIndex].id]);
		this.undoStack.do(state => { moveItemInArray(state.groups, $event.previousIndex, $event.currentIndex) });
	}
	
	addNewGroup() {
		let name = '';
		for (let i = 1; name = `Groupe ${i}`, this.store.state().groups.some(group => group.name === name); ++i) {
			// Empty
		}
		
		const group = new Group(name, [], new Set());
		this.undoStack.do(state => { state.groups.push(castDraft(group)) });
		this.selectedGroupIds.set([group.id]);
	}
	
	deleteGroup() {
		const group = this.store.state().findId('groups', this.selectedGroupIds()[0])!;
		const index = this.store.state().groups.indexOf(group);
		
		this.undoStack.do(state => {
			for (const otherGroup of state.groups) {
				if (otherGroup.nextGroupId === group.id) {
					otherGroup.nextGroupId = null;
				}
			}
			
			state.groups.splice(index, 1);
		});
		this.selectedGroupIds.set(this.store.state().groups.length > 0 ? [this.store.state().groups[Math.max(0, index - 1)].id] : []);
	}
	
	updateSelectedGroup() {
		if (this.store.state().findId('groups', this.selectedGroupIds()[0]) === undefined) {
			this.selectedGroupIds.set(this.store.state().groups.length > 0 ? [this.store.state().groups[this.store.state().groups.length - 1].id] : []);
		}
	}
	
	onPaste($event: ClipboardEvent) {
		const jsonString = $event.clipboardData?.getData('application/json-group') ?? '';
		if (jsonString === '') {
			return;
		}
		
		const jsonGroup = JSON.parse(jsonString) as HumanJson<Group>;
		while (this.store.state().groups.some(subject => subject.name === jsonGroup.name)) {
			jsonGroup.name += ' (copie)';
		}
		
		const group = Group.fromHumanJson(jsonGroup).setNextGroupFromHumanJson(jsonGroup, this.store.state().groups);
		this.undoStack.do(state => { state.groups.push(castDraft(group)); });
		this.selectedGroupIds.set([group.id]);
	}

	protected onKeydown($event: KeyboardEvent) {
		if ($event.key === 'Delete') {
			this.deleteGroup();
			$event.preventDefault();
		}
	}
}
