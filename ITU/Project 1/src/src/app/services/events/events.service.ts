/* 
  Author: David Gajdos (xgajdo22)
  Service: This service functions as a place to get events from. 
  Like Service - animals, it serves mostly as a "database".
*/

import { Injectable } from '@angular/core';

export interface Event{
  name: string,
  date: string,
  time: string,
  image: string,
  description: string,
  place: string,
  available_num: number,
}


@Injectable({
  providedIn: 'root'
})
export class EventsService {

  constructor() { }

  lastClickedEvent;
  private events:Array<Event> = [
    {
      name: "Island of Joy",
      date: "12. 3. 2020",
      time: "14:50",
      image: "assets/images/events/1.png",
      description:  "It is a long established fact"+ 
                    "that a reader will be distracted"+
                    "by the readable content of a page when" +
                    "looking at its layout. The point of using"+
                    "Lorem Ipsum is that it has a more-or-less",
      place: "Darkmoon Island",
      available_num: -1,
    },
    {
      name: "Darkmoon",
      date: "15. 3. 2021",
      time: "12:10",
      image: "assets/images/monkeys/monkey.png",
      description:  "It is a long established fact"+ 
                    "that a reader will be distracted"+
                    "by the readable content of a page when" +
                    "looking at its layout. The point of using"+
                    "Lorem Ipsum is that it has a more-or-less",
      place: "Darkmoon Land",
      available_num: 100,
    }
  ];

  getEvents(){
    return this.events;
  }
  setLastClickedEvent(eventName){
    this.lastClickedEvent =  this.events.find(i => i.name === eventName);
  }
  getLastClickedEvent(){
    return this.lastClickedEvent;
  }
}
