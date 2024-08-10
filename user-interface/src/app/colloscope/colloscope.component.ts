import { ChangeDetectionStrategy, ChangeDetectorRef, Component, type OnDestroy, type OnInit, inject } from '@angular/core';
import { MatTableModule } from '@angular/material/table';

import { Subscription } from 'rxjs';

import { Computation } from '../computation';
import { type Subject } from '../subject';
import { type Teacher } from '../teacher';
import { type Timeslot } from '../timeslot';
import { type Trio } from '../trio';
import { type Week } from '../week';

import { StoreService } from '../store.service';

type TableRow = {
	subject: Subject,
	teacher: Teacher,
	timeslot: Timeslot,
	triosByWeek: (Trio | null)[],
};

@Component({
	selector: 'app-colloscope',
	templateUrl: './colloscope.component.html',
	styleUrls: ['./colloscope.component.scss'],
	changeDetection: ChangeDetectionStrategy.OnPush,
	standalone: true,
	imports: [MatTableModule],
})
export class ColloscopeComponent implements OnInit, OnDestroy {
	readonly store = inject(StoreService);
	protected readonly changeDetectorRef = inject(ChangeDetectorRef);

	storeSubscription: Subscription | undefined;
	computation = new Computation();

	tableData: TableRow[] = [];
	tableWeeksHeaderRowDef: string[] = [];
	tableSubjectRowspan: number[] = [];
	tableTeacherRowspan: number[] = [];

	ngOnInit() {
		this.storeSubscription = this.store.changeObservable.subscribe(() => this.update());
		this.update();
	}
	
	ngOnDestroy() {
		this.storeSubscription?.unsubscribe();
	}

	protected update() {
		this.computation = this.store.state.computation ?? this.store.state.prepareComputation();
		this.computeTableData();
		this.changeDetectorRef.markForCheck();
	}
	
	protected computeTableData() {
		this.tableData = [];
		for (const subject of this.computation.subjects) {
			for (const teacher of this.computation.teachers.filter(t => t.subjectId === subject.id)) {
				for (const timeslot of teacher.availableTimeslots) {
					const triosByWeek = [];
					for (const week of this.computation.calendar.getWorkingWeeks()) {
						triosByWeek.push(this.getTrio(teacher, timeslot, week));
					}
					
					if (this.computation.colles.length === 0 || triosByWeek.some(trio => trio !== null)) {
						this.tableData.push({
							subject: subject,
							teacher: teacher,
							timeslot: timeslot,
							triosByWeek: triosByWeek,
						});
					}
				}
			}
		}
		
		this.tableWeeksHeaderRowDef = this.computation.calendar.getWorkingWeeks().map(week => 'week-' + week.id);
		
		this.tableSubjectRowspan = this.getRowspanArray('subject');
		this.tableTeacherRowspan = this.getRowspanArray('teacher');
	}
	
	protected getTrio(teacher: Teacher, timeslot: Timeslot, week: Week): Trio | null {
		for (const colle of this.computation.colles) {
			if (colle.teacherId === teacher.id && colle.timeslot.isEqual(timeslot) && colle.weekId === week.id) {
				return this.computation.trios.find(trio => trio.id === colle.trioId)!;
			}
		}
		
		return null;
	}
	
	protected getRowspanArray(key: keyof TableRow): number[] {
		const array = [1];
		let lastIndex = 0;
		for (let i = 1; i < this.tableData.length; ++i) {
			if (this.tableData[i][key] === this.tableData[lastIndex][key]) {
				++array[lastIndex];
				array.push(0);
			}
			else {
				lastIndex = i;
				array.push(1);
			}
		}
		
		return array;
	}
}
