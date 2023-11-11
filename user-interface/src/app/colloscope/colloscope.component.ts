import { Component } from '@angular/core';
import { MatTableModule } from '@angular/material/table';

import { Teacher } from '../teacher';
import { Timeslot } from '../timeslot';
import { Trio } from '../trio';
import { Week } from '../week';

import { StoreService } from '../store.service';

@Component({
	selector: 'app-colloscope',
	templateUrl: './colloscope.component.html',
	styleUrls: ['./colloscope.component.scss'],
	standalone: true,
	imports: [MatTableModule],
})
export class ColloscopeComponent {
	tableData: any[] = [];
	tableWeeksHeaderRowDef: string[] = [];
	tableSubjectRowspan: number[] = [];
	tableTeacherRowspan: number[] = [];
	
	constructor(public store: StoreService) { }
	
	getTableData() {
		this.tableData = [];
		for (let subject of this.store.state.subjects) {
			for (let teacher of this.store.state.teachers.filter(t => t.subjectId === subject.id)) {
				for (let timeslot of teacher.availableTimeslots) {
					let triosByWeek = [];
					for (let week of this.store.state.weeks) {
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
		
		this.tableWeeksHeaderRowDef = this.store.state.weeks.map(week => 'week-' + week.id);
		
		this.tableSubjectRowspan = this.getRowspanArray('subject');
		this.tableTeacherRowspan = this.getRowspanArray('teacher');
		
		return this.tableData;
	}
	
	getTrio(teacher: Teacher, timeslot: Timeslot, week: Week): Trio | null {
		for (let colle of this.store.state.colles) {
			if (colle.teacherId === teacher.id && colle.timeslot.isEqual(timeslot) && colle.weekId === week.id) {
				return this.store.state.findId('trios', colle.trioId)!;
			}
		}
		
		return null;
	}
	
	getRowspanArray(key: any): number[] {
		let array = [1];
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
