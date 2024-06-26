import { ActivatedRouteSnapshot, type DetachedRouteHandle, RouteReuseStrategy } from '@angular/router';

/** @link https://stackoverflow.com/questions/41280471/how-to-implement-routereusestrategy-shoulddetach-for-specific-routes-in-angular/44854611#44854611 */
export class ReuseStrategy implements RouteReuseStrategy {
	storedRouteHandles = new Map<string, DetachedRouteHandle>();
	
	shouldDetach(): boolean {
		return true;
	}
	
	store(route: ActivatedRouteSnapshot, handle: DetachedRouteHandle): void {
		if (route.routeConfig?.path) {
			this.storedRouteHandles.set(route.routeConfig.path, handle);
		}
	}
	
	shouldAttach(route: ActivatedRouteSnapshot): boolean {
		return route.routeConfig?.path ? this.storedRouteHandles.has(route.routeConfig.path) : false;
	}

	retrieve(route: ActivatedRouteSnapshot): DetachedRouteHandle {
		return this.storedRouteHandles.get(route.routeConfig!.path as string) as DetachedRouteHandle;
	}
	
	shouldReuseRoute(future: ActivatedRouteSnapshot, curr: ActivatedRouteSnapshot): boolean {
		return future.routeConfig === curr.routeConfig;
	}
}
