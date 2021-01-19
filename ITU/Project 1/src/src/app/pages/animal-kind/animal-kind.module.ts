import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { IonicModule } from '@ionic/angular';

import { AnimalKindPageRoutingModule } from './animal-kind-routing.module';

import { AnimalKindPage } from './animal-kind.page';

@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    IonicModule,
    AnimalKindPageRoutingModule
  ],
  declarations: [AnimalKindPage]
})
export class AnimalKindPageModule {}
