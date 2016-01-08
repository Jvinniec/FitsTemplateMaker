//
//  WcsHandler.h
//  FitsTemplateMaker
//
//  Created by Josh Cardenzana on 1/7/16.
//  Copyright © 2016 JCardenzana. All rights reserved.
//

#ifndef FitsTemplateMaker_WcsHandler_h
#define FitsTemplateMaker_WcsHandler_h

// C++ HEADERS
#include <cstring>
#include <stdio.h>
#include <string>

// WCS header
#include "wcs.h"

// Method for converting coordinates generically
void ConvertCoordinates(double *xcoord, double *ycoord,
                        const std::string& old_coord_system_,
                        const std::string& new_coord_system_) ;

// Method for converting from gnomonic projected coordinates to world coordinates
void Projected2Coordinate(double xproj, double yproj,
                          double xTangentPoint, double yTangentPoint,
                          double *xcoord, double *ycoord,
                          const std::string& coordSys="J2000") ;

// Method for converting from world coordinates to gnomonic projected coordinates
void Coordinate2Projected(double xcoord, double ycoord,
                          double xTangentPoint, double yTangentPoint,
                          double *xproj, double *yproj,
                          const std::string& coordSys="J2000") ;

// Method for converting from celestial (J2000) to galactic coordinates
void Celestial2Galactic(double Ra, double Dec,
                        double *GLon, double *GLat) ;

// Method for converting from galactic to celestial (J2000) coordinates
void Galactic2Celestial(double GLon, double GLat,
                        double *Ra, double *Dec) ;

// Method for calculating the angular separation between two points on the celestial sphere
// Specifically assumes coordinates passed and angular distance returned are all in degrees.
double AngularSeparation_Deg(double x1coord, double y1coord,
                             double x2coord, double y2coord) ;

// Method for calculating the angular separation between two points on the celestial sphere
// Specifically assumes coordinates passed and angular distance returned are all in radians.
double AngularSeparation_Rad(double x1coord, double y1coord,
                             double x2coord, double y2coord) ;

// Method for initializing the wcs object which is needed by some of the above coordinate
// conversion methods
WorldCoor* initWCS(double xTangentPoint, double yTangentPoint, const std::string& coordSys) ;

// Methods adapted from ROOT's TMath class
inline double Pi()       { return 3.14159265358979323846; }
inline double RadToDeg() { return 180.0 / Pi(); }
inline double DegToRad() { return Pi() / 180.0; }

#endif /* FitsTemplateMaker_WcsHandler_h */
