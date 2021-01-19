/* 
  Author: David Gajdos (xgajdo22)
  Component: This component is a page to display one event and this event's information.
*/

import { Component, OnInit } from '@angular/core';
import { EventsService } from '../../services/events/events.service';
import { Location } from '@angular/common';
import { PlayerService } from '../../services/player/player.service';

@Component({
  selector: 'app-event',
  templateUrl: './event.page.html',
  styleUrls: ['./event.page.scss'],
})
export class EventPage implements OnInit {

  event;
  constructor(private _location: Location, 
              public eventsService: EventsService,
              public playerService:PlayerService) { }

  ngOnInit() {

    this.event = this.eventsService.getLastClickedEvent();
  }

  public goBack(){
    this._location.back();
  }

}
