export const enum Day {
	Monday, Tuesday, Wednesday, Thursday, Friday
}

const dayNames = [
	'Lundi', 'Mardi', 'Mercredi', 'Jeudi', 'Vendredi'
];

export class Timeslot {
	constructor(
		readonly day: Day,
		readonly hour: number,
	) {}
	
	isEqual(timeslot: Timeslot): boolean {
		return this.day === timeslot.day && this.hour === timeslot.hour;
	}
	
	toString(): string {
		return `${dayNames[this.day]} à ${this.hour}h`;
	}
}
