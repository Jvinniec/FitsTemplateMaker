//
//  WcsHandler.cxx
//  FitsTemplateMaker
//
//  Created by Josh Cardenzana on 1/7/16.
//  Copyright © 2016 JCardenzana. All rights reserved.
//

#include "WcsHandler.h"

#include <stdio.h>
#include <string>
#include <cstring>

//_____________________________________________________________________________
// Method for converting a set of coordinates from an old value to a new one
void ConvertCoordinates(double *xcoord, double *ycoord,
                        const std::string& old_coord_system_,
                        const std::string& new_coord_system_)
{
    // Galactic -> <something>
    if (old_coord_system_.compare("Galactic")==0) {
        // GALACTIC -> J2000
        if (new_coord_system_.compare("J2000")==0) {
            Galactic2Celestial(*xcoord, *ycoord, xcoord, ycoord) ;
        }
    // J2000 -> <something>
    } else if (old_coord_system_.compare("J2000")==0) {
        // J2000 -> GALACTIC
        if (new_coord_system_.compare("GALACTIC")==0) {
            Celestial2Galactic(*xcoord, *ycoord, xcoord, ycoord) ;
        }
    }
}


//_____________________________________________________________________________
// Method for converting from gnomonic projected coordinates to world coordinates
void Projected2Coordinate(double xproj, double yproj,
                          double xTangentPoint, double yTangentPoint,
                          double *xcoord, double *ycoord,
                          const std::string& coordSys)
{
    // Create a WorldCoor object to handle manipulation of objects
    WorldCoor* wcs = initWCS(xTangentPoint, yTangentPoint, coordSys) ;
    
    xproj *= DegToRad() ;
    yproj *= DegToRad() ;
    
    // Fill the actual objects
    tnxpos(xproj, yproj, wcs, xcoord, ycoord) ;
    
    delete wcs ;
    
    return ;
}


//_____________________________________________________________________________
// Method for converting from world coordinates to gnomonic projected coordinates
// Make sure coordinates are in
void Coordinate2Projected(double xcoord, double ycoord,
                          double xTangentPoint, double yTangentPoint,
                          double *xproj, double *yproj,
                          const std::string& coordSys)
{
    // Create a WorldCoor object to handle manipulation of objects
    WorldCoor* wcs = initWCS(xTangentPoint, yTangentPoint, coordSys) ;
    
    // Fill the actual objects
    tnxpix(xcoord, ycoord, wcs, xproj, yproj) ;
    
    // Returned projected coordinates are in radians, so we must convert them to degrees
    *xproj *= RadToDeg() ;
    *yproj *= RadToDeg() ;
    
    // We no longer need the wcs object, so delete it
    delete wcs ;
    
    return ;
}


//_____________________________________________________________________________
// Method for converting from celestial (J2000) to galactic coordinates
void Celestial2Galactic(double Ra, double Dec,
                        double *GLon, double *GLat)
{
    // Set GLon,GLat equal to Ra,Dec. They will be converted in the call to wcscon.
    *GLon = Ra ;
    *GLat = Dec ;
    
    // Get the converted coordinates
    wcscon(1, 3, 2000, 2000, GLon, GLat, 2000) ;
    
    return ;
}


//_____________________________________________________________________________
// Method for converting from galactic to celestial (J2000 coordinates
void Galactic2Celestial(double GLon, double GLat,
                        double *Ra, double *Dec)
{
    // Set GLon,GLat equal to Ra,Dec. They will be converted in the call to wcscon.
    *Ra  = GLon ;
    *Dec = GLat ;
    
    // Get the converted coordinates
    wcscon(3, 1, 2000, 2000, Ra, Dec, 2000) ;
    
    return ;
}


//_____________________________________________________________________________
double AngularSeparation_Deg(double x1coord, double y1coord,
                             double x2coord, double y2coord)
{
    // This method assumes you're passing coordinates in degrees and want the
    // separation returned in degrees.
    
    // Compute the angular separation between the two points
    return wcsdist(x1coord, y1coord, x2coord, y2coord) ;
}


//_____________________________________________________________________________
double AngularSeparation_Rad(double x1coord, double y1coord,
                             double x2coord, double y2coord)
{
    // This method assumes you are passing coordinates in Radians and want the
    // separation returned in radians.
    
    // First convert all of the passed variables to degrees
    x1coord *= RadToDeg() ;
    y1coord *= RadToDeg() ;
    x2coord *= RadToDeg() ;
    y2coord *= RadToDeg() ;
    
    // Compute the angular separation between the two points
    return DegToRad() * wcsdist(x1coord, y1coord, x2coord, y2coord) ;
}


//_____________________________________________________________________________
WorldCoor* initWCS(double xTangentPoint, double yTangentPoint, const std::string& coordSys)
{
    // Get the ctypes for the two axes (as recognized by wcslib)
    std::string stype1, stype2 ;
    int syscode=1;
    if (std::strcmp(coordSys.c_str(), "J2000") == 0) {
        stype1 = "RA" ;
        stype2 = "DEC" ;
        //        syscode = ;
    } else if (std::strcmp(coordSys.c_str(), "GALACTIC") == 0) {
        stype1 = "GLON" ;
        stype2 = "GLAT" ;
        syscode = 3 ;
    }
    
    // Setup all of the variables we're going to need to define the wcs object
    int naxis1 = 0 ;                                // number of bins in x
    int naxis2 = 0 ;                                // number of bins in y
    char *ctype1 = new char[stype1.length()+1] ;    // x-axis coordinate
    char *ctype2 = new char[stype2.length()+1] ;    // y-axis coordinate
    double crpix1 = 0.0 ;                           // Reference position X bin
    double crpix2 = 0.0 ;                           // Reference position Y bin
    double crval1 = xTangentPoint ;                 // Reference position X coordinate
    double crval2 = yTangentPoint ;                 // Reference position Y coordinate
    double cd[4] = {-1.0,0.0,0.0,-1.0} ;            // Rotation matrix, setup to have no rotation
    double cdelt1 = 0.0 ;                           // ignored if (cd != NULL)
    double cdelt2 = 0.0 ;                           // ignored if (cd != NULL)
    double crota  = 0.0 ;                           // ignored if (cd != NULL)
    int equinox = 2000 ;
    double epoch = 0.0 ;
    
    // Establish the wcs object
    WorldCoor* wcs = wcskinit(naxis1, naxis2,
                              ctype1, ctype2,
                              crpix1, crpix2,
                              crval1, crval2,
                              cd,
                              cdelt1, cdelt2,
                              crota,
                              equinox,
                              epoch) ;
    
    // The following are necessary to ensure the correct values of our variables are returned
    wcs->syswcs = syscode ;
    wcs->rodeg = 1.0 ;
    
    // Clean up our coordinate type variables
    delete[] ctype1 ;
    delete[] ctype2 ;
    
    return wcs ;
}