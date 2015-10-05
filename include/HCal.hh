#ifndef HCAL_HH
#define HCAL_HH

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <set>

class HCal : public sf::Drawable, public sf::Transformable {

private:
  float displayx, displayy;
  sf::Vector2f center_display;

  // Module Properties
  float mylar;
  sf::Vector2f size;
  float minx, maxx, miny, maxy;
  int count_modules, count_nodes;
  int maxclustersize;
  float hcal_x, hcal_y;
  int maxrow, maxcol;

  sf::RectangleShape temp;
  std::vector<sf::RectangleShape> modules;
  std::vector<sf::RectangleShape>::iterator modit;
  std::map<int,sf::RectangleShape> modmap, cluster, final;
  std::map<int,sf::RectangleShape>::iterator mapit, clustit, clusterit, lastone;

  std::vector<std::map<int,sf::RectangleShape> > global_logic;
  std::vector<std::map<int,sf::RectangleShape> >::iterator glit, glit_rest;

  // Node Properties
  float nodeR;
  int increment;
  sf::CircleShape node;
  std::vector<sf::CircleShape> nodes;
  std::vector<sf::CircleShape>::iterator nodit;
  std::vector<sf::Text> textnodes;
  sf::Font font;
  sf::Text textind;

  // Boarders
  std::vector<sf::Color> colors, boardercolors;
  sf::VertexArray lines;
  std::vector<sf::VertexArray> boarderthelogic;
  std::vector<std::vector<sf::VertexArray> > manyboarders;

public:
  HCal(float,float);
  ~HCal() {};
  
  void draw(sf::RenderTarget&, sf::RenderStates) const;
  void initializeHCal();
  void triggerlogic();
  void colorthelogic();
  void logicinfo();
  void indexnodes();

};
#endif
