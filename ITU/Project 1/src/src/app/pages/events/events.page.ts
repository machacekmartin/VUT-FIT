/* 
  Author: David Gajdos (xgajdo22)
  Component: This component is a page for displaying all events in the zoo, 
  uses EvetsService to get it.
*/

import { Component, OnInit } from '@angular/core';
import { EventsService } from '../../services/events/events.service';
import { PlayerService } from '../../services/player/player.service';

@Component({
  selector: 'app-events',
  templateUrl: './events.page.html',
  styleUrls: ['./events.page.scss'],
})
export class EventsPage implements OnInit {

  events;

  constructor(public eventsService: EventsService,
            public playerService:PlayerService) { }

  ngOnInit() {
    this.getEvents();
  }

  getEvents(){
    this.events = this.eventsService.getEvents();
  }
  setLastClickedEvent(eventName){
    console.log("AWDJNADWNDWDJWJKDWJKNDW");
    this.eventsService.setLastClickedEvent(eventName);
  }

}
