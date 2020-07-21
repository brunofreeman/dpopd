# Generation of Discrete Pedestrian Occupancy Probability Distributions
This is the codebase for a [Caltech SURF](http://sfp.caltech.edu/programs/surf) research project.
The project aims to produce a predictive model that can forecast the likelihood that a
discretized cell in an environment will be occupied by at least one pedestrian at a future time
step.

## Dependencies/Incorporations
* Agent Navigation
  - [brunofreeman/dijkstra_polygon](https://github.com/brunofreeman/dijkstra_polygon): finds shortest polyline between points in environment
  - [fawwazbmn/SocialForceModel](https://github.com/fawwazbmn/SocialForceModel): basis for SFM implementation
    * Note: code significantly altered and incorporated directly into this repository
* Graphics
  - [GLFW](https://www.glfw.org/): creates application window
  - [GLEW](http://glew.sourceforge.net/): accesses OpenGL functions
  - [mapbox/earcut.hpp](https://github.com/mapbox/earcut.hpp): triangulates polygons
  - [The Cherno](https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2): provided basis for graphics abstraction
    * Note: not a publically-available versioned code repository, rather a tutorial series on which much of this project's graphics abstraction is based
* Other
  - [nlohmann/json](https://github.com/nlohmann/json): reads environment definitions from JSON files
