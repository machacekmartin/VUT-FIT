import { async, ComponentFixture, TestBed } from '@angular/core/testing';
import { IonicModule } from '@ionic/angular';

import { AudioguidePage } from './audioguide.page';

describe('AudioguidePage', () => {
  let component: AudioguidePage;
  let fixture: ComponentFixture<AudioguidePage>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ AudioguidePage ],
      imports: [IonicModule.forRoot()]
    }).compileComponents();

    fixture = TestBed.createComponent(AudioguidePage);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }));

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
