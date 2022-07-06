import { TestBed } from '@angular/core/testing';

import { UndoStackService } from './undo-stack.service';

describe('UndoStackService', () => {
  let service: UndoStackService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(UndoStackService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
