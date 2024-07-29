import { Component } from '@angular/core';
import { MatTableModule } from '@angular/material/table';

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
	standalone: true,
	imports: [MatTableModule],
})
export class ColloscopeComponent {
	tableData: TableRow[] = [];
	tableWeeksHeaderRowDef: string[] = [];
	tableSubjectRowspan: number[] = [];
	tableTeacherRowspan: number[] = [];
	
	constructor(public store: StoreService) { }
	
	getTableData() {
		this.tableData = [];
		for (const subject of this.store.state.subjects) {
			for (const teacher of this.store.state.teachers.filter(t => t.subjectId === subject.id)) {
				for (const timeslot of teacher.availableTimeslots) {
					const triosByWeek = [];
					for (const week of this.store.state.calendar.weeks) {
						triosByWeek.push(this.getTrio(teacher, timeslot, week));
					}
					
					if (this.store.state.colles.length === 0 || triosByWeek.some(trio => trio !== null)) {
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
		
		this.tableWeeksHeaderRowDef = this.store.state.calendar.weeks.map(week => 'week-' + week.id);
		
		this.tableSubjectRowspan = this.getRowspanArray('subject');
		this.tableTeacherRowspan = this.getRowspanArray('teacher');
		
		return this.tableData;
	}
	
	getTrio(teacher: Teacher, timeslot: Timeslot, week: Week): Trio | null {
		for (const colle of this.store.state.colles) {
			if (colle.teacherId === teacher.id && colle.timeslot.isEqual(timeslot) && colle.weekId === week.id) {
				return this.store.state.findId('trios', colle.trioId)!;
			}
		}
		
		return null;
	}
	
	getRowspanArray(key: keyof TableRow): number[] {
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
