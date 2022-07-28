import { Component } from '@angular/core';
import { Teacher } from '../teacher';
import { Timeslot } from '../timeslot';
import { Trio } from '../trio';
import { Week } from '../week';

import { StateService } from '../state.service';

@Component({
	selector: 'app-colloscope',
	templateUrl: './colloscope.component.html',
	styleUrls: ['./colloscope.component.scss']
})
export class ColloscopeComponent {
	tableData: any[] = [];
	tableWeeksHeaderRowDef: string[] = [];
	tableSubjectRowspan: number[] = [];
	tableTeacherRowspan: number[] = [];
	
	constructor(public state: StateService) { }
	
	getTableData() {
		this.tableData = [];
		for (let subject of this.state.subjects) {
			for (let teacher of this.state.teachers.filter(t => t.subject === subject)) {
				for (let timeslot of teacher.availableTimeslots) {
					let triosByWeek = [];
					for (let week of this.state.weeks) {
						triosByWeek.push(this.getTrio(teacher, timeslot, week));
					}
					
					if (this.state.colles.length === 0 || triosByWeek.some(trio => trio !== null)) {
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
		
		this.tableWeeksHeaderRowDef = this.state.weeks.map(week => 'week-' + week.id);
		
		this.tableSubjectRowspan = this.getRowspanArray('subject');
		this.tableTeacherRowspan = this.getRowspanArray('teacher');
		
		return this.tableData;
	}
	
	getTrio(teacher: Teacher, timeslot: Timeslot, week: Week): Trio | null {
		for (let colle of this.state.colles) {
			if (colle.teacher === teacher && colle.timeslot.isEqual(timeslot) && colle.week === week) {
				return colle.trio;
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
