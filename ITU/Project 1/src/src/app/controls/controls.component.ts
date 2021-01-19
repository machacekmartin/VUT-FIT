/* 
  Author: Martin Machacek (xmacha73)
  Component: This component is the bottom player panel when audio is playing.
  This component allows user to pause, play next audio, or previous audio, or to stop plying audio, all 
  via usage of functions from PlayerService service.
*/


import { Component, OnInit} from '@angular/core';
import { PlayerService } from '../services/player/player.service';
import { AnimalsService } from '../services/animals/animals.service';


@Component({
  selector: 'app-controls',
  templateUrl: './controls.component.html',
  styleUrls: ['./controls.component.scss'],
  
})
export class ControlsComponent implements OnInit {

  expanded = false;

  constructor(public playerService: PlayerService,
              public animalsService: AnimalsService) { }

  ngOnInit() {
  }

  toggleExpanded(){
    this.expanded = !this.expanded;
  }

  stopPlaying(){
    this.playerService.stopPlaying();
  }
  togglePlaying(){
    this.playerService.togglePlayer();
  }
  playNext(){
    let currentlyPlaying = this.getCurrentKind();
    let currentIndex = this.animalsService.getIndexOfKind(currentlyPlaying.type);
    let nextAudio;
    if (currentIndex === this.animalsService.getAllAnimalKinds().length - 1){
      nextAudio = this.animalsService.getKindByIndex(0);
    }
    else{
      nextAudio = this.animalsService.getKindByIndex(currentIndex + 1);
    }
    let nextTrack = {about: nextAudio.type, path: nextAudio.audio};

    this.playerService.play(nextTrack);
  }
  playPrevious(){
    let currentlyPlaying = this.getCurrentKind();
    let currentIndex = this.animalsService.getIndexOfKind(currentlyPlaying.type);
    let previousAudio;
    if (currentIndex === 0){
      previousAudio = this.animalsService.getKindByIndex(this.animalsService.getAllAnimalKinds().length - 1);
    }
    else{
      previousAudio = this.animalsService.getKindByIndex(currentIndex - 1);
    }
    let previousTrack = {about: previousAudio.type, path: previousAudio.audio};
    this.playerService.play(previousTrack);
  }

  getCurrentKind(){
    return this.animalsService.getKind(this.playerService.getRecentTrack().about);
  }
  isPlaying(){
    return this.playerService.getIsPlaying();
  }


}
