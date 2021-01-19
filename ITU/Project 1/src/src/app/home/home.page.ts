/* 
  Author: Martin Machacek (xmacha73)
  Component: This component serves as a homepage for the app. It collects information about animals from AnimalsService,
  provides containers for menu component for nice "animated feel".
*/


import { Component } from '@angular/core';
import { AnimalsService } from '../services/animals/animals.service';
import { PlayerService } from '../services/player/player.service';
import { MenuService } from '../services/menu/menu.service';
import { EventsService } from '../services/events/events.service';

@Component({
  selector: 'home',
  templateUrl: 'home.page.html',
  styleUrls: ['home.page.scss'],
})

export class HomePage{

  constructor( public animalsService: AnimalsService,
               public playerService: PlayerService,
               public menuService: MenuService,
               public eventsService: EventsService){}

  threeAnimalKinds = this.animalsService.getAllAnimalKinds().slice(0, 3);
  threeEvents = this.eventsService.getEvents().slice(0, 3);

  isAnimals = true;
  isEvents = false;

  changeView($event){
    this.isAnimals = false;
    this.isEvents = false;

    switch($event){
      case 'animals':{
        this.isAnimals = true;
        break;
      }
      case 'events':{
        this.isEvents = true;
        break;
      }
      default :{
        break;
      }
    }
  }
  setAnimalKind(kind:any){
    this.animalsService.setLastClicked(kind);
  }
  toggleMenu(){
    this.menuService.toggleMenu();
  }
  getIsMenuOpen(){
    return this.menuService.getIsMenuOpen();
  }
  getEvents(){
    return this.eventsService.getEvents();
  }

}




