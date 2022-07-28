import { Component, OnInit, OnDestroy } from '@angular/core';
import { CdkDrag, CdkDragDrop, CdkDropList } from '@angular/cdk/drag-drop';
import { Subscription } from 'rxjs';

import { listAnimation, slideAnimation } from '../animations';
import { Subject } from '../subject';
import { Teacher } from '../teacher';
import { UndoStackService } from '../undo-stack.service';
import { StateService } from '../state.service';

@Component({
	selector: 'app-teachers-page',
	templateUrl: './teachers-page.component.html',
	styleUrls: ['./teachers-page.component.scss'],
	animations: [
		listAnimation,
		slideAnimation,
	],
})
export class TeachersPageComponent implements OnInit, OnDestroy {
	selectedTeachers: Teacher[] = [];
	undoStackSubscription: Subscription | undefined;
	
	constructor(public state: StateService, private undoStack: UndoStackService) { }

	ngOnInit() {
		this.undoStackSubscription = this.undoStack.changeObservable.subscribe(() => this.updateSelectedTeacher());
	}
	
	ngOnDestroy() {
		this.undoStackSubscription?.unsubscribe();
	}
	
	canDrop = (draggedTeacher: CdkDrag<Teacher>, droppedSubject: CdkDropList<Subject>): boolean => {
		for (let teacher of this.state.teachers) {
			if (teacher.subject === droppedSubject.data && teacher.name === draggedTeacher.data.name) {
				return false;
			}
		}
		
		return true;
	}
	
	onDrop($event: CdkDragDrop<Subject, Subject, Teacher>) {
		const teacher = $event.item.data;
		const teachersOfNewSubject = this.state.teachers.filter(t => t.subject === $event.container.data);
		
		const indexFrom = this.state.teachers.indexOf(teacher);
		const indexBefore = this.state.teachers.indexOf(teachersOfNewSubject[$event.currentIndex]);
		const indexTo =
			teachersOfNewSubject.length === 0 ? indexFrom :
			teacher.subject === $event.container.data ? indexBefore :
			indexBefore === -1 ? this.state.teachers.length - 1 :
			indexFrom < indexBefore ? indexBefore - 1 :
			indexBefore
		;
		this.selectedTeachers = [teacher];
		
		this.undoStack.startGroup();
		if (teacher.subject !== $event.container.data) {
			this.undoStack.actions.update(`teachers[${indexFrom}].subject`, $event.container.data);
		}
		this.undoStack.actions.move('teachers', indexFrom, indexTo);
		this.undoStack.endGroup();
		
		/** @todo Really, really nasty way to update the form */
		this.undoStack.undo();
		this.undoStack.redo();
	}
	
	addNewTeacher() {
		let name = '';
		for (let i = 1; name = `Enseignant ${i}`, this.state.teachers.some(teacher => teacher.name === name); ++i) {
		}
		
		this.undoStack.actions.push('teachers', new Teacher(name, this.selectedTeachers[0]?.subject ?? this.state.subjects[0], []));
	}
	
	deleteTeacher() {
		const teacher = this.selectedTeachers[0];
		const index = this.state.teachers.indexOf(teacher);
		
		this.undoStack.actions.splice('teachers', index);
		this.selectedTeachers = this.state.teachers.length > 0 ? [this.state.teachers[Math.max(0, index - 1)]] : [];
	}
	
	updateSelectedTeacher() {
		const index = this.state.teachers.indexOf(this.selectedTeachers[0]);
		if (index === -1) {
			this.selectedTeachers = this.state.teachers.length > 0 ? [this.state.teachers[this.state.teachers.length - 1]] : [];
		}
	}
}
