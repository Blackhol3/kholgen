import { Component } from '@angular/core';
import { CdkDragDrop, moveItemInArray } from '@angular/cdk/drag-drop';

import { StoreService } from '../store.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-options-page',
	templateUrl: './options-page.component.html',
	styleUrls: ['./options-page.component.scss'],
})
export class OptionsPageComponent {
	constructor(public store: StoreService, private undoStack: UndoStackService) { }
	
	onDrop($event: CdkDragDrop<any[]>) {
		this.undoStack.do(state => { moveItemInArray(state.objectives, $event.previousIndex, $event.currentIndex) });
	}
}
