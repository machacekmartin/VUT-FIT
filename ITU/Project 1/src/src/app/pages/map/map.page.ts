/* 
  Author: Vaclav Trampeska (xtramp00)
  Component: This component is a page with map, 
  this map will have options to display animals, restaurants, sights, and events on the map
*/


import { Component, OnInit } from '@angular/core';
import { AnimalsService } from '../../services/animals/animals.service';
import { PlayerService } from '../../services/player/player.service';
import { Location } from '@angular/common';

@Component({
  selector: 'app-map',
  templateUrl: './map.page.html',
  styleUrls: ['./map.page.scss'],
})
export class MapPage implements OnInit {


  public animalKinds;

  constructor(private _location: Location, 
              public animalsService: AnimalsService, 
              public playerService:PlayerService) { }

  ngOnInit() {
    this.animalKinds = this.animalsService.getAllAnimalKinds();
  }

  public goBack(){
    this._location.back();
  }

  playAudioByKind(kind:any, path:any){
    let track = {about:kind, path:path};
    this.playerService.play(track);
  }

  isThisKindActive(kind:any, path:any){
    let track = {about:kind, path:path};
    return this.playerService.areTracksEqual(track);
  }

  isPlayingAudio(){
    return this.playerService.getIsPlaying();
  }

  isPlayerReady(){
    return this.playerService.isPlayerReady();
  }

  toggleAudio(){
    this.playerService.togglePlayer();
  }
  setKind(kind:any){
    this.animalsService.getKind(this.playerService.getRecentTrack().about);
  }

}
