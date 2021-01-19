/* 
  This component was not used, was scrapped due to users' testing results, but still remains here, for some reason
*/


import { Component, OnInit } from '@angular/core';
import { Location } from '@angular/common';
import { AnimalsService } from '../../services/animals/animals.service';
import { PlayerService } from '../../services/player/player.service';


@Component({
  selector: 'app-audioguide',
  templateUrl: './audioguide.page.html',
  styleUrls: ['./audioguide.page.scss'],
})
export class AudioguidePage implements OnInit {

  animalKind = {};
  animalKinds = [];

  constructor(private _location: Location, 
              public animalsService: AnimalsService,
              public playerService: PlayerService) { }

  ngOnInit() {
    this.animalKind = this.animalsService.getCurrentAnimal();  
    this.animalKinds = this.animalsService.getAllAnimalKinds();
    let audio = {about: this.animalKind['type'], path: this.animalKind['audio']};
  
    this.playerService.play(audio);  
  }

  public togglePlay(){
    this.playerService.togglePlayer();
  }
  
  public goBack(){
    this._location.back();
  }

  public isPlaying(){
    return this.playerService.getIsPlaying();
  }

}
