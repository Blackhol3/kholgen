import { Component } from '@angular/core';
import { CdkDragDrop } from '@angular/cdk/drag-drop';

import { StateService } from '../state.service';
import { UndoStackService } from '../undo-stack.service';

@Component({
	selector: 'app-options-page',
	templateUrl: './options-page.component.html',
	styleUrls: ['./options-page.component.scss'],
})
export class OptionsPageComponent {
	constructor(public state: StateService, private undoStack: UndoStackService) { }
	
	onDrop($event: CdkDragDrop<any[]>) {
		this.undoStack.actions.move('objectives', $event.previousIndex, $event.currentIndex);
		
		/** @todo Really, really nasty way to update the views */
		this.undoStack.undo();
		this.undoStack.redo();
	}
}
