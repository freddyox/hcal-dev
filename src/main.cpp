//    ************************************************************
//    *                           HCal                           *
//    *                         OCt 2015                         *
//    ************************************************************
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

#include "../include/HCal.hh"

const float gDisplayx = 1900;
const float gDisplayy = 5000;
int main() {
  //GAME SETUP
  sf::RenderWindow window(sf::VideoMode(gDisplayx,gDisplayy), "HCal Model");
  window.setFramerateLimit(60);

  // Handling Time 
  sf::Clock clock;
  float time;
  
  // Handling Camera View
  sf::View view(sf::FloatRect(0.5*gDisplayx, 0.5*gDisplayy, gDisplayx, 1200) );
  view.setCenter( 0.5*gDisplayx, 500 );
  int mouseDelta = 0;

  // INITIALIZE HCal
  HCal hcal( window.getSize().x, window.getSize().y );
  hcal.initializeHCal();
  hcal.triggerlogic();
  hcal.colorthelogic();
  hcal.logicinfo();
  hcal.indexnodes();

  while( window.isOpen() ) {
    sf::Event event;
    while( window.pollEvent(event) ) {
      if( event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ) {
	window.close();
      }
    }
    // if( !hcal.onoroff() ) 
    //   window.clear(sf::Color(220,220,220));
    // else
       window.clear(sf::Color(220,220,220));
    
    window.setView( view );

    // UPDATING
    sf::Time elapsed = clock.restart();
    //ecal.controldrawings(elapsed);
    // UPDATING CAMERA
    if( sf::Mouse::isButtonPressed(sf::Mouse::Left) ) {
      view.zoom( 0.95 );
    }
    if( sf::Mouse::isButtonPressed(sf::Mouse::Right) ) {
      view.zoom( 1.05 );
    }
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ) {
      view.move( 0, 10 );
    }
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
      view.move( 0, -10 );
    }
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ) {
      view.move( 10, 0 );
    }
    if( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ) {
      view.move( -10, -0 );
    }
    
    // DRAWINGS
    window.draw(hcal);      
    window.display();   
  }
  return 0;
}
