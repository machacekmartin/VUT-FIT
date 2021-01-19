/* 
  Author: Vaclav Trampeska (xtramp00)
  Component: This component is a page for displaying all animals in the zoo.
  uses AnimalService to get it.
*/

import { Component, OnInit } from '@angular/core';
import { AnimalsService } from '../../services/animals/animals.service';
import { PlayerService } from '../../services/player/player.service';

@Component({
  selector: 'app-animals',
  templateUrl: './animals.page.html',
  styleUrls: ['./animals.page.scss'],
})
export class AnimalsPage implements OnInit {

    animals = [];
    constructor(public animalsService: AnimalsService,
                public playerService: PlayerService){ }  

    ngOnInit() {
      this.animals = this.animalsService.getAllAnimalKinds();
    }
    setAnimalKind(kind:any){
      this.animalsService.setLastClicked(kind);
    }
}