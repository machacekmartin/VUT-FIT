import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { AnimalKindPage } from './animal-kind.page';

const routes: Routes = [
  {
    path: '',
    component: AnimalKindPage
  }
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule],
})
export class AnimalKindPageRoutingModule {}
