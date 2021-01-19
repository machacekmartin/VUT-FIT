import { Component } from '@angular/core';

import { Platform } from '@ionic/angular';
import { SplashScreen } from '@ionic-native/splash-screen/ngx';
import { StatusBar } from '@ionic-native/status-bar/ngx';

import { PlayerService } from './services/player/player.service';
import { MenuService } from './services/menu/menu.service';
import { Router } from '@angular/router';

@Component({
  selector: 'app-root',
  templateUrl: 'app.component.html',
  styleUrls: ['app.component.scss'],
})
export class AppComponent {
  constructor(
    private platform: Platform,
    private splashScreen: SplashScreen,
    private statusBar: StatusBar,
    private playerService: PlayerService,
    private menuService: MenuService,
    public router: Router,
  ) {
    this.initializeApp();
  }

  initializeApp() {
    this.platform.ready().then(() => {
      this.statusBar.styleDefault();
      this.statusBar.styleBlackTranslucent();
      this.splashScreen.hide();
    });
  }

  getIsMenuOpen(){
    return this.menuService.getIsMenuOpen();
  }
  stopPlaying(){
    this.playerService.stopPlaying();
  }
  getPlayingName(){
    return this.playerService.getRecentTrack().about;
  }
}
