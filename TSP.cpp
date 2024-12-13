#include "TSP.hpp"

/**
 * Displays the edges and total distance of the tour.
 * Each edge is printed in the format: "EDGE start_id -> end_id | WEIGHT: weight".
 */
void TSP::Tour::display() const {
  for (size_t i = 1; i < path.size(); i++) {
    std::cout << "EDGE " << path[i-1].id << " -> " << path[i].id << " | WEIGHT : " << weights[i] << std::endl;
  }
  std::cout << "TOTAL DISTANCE: " << total_distance << std::endl;
}

/**
 * Reads a .tsp file and constructs a list of cities as nodes.
 * The file should have a section labeled "NODE_COORD_SECTION" followed by lines with the format: ID x-coordinate y-coordinate.
 * 
 * @param filename The path to the TSP file.
 * @return A list of `Node` objects representing cities and their coordinates.
 * @throws std::runtime_error If the file cannot be read or parsed.
 * 
 * @pre The file specified by `filename` exists and follows the TSP format.
 */
std::list<Node> TSP::constructCities(const std::string& filename) {
  // Read past metadata
  std::ifstream fin(filename);
  if (fin.fail()) {
    std::cerr << "ERROR: Could not read file: " << filename << std::endl;
    throw std::runtime_error("Failed to read file. Terminating.");
  }

  std::string line;
  do { std::getline(fin, line); }
  while (line.find("NODE_COORD_SECTION"));

  // Read data from file into Node list "cities"
  std::list<Node> cities;
  size_t ID;
  double x, y;
  while (!fin.eof()){
    if (!(fin >> ID >> x >> y)) break;
    cities.push_back(Node(ID, x, y));
  }
  return cities;
}

/**
 * Constructs a tour using the nearest neighbor heuristic for the traveling salesperson problem (TSP).
 * Starting from the city of the given ID, it iteratively visits the nearest unvisited city and returns to the start.
 *
 * @param cities A list of `Node` objects representing the cities to be visited.
 * @param start_id The unique identifier of the starting city.
 * @return A `TSP::Tour` object representing the path, edge weights, and total distance of the computed tour.
 * 
 * @pre `start_id` must be a valid city ID within the range of IDs in `cities`.
 * @note The ith weight in `weights` is the distance traveled from the previous (i-1) city to the current i city (represented by the ith city in the returned Tour.path)
 *       As such, the first weight will ALWAYS equal 0, since there is no edge from the start city to itself
 *
 */
TSP::Tour TSP::nearestNeighbor(std::list<Node> cities, const size_t& start_id) {
  TSP::Tour tour;
  
  // check if start_id is a valid city ID within the range
  auto it = std::find_if(cities.begin(), cities.end(), [&start_id](const Node& city) {
      return city.id == start_id;
   });
  

  // start from that specified city as a source node
  Node current_city = *it;
  tour.path.push_back(current_city);
  tour.weights.push_back(0);
  size_t total_distance = 0;
  
  cities.erase(it);

  while(!cities.empty()) {
    // finding the nearest unvisited city
    Node nearest_city = *cities.begin();
    size_t min_distance = current_city.distance(*cities.begin());

    for(auto it = cities.begin(); it != cities.end(); ++it) {
      size_t current_distance = current_city.distance(*it);
      if(current_distance < min_distance) {
        nearest_city = *it;
        min_distance = current_distance;
      }
    }

    // update tour path with nearest city
     tour.path.push_back(nearest_city);
     tour.weights.push_back(min_distance);
     total_distance += min_distance;

    // remove nearest city from the list of cities and make it the current city it visited
    //  cities.erase(std::find_if(cities.begin(), cities.end(), [&nearest_city](const Node&city) {
    //     return city.id == nearest_city.id;
    //  }));
    auto nearest_it = std::find_if(cities.begin(), cities.end(), [&nearest_city](const Node& city) {
      return city.id == nearest_city.id;
   });
     cities.erase(nearest_it);
     current_city = nearest_city;
  }

  Node start_city = tour.path.front(); // distance between current city and starting point
  size_t return_distance = current_city.distance(start_city);
  tour.path.push_back(start_city);
  tour.weights.push_back(return_distance);
  total_distance += return_distance;

  tour.total_distance = total_distance;
  return tour;
}
