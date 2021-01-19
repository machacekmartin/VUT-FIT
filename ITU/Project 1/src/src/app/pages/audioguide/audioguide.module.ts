import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { IonicModule } from '@ionic/angular';

import { AudioguidePageRoutingModule } from './audioguide-routing.module';

import { AudioguidePage } from './audioguide.page';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    IonicModule,
    AudioguidePageRoutingModule
  ],
  declarations: [AudioguidePage]
})
export class AudioguidePageModule {}
