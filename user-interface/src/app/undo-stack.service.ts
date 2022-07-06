import { Injectable } from '@angular/core';
import { get, set } from 'lodash';
import { Subject } from 'rxjs';

import { SettingsService } from './settings.service';

interface Command {
	undo(state: object): void;
	redo(state: object): void;
}

class UpdateCommand implements Command {
	 constructor(
		 public path: string,
		 public oldValue: any,
		 public newValue: any,
	 ) { }
	 
	 undo(state: object): void {
		 set(state, this.path, this.oldValue);
	 }
	 
	 redo(state: object): void {
		 set(state, this.path, this.newValue);
	 }
}

@Injectable({
	providedIn: 'root'
})
export class UndoStackService {
	protected changeSubject = new Subject<void>();
	public changeObservable = this.changeSubject.asObservable();
	
	protected savedState!: object;
	protected readonly state: object;
	
	protected undoStack: Command[] = [];
	protected redoStack: Command[] = [];
	
	constructor(state: SettingsService) {
		this.state = state;
		this.clear();
	}
	
	clear(): void {
		this.savedState = structuredClone(this.state);
		this.undoStack = [];
		this.redoStack = [];
	}
	
	addUpdate(path: string, mergeIdenticalPath: boolean = true): void {
		const newValue = get(this.state, path);
		const previousCommand = this.undoStack[this.undoStack.length - 1];
		
		if (mergeIdenticalPath && previousCommand instanceof UpdateCommand && previousCommand.path === path) {
			if (previousCommand.oldValue === newValue) {
				this.undoStack.pop();
			}
			else {
				previousCommand.newValue = newValue;
			}
		}
		else {
			this.undoStack.push(new UpdateCommand(
				path,
				get(this.savedState, path),
				newValue,
			));
		}
		
		set(this.savedState, path, newValue);
		this.redoStack = [];
	}
	
	canUndo(): boolean {
		return this.undoStack.length > 0;
	}
	
	canRedo(): boolean {
		return this.redoStack.length > 0;
	}
	
	undo(): void {
		if (!this.canUndo()) {
			return;
		}
		
		const command = this.undoStack.pop() as Command;
		this.redoStack.unshift(command);
		
		command.undo(this.savedState);
		command.undo(this.state);
		this.changeSubject.next();
	}
	
	redo(): void {
		if (!this.canRedo()) {
			return;
		}
		
		const command = this.redoStack.shift() as Command;
		this.undoStack.push(command);
		
		command.redo(this.savedState);
		command.redo(this.state);
		this.changeSubject.next();
	}
}
