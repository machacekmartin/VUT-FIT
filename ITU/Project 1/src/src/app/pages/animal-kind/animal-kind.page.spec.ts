import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { IonicModule } from '@ionic/angular';

import { AnimalKindPage } from './animal-kind.page';

describe('AnimalKindPage', () => {
  let component: AnimalKindPage;
  let fixture: ComponentFixture<AnimalKindPage>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ AnimalKindPage ],
      imports: [IonicModule.forRoot()]
    }).compileComponents();

    fixture = TestBed.createComponent(AnimalKindPage);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }));

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
