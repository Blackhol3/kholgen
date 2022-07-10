import { animate, query, stagger, style, transition, trigger } from '@angular/animations';

/** @link https://ultimatecourses.com/blog/angular-animations-how-to-animate-lists */
export const listAnimation = trigger('listAnimation', [
	transition('* <=> *', [
		query(':enter', [
			style({opacity: 0}),
			stagger('50ms', animate('200ms', style({opacity: 1})))
		], {optional: true}),
		query(':leave', [
			stagger('50ms', animate('150ms', style({opacity: 0})))
		], {optional: true}),
	]),
]);

/**
 * @bug Float property isn't set by Angular animation, so we set it manually
 * @see app.component.scss
 */
export const slideAnimation = trigger('slideAnimation', [
	transition(':enter', [
		query(':scope > *', [
			style({width: '*', /* float: 'right', */}),
		]),
		style({width: 0, minWidth: 0}),
		animate('200ms', style({width: '*', minWidth: 0})),
	]),
	transition(':leave', [
		query(':scope > *', [
			style({width: '*', /* float: 'right', */}),
		]),
		style({width: '*', minWidth: 0}),
		animate('150ms', style({width: 0, minWidth: 0})),
	]),
]);
