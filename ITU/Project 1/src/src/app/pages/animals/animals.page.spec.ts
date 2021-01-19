import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { IonicModule } from '@ionic/angular';

import { AnimalsPage } from './animals.page';

describe('AnimalsPage', () => {
  let component: AnimalsPage;
  let fixture: ComponentFixture<AnimalsPage>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ AnimalsPage ],
      imports: [IonicModule.forRoot()]
    }).compileComponents();

    fixture = TestBed.createComponent(AnimalsPage);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }));

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
