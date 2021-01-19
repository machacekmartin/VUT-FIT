/* 
  Author: Martin Machacek (xmacha73)
  Component: This functions as a page for user to scan QR code of an animal or a section, 
  to save user time looking for the animal he wants
*/

import { Component, OnInit } from '@angular/core';
import { Location } from '@angular/common';
import { PlayerService } from '../../services/player/player.service';

@Component({
  selector: 'app-scan',
  templateUrl: './scan.page.html',
  styleUrls: ['./scan.page.scss'],
})
export class ScanPage implements OnInit {

  constructor(private _location: Location,
              private playerService: PlayerService) { }

  ngOnInit() {
  }

  public goBack(){
    this._location.back();
  }

}
