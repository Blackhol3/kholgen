import { Directive, HostListener, Input } from '@angular/core';

@Directive({
	selector: '[appCopyData]'
})
export class CopyDataDirective {
	@Input('appCopyData') data: any;
	@Input('appCopyType') type: string | undefined;
	
	constructor() { }
	
	@HostListener('copy', ['$event'])
	@HostListener('cut', ['$event'])
	protected copy($event: ClipboardEvent) {
		const type = 'application/json' + (this.type === undefined ? '' : ('-' + this.type));
		$event.clipboardData?.setData(type, JSON.stringify(this.data));
		$event.preventDefault();
	}
}
