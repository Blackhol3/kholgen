import { Component, OnInit, OnDestroy } from '@angular/core';
import { CdkDrag, CdkDragDrop, CdkDropList, moveItemInArray } from '@angular/cdk/drag-drop';
import { castDraft } from 'immer';
import { Subscription } from 'rxjs';

import { listAnimation, slideAnimation } from '../animations';
import { Subject } from '../subject';
import { Teacher } from '../teacher';
import { UndoStackService } from '../undo-stack.service';
import { StoreService } from '../store.service';

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
	selectedTeacherIds: string[] = [];
	storeSubscription: Subscription | undefined;
	
	constructor(public store: StoreService, private undoStack: UndoStackService) { }

	ngOnInit() {
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.updateSelectedTeacher());
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}
	
	canDrop = (draggedTeacher: CdkDrag<Teacher>, droppedSubject: CdkDropList<Subject>): boolean => {
		for (let teacher of this.store.state.teachers) {
			if (teacher.subjectId === droppedSubject.data.id && teacher.name === draggedTeacher.data.name) {
				return false;
			}
		}
		
		return true;
	}
	
	onDrop($event: CdkDragDrop<Subject, Subject, Teacher>) {
		const subject = $event.container.data;
		const teacher = $event.item.data;
		const teachersOfNewSubject = this.store.state.teachers.filter(t => t.subjectId === subject.id);
		
		const indexFrom = this.store.state.teachers.indexOf(teacher);
		const indexBefore = this.store.state.teachers.indexOf(teachersOfNewSubject[$event.currentIndex]);
		const indexTo =
			teachersOfNewSubject.length === 0 ? indexFrom :
			teacher.subjectId === subject.id ? indexBefore :
			indexBefore === -1 ? this.store.state.teachers.length - 1 :
			indexFrom < indexBefore ? indexBefore - 1 :
			indexBefore
		;
		this.selectedTeacherIds = [teacher.id];
		
		this.undoStack.do(state => {
			if (teacher.subjectId !== subject.id) {
				state.teachers[indexFrom].subjectId = subject.id;
			}
			moveItemInArray(state.teachers, indexFrom, indexTo);
		});
	}
	
	addNewTeacher() {
		let name = '';
		for (let i = 1; name = `Enseignant ${i}`, this.store.state.teachers.some(teacher => teacher.name === name); ++i) {
		}
		
		this.undoStack.do(state => { state.teachers.push(castDraft(new Teacher(name, this.store.state.findId('teachers', this.selectedTeacherIds[0])?.subjectId ?? this.store.state.subjects[0].id, []))) });
	}
	
	deleteTeacher() {
		const teacher = this.store.state.findId('teachers', this.selectedTeacherIds[0])!;
		const index = this.store.state.teachers.indexOf(teacher);
		
		this.undoStack.do(state => { state.teachers.splice(index, 1) });
		this.selectedTeacherIds = this.store.state.teachers.length > 0 ? [this.store.state.teachers[Math.max(0, index - 1)].id] : [];
	}
	
	updateSelectedTeacher() {
		if (this.store.state.findId('teachers', this.selectedTeacherIds[0]) === undefined) {
			this.selectedTeacherIds = this.store.state.teachers.length > 0 ? [this.store.state.teachers[this.store.state.teachers.length - 1].id] : [];
		}
	}
	
	onPaste($event: ClipboardEvent) {
		const jsonString = $event.clipboardData?.getData('application/json-teacher') ?? '';
		if (jsonString === '') {
			return;
		}
		
		const jsonTeacher = JSON.parse(jsonString) as ReturnType<Teacher['toHumanJsonObject']>;
		while (this.store.state.teachers.some(teacher => teacher.name === jsonTeacher.name)) {
			jsonTeacher.name += ' (copie)';
		}
		
		if (this.selectedTeacherIds[0] !== undefined) {
			jsonTeacher.subject = this.store.state.findId('subjects', this.store.state.findId('teachers', this.selectedTeacherIds[0])!.subjectId)!.name;
		}
		
		const teacher = Teacher.fromJsonObject(jsonTeacher, this.store.state.subjects);
		if (teacher !== undefined) {
			this.undoStack.do(state => { state.teachers.push(castDraft(teacher)) });
		}
	}
}
