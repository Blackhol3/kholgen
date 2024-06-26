import { Injectable } from '@angular/core';
import { type Patch } from 'immer';

import { StoreService } from './store.service';

function comparePath(patch1: Patch, patch2: Patch): boolean {
	return patch1.path.length === patch2.path.length && patch1.path.every((value, key) => value === patch2.path[key]);
}

interface Command {
	undo(): Patch[];
	redo(): Patch[];
	merge(command: Command): []|[Command]|[Command, Command];
}

class PatchesCommand implements Command {
	constructor(
		protected redoPatches: Patch[],
		protected undoPatches: Patch[],
	) { }
	
	undo() {
		return this.undoPatches;
	}
	
	redo() {
		return this.redoPatches;
	}
	
	merge(command: Command): []|[PatchesCommand]|[Command, PatchesCommand] {
		if (command instanceof PatchesCommand && this.isMergeable() && command.isMergeable() && comparePath(this.redoPatches[0], command.redoPatches[0])) {
			command.redoPatches = this.redoPatches;
			return command.redoPatches[0].value === command.undoPatches[0].value ? [] : [command];
		}
		
		return [command, this];
	}
	
	protected isMergeable() {
		return this.redoPatches.length === 1 && this.redoPatches[0].op === 'replace';
	}
}

class GroupCommand implements Command {
	protected commands: Command[] = [];
	
	undo() {
		return ([] as Patch[]).concat(...this.commands.slice().reverse().map(command => command.undo()));
	}
	
	redo() {
		return ([] as Patch[]).concat(...this.commands.map(command => command.redo()));
	}
	
	merge(command: Command): [Command, GroupCommand] {
		return [command, this];
	}
	
	push(command: Command) {
		this.commands.push(command);
	}
	
	splice(start: number, deleteCount: number, ...items: Command[]) {
		this.commands.splice(start, deleteCount, ...items);
	}
	
	last(): Command | undefined {
		return last(this.commands);
	}
}

function last(collection: Command[]|GroupCommand): Command | undefined {
	if (collection instanceof Array) {
		return collection[collection.length - 1];
	}
	
	return collection.last();
}

@Injectable({
	providedIn: 'root'
})
export class UndoStackService {
	protected undoStack: Command[] = [];
	protected redoStack: Command[] = [];
	protected groupLevel = 0;
	
	constructor(protected store: StoreService) {
	}
	
	clear(): void {
		this.undoStack = [];
		this.redoStack = [];
		this.groupLevel = 0;
	}
	
	do(recipe: Parameters<typeof this.store.do>[0], shouldMergeIfPossible: boolean = true) {
		const [patches, inversePatches] = this.store.do(recipe);
		if (patches.length !== 0) {
			this.add(new PatchesCommand(patches, inversePatches), shouldMergeIfPossible);
		}
	}
	
	canUndo(): boolean {
		return !this.isGrouped() && this.undoStack.length > 0;
	}
	
	canRedo(): boolean {
		return !this.isGrouped() && this.redoStack.length > 0;
	}
	
	undo(): void {
		if (!this.canUndo()) {
			return;
		}
		
		const command = this.undoStack.pop() as Command;
		this.redoStack.unshift(command);
		this.store.apply(command.undo());
	}
	
	redo(): void {
		if (!this.canRedo()) {
			return;
		}
		
		const command = this.redoStack.shift() as Command;
		this.undoStack.push(command);
		this.store.apply(command.redo());
	}
	
	startGroup(): void {
		if (!this.isGrouped()) {
			this.undoStack.push(new GroupCommand());
		}
		
		++this.groupLevel;
	}
	
	isGrouped(): boolean {
		return this.groupLevel > 0;
	}
	
	endGroup(): void {
		--this.groupLevel;
		
		if (!this.isGrouped()) {
			const groupCommand = last(this.undoStack) as GroupCommand;
			if (groupCommand.last() === undefined) {
				this.undoStack.pop();
			}
		}
	}
	
	protected add(command: Command, shouldMergeIfPossible: boolean) {
		const stack = this.isGrouped() ? last(this.undoStack) as GroupCommand : this.undoStack;
		const previousCommand = last(stack);
		
		if (shouldMergeIfPossible && previousCommand !== undefined) {
			stack.splice(-1, 1, ...command.merge(previousCommand));
		}
		else {
			stack.push(command);
		}
		
		this.redoStack = [];
	}
}
