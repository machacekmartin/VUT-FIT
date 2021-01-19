/* 
  Author: Vaclav Trampeska (xtramp00)
  Service: This service functions as a place to get animals from. 
  Like Service events, it serves mostly as a "database", 
  but it also has functions to recognize currently viewed animal, get last clicked animal..
*/

import { Injectable } from '@angular/core';

export interface AnimalKind{
    type: string,
    image: string,
    origin: string,
    grownweight: string,
    feeding: string,
    temper: string,
    story: string,
    animals: Array<Animal>,
    audio: string,
} 
export interface Animal{
  name: string,
  age: number,
  gender: string,
  image: string,
  audio: string,
}

@Injectable({
  providedIn: 'root'
})
export class AnimalsService {

  constructor() { }

  private lastClicked;
  private animals:Array<AnimalKind> = [
    { 
      type: "Lions", 
      image: "assets/images/lions/lion.png", 
      origin: "Africa",
      grownweight: "130kg",
      feeding: "2 PM",
      temper: "Agressive",
      story:  "It is a long established fact"+ 
              "that a reader will be distracted"+
              "by the readable content of a page when" +
              "looking at its layout. The point of using"+
              "Lorem Ipsum is that it has a more-or-less"+
              "normal distribution of letters, as opposed"+
              "to using 'Content here, content here'," +
              "making it look like readable English",
      animals: [
        {
          name: "Lionell",
          age: 22,
          gender: "male",
          image: "assets/images/lions/lion-1.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Ariel",
          age: 30,
          gender: "female",
          image: "assets/images/lions/lion-2.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Benroy",
          age: 12,
          gender: "male",
          image: "assets/images/lions/lion-3.png",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Ari",
          age: 3,
          gender: "female",
          image: "assets/images/lions/lion-4.jpg",
          audio: "assets/files/audioguide/short.mp3",
        }
      ],
      audio: "./assets/files/audioguide/lion.mp3",
    },
    { 
      type: "Tigers", 
      image: "assets/images/tigers/tiger.png", 
      origin: "Africa",
      feeding: "4 PM",
      grownweight: "200kg",
      temper: "Passive",
      story:  "Surprisingly Daddy tiger was all"+
              "for going and getting some chocolate."+
              "It would be good for our son to see a"+
              "bit more of the jungle,he replied",
      animals: [
        {
          name: "Sam",
          age: 20,
          gender: "male",
          image: "assets/images/tigers/tiger-1.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Oscar",
          age: 13,
          gender: "male",
          image: "assets/images/tigers/tiger-2.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },
      ],
      audio: "./assets/files/audioguide/tiger.mp3",
    },
    { 
      type: "Giraffes", 
      image: "assets/images/giraffes/giraffe.png", 
      origin: "Africa",
      feeding: "5 PM",
      grownweight: "130kg",
      temper: "Very Friendly",
      story: "From the land of Africa comes a most unusual"+
             "story. A big giraffe came upon a family of very"+
            "happy monkeys that were living in a valley of"+ 
            "fruit trees. The giraffe, being quite tall,"+
            "began eating fruit off the top of one tree"+
            "that was as high as a house.",
      animals: [
        {
          name: "Melmen",
          age: 10,
          gender: "male",
          image: "assets/images/giraffes/giraffe-1.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Bao",
          age: 2,
          gender: "male",
          image: "assets/images/giraffes/giraffe-2.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Spot",
          age: 12,
          gender: "female",
          image: "assets/images/giraffes/giraffe-3.jpg",
          audio: "assets/files/audioguide/short.mp3",
        }
      ],
      audio: "./assets/files/audioguide/giraffe.mp3",
    },
    { 
      type: "Monkeys", 
      image: "assets/images/monkeys/monkey.png",
      origin: "Africa",
      feeding: "1 PM",
      grownweight: "50kg",
      temper: "Depends",
      story: "One day when a monkey was climbing a"+
              "tree in the forest in which he lived,"+
              "he ran a thorn into his tail. Try as"+
              "he would, he could not get it out,"+
              "so he went to a barber in the town"+
              "and said.",
      animals: [
        {
          name: "Grape ape",
          age: 13,
          gender: "male",
          image: "assets/images/monkeys/monkey-1.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Koko",
          age: 2,
          gender: "female",
          image: "assets/images/monkeys/monkey-2.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Clyde",
          age: 6,
          gender: "female",
          image: "assets/images/monkeys/monkey-3.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },

      ],
      audio: "./assets/files/audioguide/monkey.mp3",
    },  
    { 
      type: "Chupacabras", 
      image: "assets/images/chupacabras/chupacabra.png",  
      origin: "Unknown",
      feeding: "9 AM",
      grownweight: "XXkg",
      temper: "Very aggresive",
      story: "Eyewitness sightings have been claimed in"+ 
              "Puerto Rico, and have since been reported"+ 
              "as far north as Maine, and as far south"+ 
              "as Chile, and even being spotted outside"+ 
              "the Americas in countries like Russia and"+ 
              "the Philippines",
      animals: [
        {
          name: "Albert",
          age: 989,
          gender: "male",
          image: "assets/images/chupacabras/chupacabra-1.png",
          audio: "assets/files/audioguide/short.mp3",
        },
      ],
      audio: "./assets/files/audioguide/elephant.mp3",
    },
    { 
      type: "Alpacas", 
      image: "assets/images/alpacas/alpaca.jpg",  
      origin: "Czechia",
      feeding: "11 AM",
      grownweight: "300kg",
      temper: "Very friendly",
      story: "Alpacas (lama pacos) are one of the"+ 
            "most beautiful South American Camelids."+ 
            "Related to the Llama, Guanacos and Vicuña,"+ 
            "Alpacas have been prized for their fibre "+
            "since Pre-Incan times due to the properties"+ 
            "and quality of their hair.",
      animals: [
        {
          name: "Chewpaca",
          age: 12,
          gender: "male",
          image: "assets/images/alpacas/alpaca-1.png",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Alpacachino",
          age: 6,
          gender: "female",
          image: "assets/images/alpacas/alpaca-2.png",
          audio: "assets/files/audioguide/short.mp3",
        },
        {
          name: "Avocado",
          age: 3,
          gender: "female",
          image: "assets/images/alpacas/alpaca-3.jpg",
          audio: "assets/files/audioguide/short.mp3",
        },
      ],
      audio: "./assets/files/audioguide/elephant.mp3",
    }
  ];
  
  public getAllAnimalKinds(){
    return this.animals;
  }
  public getKind(type:any){
    let animalKind = this.animals.find(i => i.type === type);
    return animalKind;
  }
  public getIndexOfKind(type:any){
    let index = this.animals.findIndex(x => x.type === type);
    return index;
  }
  public getKindByIndex(index:any){
    console.log(this.animals[index]);
    return this.animals[index];
  }
  public setLastClicked(last:any){
    console.log("setting last clicked to:", last);
    this.lastClicked = last;
  }
  public getLastClicked(){
    return this.lastClicked;
  }
  public getCurrentAnimal(){
    console.log("Getting current animal by searching last clicked");
    let currentAnimal = this.getAllAnimalKinds().find(i => i.type === this.getLastClicked());
    console.log("Found current animal!", currentAnimal.type);
    return currentAnimal;  
  }
  


}
