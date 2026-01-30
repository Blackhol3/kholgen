import { Component } from '@angular/core';
import { ComponentFixture, TestBed } from '@angular/core/testing';

import { EnterLeaveAnimationDirective } from './enter-leave-animation.directive';

@Component({
	imports: [EnterLeaveAnimationDirective],
	template: `<div appEnterLeaveAnimation="name"></div>`,
})
class TestComponent {}

describe('EnterLeaveAnimationDirective', () => {
	let fixture: ComponentFixture<TestComponent>;

	beforeEach(async () => {
		fixture = TestBed.createComponent(TestComponent);
		await fixture.whenStable();
	});

	it('should add the appropriate class', () => {
		expect(fixture.debugElement.children[0].classes['name-animation']).toBe(true);
	});
});
