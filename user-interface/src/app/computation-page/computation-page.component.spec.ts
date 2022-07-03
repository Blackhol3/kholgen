import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ComputationPageComponent } from './computation-page.component';

describe('ComputationPageComponent', () => {
  let component: ComputationPageComponent;
  let fixture: ComponentFixture<ComputationPageComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ ComputationPageComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(ComputationPageComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
