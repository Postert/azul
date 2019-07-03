// azul
// Copyright © 2016-2019 Ken Arroyo Ohori
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef JSONParsingHelper_hpp
#define JSONParsingHelper_hpp

#include "DataModel.hpp"
#include "simdjson/jsonparser.h"

class JSONParsingHelper {
    
//    object.id = jsonObject.key();
//    //  std::cout << "ID: " << object.id << std::endl;
//    object.type = jsonObject.value()["type"];
//    //  std::cout << "Type: " << object.type << std::endl;
//
//    for (auto const &geometry: jsonObject.value()["geometry"]) {
////      std::cout << "Geometry: " << geometry.dump(2) << std::endl;
//
//      if (geometry["type"] == "MultiSurface" || geometry["type"] == "CompositeSurface") {
////        std::cout << "Surfaces: " << geometry["boundaries"].dump() << std::endl;
//        for (unsigned int surfaceIndex = 0; surfaceIndex < geometry["boundaries"].size(); ++surfaceIndex) {
////          std::cout << "Surface: " << geometry["boundaries"][surfaceIndex].dump() << std::endl;
//          std::vector<std::vector<std::size_t>> surface = geometry["boundaries"][surfaceIndex];
//          std::string surfaceType;
//          if (geometry.count("semantics")) {
////            std::cout << "Surface semantics: " << geometry["semantics"] << std::endl;
//            if (geometry["semantics"]["values"].size() > surfaceIndex &&
//                !geometry["semantics"]["values"][surfaceIndex].is_null()) {
//              std::size_t semanticSurfaceIndex = geometry["semantics"]["values"][surfaceIndex];
//              auto const &surfaceSemantics = geometry["semantics"]["surfaces"][semanticSurfaceIndex];
//              surfaceType = surfaceSemantics["type"];
//              std::cout << "Surface type: " << surfaceType << std::endl;
//              AzulObject newChild;
//              newChild.type = surfaceType;
//              AzulPolygon newPolygon;
//              parseCityJSONPolygon(surface, newPolygon, vertices);
//              newChild.polygons.push_back(newPolygon);
//              object.children.push_back(newChild);
//            } else {
//              AzulPolygon newPolygon;
//              parseCityJSONPolygon(surface, newPolygon, vertices);
//              object.polygons.push_back(newPolygon);
//            }
//          } else {
//            AzulPolygon newPolygon;
//            parseCityJSONPolygon(surface, newPolygon, vertices);
//            object.polygons.push_back(newPolygon);
//          }
//        }
//      }
  
  void parseCityJSONObject(ParsedJson::iterator &jsonObject, AzulObject &object, std::vector<std::tuple<double, double, double>> &vertices) {
    ParsedJson::iterator currentCityObject(jsonObject);
    if (!currentCityObject.is_object()) return;
    currentCityObject.down();
    
    do {
      if (currentCityObject.get_string_length() == 4 && memcmp(currentCityObject.get_string(), "type", 4) == 0) {
        currentCityObject.next();
        object.type = currentCityObject.get_string();
      }
      
      else if (currentCityObject.get_string_length() == 10 && memcmp(currentCityObject.get_string(), "attributes", 10) == 0) {
        currentCityObject.next();
        ParsedJson::iterator currentAttribute(currentCityObject);
        if (currentAttribute.is_object() && currentAttribute.down()) {
          do {
            const char *attributeName = currentAttribute.get_string();
            currentAttribute.next();
            if (currentAttribute.is_string()) object.attributes.push_back(std::pair<std::string, std::string>(attributeName, currentAttribute.get_string()));
            else if (currentAttribute.is_double()) object.attributes.push_back(std::pair<std::string, std::string>(attributeName, std::to_string(currentAttribute.get_double())));
            else if (currentAttribute.is_integer()) object.attributes.push_back(std::pair<std::string, std::string>(attributeName, std::to_string(currentAttribute.get_integer())));
          } while (currentAttribute.next());
        }
      }
      
      else if (currentCityObject.get_string_length() == 8 && memcmp(currentCityObject.get_string(), "geometry", 8) == 0) {
        currentCityObject.next();
        ParsedJson::iterator currentGeometry(currentCityObject);
        if (currentGeometry.is_array() && currentGeometry.down()) {
          do {
            if (currentGeometry.is_object()) {
              ParsedJson::iterator currentGeometryMember(currentGeometry);
              ParsedJson::iterator *boundariesIterator = NULL, *semanticsIterator = NULL;
              std::vector<std::map<std::string, std::string>> semanticSurfaces;
              std::string geometryType, geometryLod;
              if (currentGeometryMember.down()) {
                do {
                  if (currentGeometryMember.get_string_length() == 4 && memcmp(currentGeometryMember.get_string(), "type", 4) == 0) {
                    currentGeometryMember.next();
                    if (currentGeometryMember.is_string()) geometryType = currentGeometryMember.get_string();
                  } else if (currentGeometryMember.get_string_length() == 3 && memcmp(currentGeometryMember.get_string(), "lod", 3) == 0) {
                    currentGeometryMember.next();
                    if (currentGeometryMember.is_string()) geometryLod = currentGeometryMember.get_string();
                    else if (currentGeometryMember.is_double()) geometryLod = std::to_string(currentGeometryMember.get_double());
                    else if (currentGeometryMember.is_integer()) geometryLod = std::to_string(currentGeometryMember.get_integer());
                  } else if (currentGeometryMember.get_string_length() == 10 && memcmp(currentGeometryMember.get_string(), "boundaries", 10) == 0) {
                    currentGeometryMember.next();
                    boundariesIterator = new ParsedJson::iterator(currentGeometryMember);
                  } else if (currentGeometryMember.get_string_length() == 9 && memcmp(currentGeometryMember.get_string(), "semantics", 9) == 0) {
                    currentGeometryMember.next();
                    ParsedJson::iterator currentSemantics(currentGeometryMember);
                    if (currentSemantics.is_object() && currentSemantics.down()) {
                      do {
                        if (currentSemantics.get_string_length() == 8 && memcmp(currentSemantics.get_string(), "surfaces", 8) == 0) {
                          currentSemantics.next();
                          ParsedJson::iterator currentSemanticSurface(currentSemantics);
                          if (currentSemanticSurface.is_array() && currentSemanticSurface.down()) {
                            semanticSurfaces.push_back(std::map<std::string, std::string>());
                            ParsedJson::iterator currentAttribute(currentSemanticSurface);
                            if (currentAttribute.is_object() && currentAttribute.down()) {
                              do {
                                if (currentAttribute.is_string()) {
                                  const char *attributeName = currentAttribute.get_string();
                                  currentAttribute.next();
                                  if (currentAttribute.is_string()) semanticSurfaces.back()[attributeName] = currentAttribute.get_string();
                                  else if (currentAttribute.is_double()) semanticSurfaces.back()[attributeName] = std::to_string(currentAttribute.get_double());
                                  else if (currentAttribute.is_integer()) semanticSurfaces.back()[attributeName] = std::to_string(currentAttribute.get_integer());
                                } else currentAttribute.next();
                              } while (currentAttribute.next());
                            }
                          }
                        } else if (currentSemantics.get_string_length() == 6 && memcmp(currentSemantics.get_string(), "values", 6) == 0) {
                          currentSemantics.next();
                          semanticsIterator = new ParsedJson::iterator(currentSemantics);
                        } else currentSemantics.next();
                      } while (currentSemantics.next());
                    }
                  } else currentGeometryMember.next();
                } while (currentGeometryMember.next());
              }
              
              if (!geometryType.empty() && !geometryLod.empty() && boundariesIterator != NULL) {
                object.children.push_back(AzulObject());
                object.children.back().type = "LoD";
                object.children.back().id = geometryLod;
                
                if (strcmp(geometryType.c_str(), "MultiSurface") == 0 ||
                    strcmp(geometryType.c_str(), "CompositeSurface") == 0) {
                  parseCityJSONGeometry(boundariesIterator, semanticsIterator, semanticSurfaces, 2, object.children.back(), vertices);
                }
                
                else if (strcmp(geometryType.c_str(), "Solid") == 0) {
                  parseCityJSONGeometry(boundariesIterator, semanticsIterator, semanticSurfaces, 3, object.children.back(), vertices);
                }
                
                else if (strcmp(geometryType.c_str(), "MultiSolid") == 0 ||
                         strcmp(geometryType.c_str(), "CompositeSolid") == 0) {
                  parseCityJSONGeometry(boundariesIterator, semanticsIterator, semanticSurfaces, 4, object.children.back(), vertices);
                }
              }
              
              if (boundariesIterator != NULL) delete boundariesIterator;
              if (semanticsIterator != NULL) delete semanticsIterator;
            }
          } while (currentGeometry.next());
        }
      }
      
      else currentCityObject.next();
    } while (currentCityObject.next());
  }
  
  void parseCityJSONGeometry(ParsedJson::iterator *jsonBoundaries, ParsedJson::iterator *jsonSemantics, std::vector<std::map<std::string, std::string>> &semanticSurfaces, int nesting, AzulObject &object, std::vector<std::tuple<double, double, double>> &vertices) {
//    std::cout << "jsonBoundaries: ";
//    dump(*jsonBoundaries);
//    std::cout << std::endl;
//    std::cout << "jsonSemantics: ";
//    dump(*jsonSemantics);
//    std::cout << std::endl;
//    std::cout << "nesting: " << nesting << std::endl;
//    if (jsonBoundaries == NULL) return;
    
    if (nesting > 1) {
      ParsedJson::iterator currentBoundary(*jsonBoundaries);
      if (!currentBoundary.is_array() || !currentBoundary.down()) return;
      if (jsonSemantics != NULL && jsonSemantics->is_array()) {
        ParsedJson::iterator currentSemantics(*jsonSemantics);
        if (currentSemantics.down()) {
          do {
            parseCityJSONGeometry(&currentBoundary, &currentSemantics, semanticSurfaces, nesting-1, object, vertices);
            if (!currentBoundary.next()) break;
            if (!currentSemantics.next()) break;
          } while (true);
        } else {
          do {
            parseCityJSONGeometry(&currentBoundary, NULL, semanticSurfaces, nesting-1, object, vertices);
          } while (currentBoundary.next());
        }
      } else {
        do {
          parseCityJSONGeometry(&currentBoundary, NULL, semanticSurfaces, nesting-1, object, vertices);
        } while (currentBoundary.next());
      }
    }
    
    else if (nesting == 1) {
      ParsedJson::iterator currentBoundary(*jsonBoundaries);
      if (jsonSemantics != NULL && jsonSemantics->is_integer()) {
        if (jsonSemantics->is_integer() && jsonSemantics->get_integer() < semanticSurfaces.size()) {
          object.children.push_back(AzulObject());
          for (auto const &attribute: semanticSurfaces[jsonSemantics->get_integer()]) {
//            std::cout << attribute.first << ": " << attribute.second << std::endl;
            if (strcmp(attribute.first.c_str(), "type") == 0) object.children.back().type = attribute.second;
            else object.children.back().attributes.push_back(std::pair<std::string, std::string>(attribute.first, attribute.second));
          } object.children.back().polygons.push_back(AzulPolygon());
          parseCityJSONPolygon(currentBoundary, object.children.back().polygons.back(), vertices);
        } else {
          object.polygons.push_back(AzulPolygon());
          parseCityJSONPolygon(currentBoundary, object.polygons.back(), vertices);
        }
      } else {
        object.polygons.push_back(AzulPolygon());
        parseCityJSONPolygon(currentBoundary, object.polygons.back(), vertices);
      }
    }
  }

  void parseCityJSONPolygon(ParsedJson::iterator &jsonPolygon, AzulPolygon &polygon, std::vector<std::tuple<double, double, double>> &vertices) {
    bool outer = true;
    ParsedJson::iterator jsonRing(jsonPolygon);
    if (jsonRing.is_array() && jsonRing.down()) {
      do {
        if (outer) {
          parseCityJSONRing(jsonRing, polygon.exteriorRing, vertices);
          outer = false;
        } else {
          polygon.interiorRings.push_back(AzulRing());
          parseCityJSONRing(jsonRing, polygon.interiorRings.back(), vertices);
        }
      } while (jsonRing.next());
    }
  }

  void parseCityJSONRing(ParsedJson::iterator &jsonRing, AzulRing &ring, std::vector<std::tuple<double, double, double>> &vertices) {
    ParsedJson::iterator jsonVertex(jsonRing);
    if (jsonVertex.is_array() && jsonVertex.down()) {
      do {
        if (jsonVertex.is_integer() && jsonVertex.get_integer() < vertices.size()) {
          ring.points.push_back(AzulPoint());
          ring.points.back().coordinates[0] = std::get<0>(vertices[jsonVertex.get_integer()]);
          ring.points.back().coordinates[1] = std::get<1>(vertices[jsonVertex.get_integer()]);
          ring.points.back().coordinates[2] = std::get<2>(vertices[jsonVertex.get_integer()]);
        }
      } while (jsonVertex.next());
      ring.points.push_back(ring.points.front());
    }
  }

public:
  void parse(const char *filePath, AzulObject &parsedFile) {
    ParsedJson parsedJson = build_parsed_json(get_corpus(filePath));
    if(!parsedJson.isValid()) {
      std::cout << "Invalid JSON file" << std::endl;
      return;
    } parsedFile.type = "File";
    parsedFile.id = filePath;
    
    const char *docType;
    const char *docVersion;
    
    // Check what we have
    ParsedJson::iterator iterator(parsedJson);
    ParsedJson::iterator *verticesIterator = NULL, *cityObjectsIterator = NULL, *metadataIterator = NULL, *geometryTemplatesIterator = NULL;
    if (!iterator.is_object() || !iterator.down()) return;
    do {
      if (iterator.get_string_length() == 4 && memcmp(iterator.get_string(), "type", 4) == 0) {
        iterator.next();
        docType = iterator.get_string();
      } else if (iterator.get_string_length() == 7 && memcmp(iterator.get_string(), "version", 7) == 0) {
        iterator.next();
        docVersion = iterator.get_string();
      } else if (iterator.get_string_length() == 10 && memcmp(iterator.get_string(), "extensions", 10) == 0) {
        iterator.next();
      } else if (iterator.get_string_length() == 8 && memcmp(iterator.get_string(), "metadata", 8) == 0) {
        iterator.next();
        metadataIterator = new ParsedJson::iterator(iterator);
      } else if (iterator.get_string_length() == 9 && memcmp(iterator.get_string(), "transform", 9) == 0) {
        iterator.next();
      } else if (iterator.get_string_length() == 11 && memcmp(iterator.get_string(), "CityObjects", 11) == 0) {
        iterator.next();
        cityObjectsIterator = new ParsedJson::iterator(iterator);
      } else if (iterator.get_string_length() == 8 && memcmp(iterator.get_string(), "vertices", 8) == 0) {
        iterator.next();
        verticesIterator = new ParsedJson::iterator(iterator);
      } else if (iterator.get_string_length() == 10 && memcmp(iterator.get_string(), "appearance", 10) == 0) {
        iterator.next();
      } else if (iterator.get_string_length() == 18 && memcmp(iterator.get_string(), "geometry-templates", 18) == 0) {
        iterator.next();
        geometryTemplatesIterator = new ParsedJson::iterator(iterator);
      }
    } while (iterator.next());
    
    if (strcmp(docType, "CityJSON") == 0) {
      std::cout << docType << " " << docVersion << " detected" << std::endl;
      if (strcmp(docVersion, "1.0") == 0) {
        // Metadata
        if (metadataIterator != NULL && metadataIterator->is_object() && metadataIterator->down()) {
          do {
            const char *attributeName = metadataIterator->get_string();
            metadataIterator->next();
            if (metadataIterator->is_string()) {
              const char *attributeValue = metadataIterator->get_string();
              parsedFile.attributes.push_back(std::pair<std::string, std::string>(attributeName, attributeValue));
            } else {
              std::cout << attributeName << " is a complex attribute. Skipped." << std::endl;
            }
          } while (metadataIterator->next());
        }
        
        // Vertices
        std::vector<std::tuple<double, double, double>> vertices;
        if (verticesIterator != NULL && verticesIterator->is_array() && verticesIterator->down()) {
          do {
            ParsedJson::iterator currentVertex(*verticesIterator);
            if (currentVertex.is_array()) {
              currentVertex.down();
              double x, y, z;
              if (currentVertex.is_double()) x = currentVertex.get_double();
              else if (currentVertex.is_integer()) x = currentVertex.get_integer();
              else continue;
              if (!currentVertex.next()) continue;
              if (currentVertex.is_double()) y = currentVertex.get_double();
              else if (currentVertex.is_integer()) y = currentVertex.get_integer();
              else continue;
              if (!currentVertex.next()) continue;
              if (currentVertex.is_double()) z = currentVertex.get_double();
              else if (currentVertex.is_integer()) z = currentVertex.get_integer();
              else continue;
              vertices.push_back(std::tuple<double, double, double>(x, y, z));
              //          std::cout << "Parsed (" << x << ", " << y << ", " << z << ")" << std::endl;
            }
          } while (verticesIterator->next());
        }
        
        // CityObjects
        if (cityObjectsIterator != NULL && cityObjectsIterator->is_object() && cityObjectsIterator->down()) {
          do {
            parsedFile.children.push_back(AzulObject());
            const char *objectId = cityObjectsIterator->get_string();
            parsedFile.children.back().id = objectId;
            cityObjectsIterator->next();
            parseCityJSONObject(*cityObjectsIterator, parsedFile.children.back(), vertices);
          } while (cityObjectsIterator->next());
        }

      } else {
        std::cout << "Unsupported version" << std::endl;
      }
    }

    if (verticesIterator != NULL) delete verticesIterator;
    if (cityObjectsIterator != NULL) delete cityObjectsIterator;
    if (metadataIterator != NULL) delete metadataIterator;
    if (geometryTemplatesIterator != NULL) delete geometryTemplatesIterator;
  }
  
  void dump(ParsedJson::iterator &iterator) {
    if (iterator.is_string()) std::cout << iterator.get_string();
    else if (iterator.is_integer()) std::cout << iterator.get_integer();
    else if (iterator.is_double()) std::cout << iterator.get_double();
    else if (iterator.is_array()) {
      std::cout << "[";
      if (iterator.down()) {
        dump(iterator);
        while (iterator.next()) {
          std::cout << ",";
          dump(iterator);
        } iterator.up();
      } std::cout << "]";
    } else if (iterator.is_object()) {
      std::cout << "{";
      if (iterator.down()) {
        std::cout << iterator.get_string();
        std::cout << ":";
        iterator.next();
        dump(iterator);
        while (iterator.next()) {
          std::cout << ",";
          std::cout << iterator.get_string();
          std::cout << ":";
          iterator.next();
          dump(iterator);
        } iterator.up();
      } std::cout << "}";
    }
  }
  
  void clearDOM() {
//    json.clear();
  }
};

#endif /* JSONParsingHelper_hpp */
