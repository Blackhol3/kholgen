import { Directive, input } from '@angular/core';

@Directive({
	selector: '[appEnterLeaveAnimation]',
	host: {
		'[class]': '`${appEnterLeaveAnimation()}-animation`',
		'animate.enter': 'enter',
		'animate.leave': 'leave',
	}
})
export class EnterLeaveAnimationDirective {
	appEnterLeaveAnimation = input.required<string>();
}
