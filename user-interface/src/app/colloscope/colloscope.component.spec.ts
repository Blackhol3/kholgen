import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ColloscopeComponent } from './colloscope.component';

describe('ColloscopeComponent', () => {
  let component: ColloscopeComponent;
  let fixture: ComponentFixture<ColloscopeComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ ColloscopeComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(ColloscopeComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
