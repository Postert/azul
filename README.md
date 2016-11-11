# azul

azul is a CityGML viewer for macOS 10.12. It is available under the GPLv3 licence.

It is written mostly in Swift 3 and C++ with a bit of Objective-C and Objective-C++ to bind them together. It uses Metal (when available) or OpenGL (otherwise) for visualisation and simd or GLKit for matrix computations. It uses pugixml in order to parse XML since it is much faster than Apple's XMLParser, as well as the CGAL Triangulation package to triangulate concave polygons for display.

