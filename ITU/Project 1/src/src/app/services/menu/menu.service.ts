/* 
  Author: Martin Machacek (xmacha73)
  Service: This service exists only to control menu state, toggle, open and close.
*/

import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class MenuService {

  isMenuOpen = false;

  constructor() { }

  toggleMenu(){
    this.isMenuOpen = !this.isMenuOpen;
  }
  getIsMenuOpen(){
    return this.isMenuOpen;
  }
}
