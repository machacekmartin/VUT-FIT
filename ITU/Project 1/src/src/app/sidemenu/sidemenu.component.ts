/* 
  Author: Martin Machacek (xmacha73)
  Component: This component contains a menu, which is opened from the home page, 
  it doesnt need any service usage, it only need toggle for closing/opening
*/


import { Component, OnInit } from '@angular/core';
import { MenuService } from '../services/menu/menu.service';

@Component({
  selector: 'app-sidemenu',
  templateUrl: './sidemenu.component.html',
  styleUrls: ['./sidemenu.component.scss'],
})
export class SidemenuComponent implements OnInit {

  constructor(private menuService: MenuService) { }

  ngOnInit() {}

  toggleMenu(){
    this.menuService.toggleMenu();
  }
}
