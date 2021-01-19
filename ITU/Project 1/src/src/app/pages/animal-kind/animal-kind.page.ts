/* 
  Author: Vaclav Trampeska (xtramp00)
  Component: This component is a page for displaying currently clicked animal.
  This page also allow user to play animal's audioguide, which uses PlayerService and it's functions to play it
*/


import { Component, OnInit } from '@angular/core';
import { Location } from '@angular/common';
import { AnimalsService } from '../../services/animals/animals.service';
import { PlayerService } from 'src/app/services/player/player.service';

@Component({
  selector: 'app-animal-kind',
  templateUrl: './animal-kind.page.html',
  styleUrls: ['./animal-kind.page.scss'],
})
export class AnimalKindPage implements OnInit {

  animalKind = {};
  track;
  
  constructor(private _location: Location, 
              public animalsService: AnimalsService,
              public playerService: PlayerService) { }

  ngOnInit() {
    this.animalKind = this.animalsService.getCurrentAnimal();   
    this.track = {about:this.animalKind['type'], path: this.animalKind['audio']};

    console.log(this.animalKind);
  }

  public goBack(){
    this._location.back();
  }

  
  areTracksEqual(){
    return this.playerService.areTracksEqual(this.track)
  }
  playAudio(){
    this.playerService.play(this.track);
  }
  toggleAudio(){
    this.playerService.togglePlayer();
  }
  isPlayingAudio(){
    return this.playerService.getIsPlaying();
  }

  playShortAudio(track: string){
    this.playerService.playShort(track);

  }



  

}
