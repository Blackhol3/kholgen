import { ComponentFixture, TestBed } from '@angular/core/testing';

import { UniqueIntegersChipInputComponent } from './unique-integers-chip-input.component';

describe('UniqueIntegersChipInputComponent', () => {
  let component: UniqueIntegersChipInputComponent;
  let fixture: ComponentFixture<UniqueIntegersChipInputComponent>;

  beforeEach(() => {
    TestBed.configureTestingModule({
      declarations: [UniqueIntegersChipInputComponent]
    });
    fixture = TestBed.createComponent(UniqueIntegersChipInputComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
