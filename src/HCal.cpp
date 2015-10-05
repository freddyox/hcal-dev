#include "../include/HCal.hh"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
// #include <iterator>
// #include <algorithm>
#include <iomanip>

HCal::HCal(float x, float y){
  displayx = x;
  displayy = y;

  center_display = sf::Vector2f( displayx/2.0, displayy/2.0 );

  // All units are in mm unless otherwise stated
  mylar  = 5.0;
  size = sf::Vector2f( 153, 153 );
  count_modules = 0;
  minx = 0;
  maxx = 0;
  miny = 0;
  maxy = 0;
  maxrow = 24;
  maxcol = 11;

  // Module Setup
  temp.setSize( size );
  temp.setOrigin( 0.5*size.x, 0.5*size.y );
  temp.setFillColor( sf::Color(166,176,16) );
  temp.setOutlineThickness( -mylar );
  temp.setOutlineColor( sf::Color::Black );

  // Node Setup
  count_nodes = 0;
  maxclustersize = 64;
  increment = 4.0*size.x; 
  nodeR = 25.0;
  node.setRadius( nodeR );
  sf::FloatRect origin = node.getLocalBounds();
  node.setOrigin(0.5*origin.width,0.5*origin.height);
  node.setFillColor( sf::Color(36,23,115) );

  // Handle text indices on nodes
  if( !font.loadFromFile("fonts/arial.ttf")) {
    std::cerr << "ERROR: Font did not load properly." << std::endl;
  }
  textind.setFont(font);
  textind.setCharacterSize( 15 );
  textind.setColor( sf::Color::White );


  // Initialize color vectors
  sf::Color red = sf::Color(51,0,0);
  sf::Color yellow = sf::Color(51,51,0);
  sf::Color blue = sf::Color(0,0,51);
  sf::Color green = sf::Color(0,51,0);
  sf::Color cyan = sf::Color(0,51,51);
  sf::Color magenta = sf::Color(51,0,51);
  sf::Color dontknow = sf::Color(18,20,25);
  colors.push_back( dontknow );
  colors.push_back( yellow );
  colors.push_back( green );
  colors.push_back( cyan );
  colors.push_back( red );
  colors.push_back( blue );
  colors.push_back( green );
 
  sf::Color red_sfml = sf::Color::Red;
  sf::Color yellow_sfml = sf::Color::Yellow;
  sf::Color blue_sfml = sf::Color::Blue;
  sf::Color green_sfml = sf::Color::Green;
  sf::Color cyan_sfml = sf::Color::Cyan;
  sf::Color magenta_sfml = sf::Color::Magenta; 
  sf::Color orange_sfml = sf::Color(255,128,0);
  sf::Color lightgreen_sfml = sf::Color(24,218,114);
  sf::Color purple_sfml = sf::Color(111,16,195);
  sf::Color pink_sfml = sf::Color(232,78,238);
  sf::Color lightblue_sfml = sf::Color(8,122,164);
  sf::Color lightpink_sfml = sf::Color(242,124,210);
  boardercolors.push_back( lightblue_sfml );
  boardercolors.push_back( yellow_sfml );
  boardercolors.push_back( pink_sfml );
  boardercolors.push_back( green_sfml );
  boardercolors.push_back( purple_sfml );
  boardercolors.push_back( orange_sfml );
  boardercolors.push_back( cyan_sfml );
  boardercolors.push_back( lightpink_sfml );
  boardercolors.push_back( lightgreen_sfml );
  boardercolors.push_back( red_sfml );
  boardercolors.push_back( blue_sfml );
  boardercolors.push_back( green_sfml );
  boardercolors.push_back( magenta_sfml );
}

void HCal::initializeHCal() {
  std::ifstream layout;
  layout.open("hcal_layout1.txt");
  std::string line;
  
  if( layout.is_open() ) {
    while( getline(layout,line) ) {
      if( line[0] != '#' ) {
	int cell, x, y, row, col;
	std::stringstream first(line);
	first >> cell >> x >> y >> row >> col;
	sf::Vector2f cellposition( 0.5*displayx + float(x), 0.5*displayy + -1*float(y) );

	temp.setPosition( cellposition );
	modules.push_back( temp );
	modmap[cell] = temp;
	count_modules++;
      }
    }
  }
  else std::cerr << "Error opening ecal_layout.txt" << std::endl;
  layout.close();

  hcal_x = maxcol * size.x;
  hcal_y = maxrow * size.y;
  sf::Vector2f hcal_size_half( hcal_x/2.0, hcal_y/2.0 );
  // Make Nodes
  float totalX = hcal_x;
  int numberX = int(totalX)/increment + 1;
  float totalY = hcal_y;
  int numberY = int(totalY)/increment + 1;
  sf::Vector2f start = center_display - hcal_size_half;

  for( int row=0; row<numberY; row++ ) {
    for( int col=0; col<numberX; col++ ) {
      sf::Vector2f tempnode(start.x + col*increment, start.y + row*increment );
      node.setPosition( tempnode );
      nodes.push_back( node );
    }
  }
}

void HCal::triggerlogic() {

  for( int i=0; i<nodes.size(); i++ ) {
    if( i%1==0 ) {
      sf::Vector2f nodetemp = nodes[i].getPosition();
      sf::Vector2f centerlogic(0,0);
      sf::Vector2f neighbors(0,0);
      int n = 0;
      int m = 0;
      std::map<int,int> cellsafe;
      std::map<int,int>::iterator cells;
      std::set<int> cells_taken;
      std::set<int>::iterator cellset;
      std::vector<float> size_in_x;
      std::set<float>::iterator vit;
      std::set<float> size_in_y;

      bool taken = true;

      cluster.clear();
      cellsafe.clear();
      cells_taken.clear();
      size_in_x.clear();
      size_in_y.clear();
      final.clear();

      // Locate the center of a logic pattern
      float mindistance;
      int closest_cell = -1;
      float maximumy;
      for( mapit = modmap.begin(); mapit != modmap.end(); mapit++ ){
	sf::Vector2f temp = (*mapit).second.getPosition();
	sf::Vector2f D = nodetemp - temp;
	float distance = sqrt( pow(D.x,2) + pow(D.y,2) );
	if( closest_cell == -1 || distance < mindistance ){
	  mindistance = distance;
	  closest_cell = mapit->first;
	  maximumy = temp.y;
	}
      }
      cluster[n++] = modmap[closest_cell];
      cellsafe[m++] = closest_cell;
      cells_taken.insert( closest_cell );
      size_in_x.push_back( maximumy );
      size_in_y.insert( maximumy );
    
      // Nearest neighbors routine
      clusterit = cluster.begin();
      while( clusterit != cluster.end() && cluster.size() < maxclustersize ){
	centerlogic = clusterit->second.getPosition();
	int clusterindex = clusterit->first; //position in cluster array
	int clustercell = cellsafe[clusterindex];

	// Get the closest modules and add to Logic Cluster
	for( clustit = modmap.begin(); clustit != modmap.end(); clustit++ ) {
	  int clustcell = clustit->first;
	  if( clustcell != clustercell ){
	    taken = true;
	    neighbors = clustit->second.getPosition();
	    sf::Vector2f Dclust = neighbors - centerlogic;
	    sf::Vector2f Dnode = neighbors - nodetemp;
	    float distance = sqrt( pow(Dclust.x,2) + pow(Dclust.y,2) );

	    if( fabs(Dnode.x) < 4.1*size.x && fabs(Dnode.y) < 4.1*size.y && cluster.size() < maxclustersize && distance < 1.2*size.x ) {
	    
	      taken =  cells_taken.find( clustit->first ) != cells_taken.end(); 
	      int mycount_in_x = 0;
	      int mycount_in_y = 0;
	      if( !taken ) {
		size_in_x.push_back( neighbors.y );
		size_in_y.insert( neighbors.y );
		mycount_in_x = std::count( size_in_x.begin(), size_in_x.end(), neighbors.y );
		mycount_in_y = size_in_y.size(); 

		if( cells_taken.size() < maxclustersize ) {	      
		  if( mycount_in_x <= 8 && mycount_in_y <= 8 ) {
		    cellsafe[ m++ ] = clustit->first;
		    cluster[ n++ ] = clustit->second;
		    cells_taken.insert( clustit->first );
		  }
		}
	      
	      }
	    }	    	  
	  }   
	} 
	++clusterit;
      }
      // Change color of clusters - overlaps handled in colorthelogic() 
      // ***this routine is necessary to make a map of cell number and shape
      for( clustit = cluster.begin(); clustit != cluster.end(); clustit++ ) {
	int temp = i % colors.size();
	(*clustit).second.setFillColor( colors[temp] );

	for( cells = cellsafe.begin(); cells != cellsafe.end(); cells++ ) {
	  // counting variable must be equal
	  if( clustit->first == cells->first ) {
	    // make the final map 
	    final[cells->second] = clustit->second;
	  }
	}
      }
      // Add to global logic vector used throughout the rest of the code
      global_logic.push_back( final );
  }
  }
}

void HCal::colorthelogic() {
  // Use cell number to compare if cluster cells overlap
  for( glit = global_logic.begin(); glit != global_logic.end(); glit++ ) {
    for( clustit = glit->begin(); clustit != glit->end(); clustit++ ) {
      
      int currentLogicCluster = clustit->first;

      for( glit_rest = global_logic.begin(); glit_rest != global_logic.end(); glit_rest++ ) {
	if( glit != glit_rest ) {
	  for( clusterit = glit_rest->begin(); clusterit != glit_rest->end(); clusterit++ ) {
	    
	    int nextLogicCluster = clusterit->first;

	    // Check to see if cell # is the same, if so then add colors to achieve overlapping effect
	    if( currentLogicCluster == nextLogicCluster ) {
	      sf::Color first = (clustit->second).getFillColor();
	      sf::Color second = (clusterit->second).getFillColor();
	      sf::Color add = first + second;
	      clusterit->second.setFillColor(add);
	    }

	  }     
	}
      }
    }
  }
}

void HCal::logicinfo() {
  // Spit out a text file with cell number + location in x and y (mm) relative
  // to the center of HCal
  int node = 0;
  
  std::ofstream logic_file("logic.txt");
  if( logic_file.is_open() ) {
    logic_file << "# Units are in mm. " << std::endl;
    logic_file << "# Coordinates are relative to HCal center, same system as G4SBS" << std::endl;
    logic_file << "# Number of logic patterns = " << global_logic.size() << std::endl;
    logic_file << "# Number of modules = " << count_modules << std::endl;
    logic_file << std::endl;
    logic_file << std::setw(5) << "#cell" << std::setw(5) << "x" 
	       << std::setw(5) << "y"    << std::setw(7) << "size" << std::endl; 
    
    for( glit = global_logic.begin(); glit != global_logic.end(); glit++ ) {
      node++;
      int cells = 0;
      for( mapit = glit->begin(); mapit != glit->end(); mapit++ ) {
	cells++;
	sf::Vector2f temp = mapit->second.getPosition() - center_display;
	sf::Vector2f size = mapit->second.getSize();
	
	logic_file << std::setw(5) << mapit->first << std::setw(6) << temp.x 
		   << std::setw(8) << -1*temp.y + 0.5   << std::setw(5) << size.x << std::endl; 
      }
      logic_file << "# node number = " << node << " with " << cells << " cells" << std::endl;
    }
  }
  else std::cerr << "Error opening text output." << std::endl;
  
  logic_file.close();
}

void HCal::draw(sf::RenderTarget& target, sf::RenderStates) const{
  std::map<int,sf::RectangleShape>::const_iterator cit;
  for( cit = modmap.begin(); cit != modmap.end(); cit++ ){
    target.draw( cit->second );
  }
  std::vector<std::map<int,sf::RectangleShape> >::const_iterator glit_const;
  //if( !logcolors ){
  for( glit_const = global_logic.begin(); glit_const != global_logic.end(); glit_const++ ) {
    for( cit = glit_const->begin(); cit != glit_const->end(); cit++ ) {
      target.draw( (*cit).second );
    }
  }
  //}
  std::vector<sf::CircleShape>::const_iterator cit1;
  for( cit1 = nodes.begin(); cit1 != nodes.end(); cit1++ ){
    target.draw(*cit1);
  }
}
