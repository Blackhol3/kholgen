import { Injectable } from '@angular/core';
import { get, set } from 'lodash';
import { Subject } from 'rxjs';

import { SettingsService } from './settings.service';

interface Command {
	undo(state: object): void;
	redo(state: object): void;
	merge(command: Command): []|[Command]|[Command, Command];
}

class UpdateCommand implements Command {
	protected path: string;
	protected oldValue: any;
	protected newValue: any;
	
	constructor(state: object, path: string, newValue: any) {
		this.path = path;
		this.oldValue = get(state, this.path);
		this.newValue = newValue;
	}
	
	undo(state: object): void {
		set(state, this.path, this.oldValue);
	}
	
	redo(state: object): void {
		set(state, this.path, this.newValue);
	}
	
	merge(command: Command): []|[UpdateCommand]|[Command, UpdateCommand] {
		if (command instanceof UpdateCommand && command.path === this.path) {
			command.newValue = this.newValue;
			return command.newValue === command.oldValue ? [] : [command];
		}
		
		return [command, this];
	}
}

class PushCommand implements Command {
	constructor(
		protected path: string,
		protected value: any,
	) { }
	
	undo(state: object): void {
		get(state, this.path).pop();
	}
	
	redo(state: object): void {
		get(state, this.path).push(this.value);
	}
	
	merge(command: Command): [Command, PushCommand] {
		return [command, this];
	}
}

class SpliceCommand implements Command {
	protected path: string;
	protected index: number;
	protected value: any;
	
	constructor(state: object, path: string, index: number) {
		this.path = path;
		this.index = index;
		this.value = get(state, this.path)[this.index];
	}
	
	undo(state: object): void {
		get(state, this.path).splice(this.index, 0, this.value);
	}
	
	redo(state: object): void {
		get(state, this.path).splice(this.index, 1);
	}
	
	merge(command: Command): [Command, SpliceCommand] {
		return [command, this];
	}
}

@Injectable({
	providedIn: 'root'
})
export class UndoStackService {
	protected changeSubject = new Subject<void>();
	public changeObservable = this.changeSubject.asObservable();
	
	protected readonly state: object;
	
	protected undoStack: Command[] = [];
	protected redoStack: Command[] = [];
	
	constructor(state: SettingsService) {
		this.state = state;
	}
	
	clear(): void {
		this.undoStack = [];
		this.redoStack = [];
	}
	
	actions = {
		update: (path: string, newValue: any, shouldMergeIfPossible: boolean = true): void => {
			this.add(new UpdateCommand(this.state, path, newValue), shouldMergeIfPossible);
		},
		
		push: (path: string, value: any): void => {
			this.add(new PushCommand(path, value), false);
		},
		
		splice: (path: string, index: any): void => {
			this.add(new SpliceCommand(this.state, path, index), false);
		},
	};
	
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
		
		command.undo(this.state);
		this.changeSubject.next();
	}
	
	redo(): void {
		if (!this.canRedo()) {
			return;
		}
		
		const command = this.redoStack.shift() as Command;
		this.undoStack.push(command);
		
		command.redo(this.state);
		this.changeSubject.next();
	}
	
	protected add(command: Command, shouldMergeIfPossible: boolean) {
		const previousCommand = this.undoStack[this.undoStack.length - 1];
		if (shouldMergeIfPossible && previousCommand !== undefined) {
			this.undoStack.splice(-1, 1, ...command.merge(previousCommand));
		}
		else {
			this.undoStack.push(command);
		}
		
		command.redo(this.state);
		this.redoStack = [];
	}
}
