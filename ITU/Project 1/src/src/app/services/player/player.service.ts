/* 
  Author: David Gajdos (xgajdo22)
  Service: This service is a component which provides a "hub" for player controls.
  Other components use this service to f.e. pause, play, skip audio, via usage of functions bellow - f.e. play(), togglePlayer()..
*/


import { Injectable } from '@angular/core';
import { Howl } from 'howler';

export interface Track{
    about: string,
    path: string,
}

@Injectable({
  providedIn: 'root'
})
export class PlayerService {

  private activeTrack:Track = null;
  private player: Howl = null;
  private shortPlayer: Howl = null;
  private isPlaying = false;
  private isReady = false;

  constructor() { }

  play(track:Track){
    if (this.player){
        this.player.stop();
    }
    this.player = new Howl({
      src: [track.path],
      html5: true,
    });
    this.player.play();
    this.activeTrack = track; 
    console.log("Playing.. track from: ", track.about, ", path: ", track.path);
    this.isPlaying = true;
    this.isReady = true;
  }
  togglePlayer(){
    if (this.isPlaying){
      this.isPlaying = false;
      this.player.pause();
    }
    else{
      this.isPlaying = true;
      this.player.play();
    }
  }
  stopPlaying(){
    this.player.stop();
    this.player = null;
    this.activeTrack = null;
    this.isPlaying = false;
    this.isReady = false;
  }
  getIsPlaying(){
    return this.isPlaying;
  }
  getRecentTrack(){
    return this.activeTrack;    
  }
  areTracksEqual(track){ 
    if (JSON.stringify(track).toLowerCase() === 
        JSON.stringify(this.getRecentTrack()).toLowerCase()  
        ){
        return true;
    }
    return false
  }
  isPlayerReady(){
    return this.isReady;
  }


  //////// -- SHORT AUDIO -- ////////
  playShort(trackPath:string){
    if (this.shortPlayer){
      this.shortPlayer.stop();
    }
    this.shortPlayer = new Howl({
      src: [trackPath],
      html5: true,
    });
    this.shortPlayer.play();
  }
}
