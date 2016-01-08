//
//  FitsTemplateModel.cxx
//  FitsTemplateMaker
//
//  Created by Josh Cardenzana on 1/5/16.
//  Copyright © 2016 JCardenzana. All rights reserved.
//

#include <numeric>

#include "FitsTemplateModel.h"
#include "WcsHandler.h"

#pragma mark - Constructors

//________________________________________________________
FitsTemplateModel::FitsTemplateModel() :
    wcInfo(nullptr)
{
}

//________________________________________________________
// Basic constructor from a filename
FitsTemplateModel::FitsTemplateModel(const std::string& filename)
{
    // Dont worry about setting the fits_file_path, that's done by default
    // in the following method.
    InitModelFromFile(filename) ;
}


//________________________________________________________
// Copy constructor. Unfortunately, I'm restricted to requiring a new filename
// in order to make a copy of the other fits object
FitsTemplateModel::FitsTemplateModel(const FitsTemplateModel& other, const std::string& new_file_name) :
    image_info_(other.image_info_),
    wcInfo(new WorldCoor(*other.wcInfo))
{
}


//________________________________________________________
FitsTemplateModel::~FitsTemplateModel()
{
    wcsfree(wcInfo) ;
}

#pragma mark - Public Methods

//________________________________________________________
void FitsTemplateModel::AddKey(const std::string& key, const std::string& description,
                               const double value)
{
//    fits_header_object_->addKey(key, value, description) ;
}


//________________________________________________________
void FitsTemplateModel::AddKey(const std::string& key, const std::string& description,
                               const std::string& value)
{
//    fits_header_object_->addKey(key, value, description) ;
}


//________________________________________________________
// Extract a predefined model from a fits file
void FitsTemplateModel::InitModelFromFile(const std::string& filename)
{
    // Attempt to open the specified fits file.
    std::unique_ptr<CCfits::FITS> fits_object = nullptr ;
    try {
        fits_object.reset(new CCfits::FITS(filename));
    } catch (CCfits::FITS::CantOpen & e) {
        std::cerr << e.message() << std::endl ;
        return ;
    }
    
    // Since we can successfully open the file, set the fits_file_path to the new filename
    fits_file_path_ = filename ;
    
    // Now fill the internal storage objects
    
    // Check whether this object has been filled before
    if (wcInfo == nullptr) {
        // The wcInfo will only be a nullptr if we havent created it yet
        int headerLength, headerOffset;
        char * header = fitsrhead(const_cast<char*>(fits_file_path_.c_str()), &headerLength, &headerOffset);
        
        wcInfo = wcsinit(header) ;
        GetCoordinateSystem(wcInfo) ;
        
        // Try to fill the information for the histogram
        try {
            // Fill the imageInfo and get the number of x-bins
            fits_object->pHDU().read(image_info_);
            
            // TODO: check if the next four lines are necessary
            //wcInfo->nxpix = fits_object->pHDU().axis(0);
            //wcInfo->nypix = fits_object->pHDU().axis(1);
            
            //wcInfo->xrefpix = fits_object->pHDU().axis(0)*0.5 ;
            //wcInfo->yrefpix = fits_object->pHDU().axis(1)*0.5 ;
            
            // TODO: Extract the list of keywords from the fits file to save to any new file
            
        } catch (CCfits::FITS::CantOpen & e){
            // Something clearly went wrong ...
            std::cerr << "FITS keyword template could not be opened. (" << e.message() << ")" << std::endl;
        }
    }
    
    // Extract the keywords associated with this file (we'll need to update them as we go)
    
}


//________________________________________________________
// Save values to xCoord, yCoord, and coordSys
void FitsTemplateModel::GetCoordinateSystem(WorldCoor* worldCoor)
{
    if (worldCoor == nullptr) return ;
    
    // Get the value associated with WorldCoor.syswcs
    std::string coord_x_ = std::string(worldCoor->c1type) ;
    std::string coord_y_ = std::string(worldCoor->c2type) ;
    
    // Set the appropriate value of coordSys based on the values of xCoord & yCoord
    // <xCoord>,<yCoord> -- <saved value of coordSys>
    // RA,DEC -- J2000
    // GLON,GLAT -- GALACTIC
    // ELON,ELAT -- ECLIPTIC
    // <anything else> -- J2000 (default)
    
    // I've chosen to include a special case for J2000 rather than omitting it to make
    // adding additional cases in the future easier and incase we ever want to change the default.
    if ((coord_x_.compare("RA") == 0) && (coord_y_.compare("DEC") == 0)) {
        if (worldCoor->epoch == 1950.0) {
            // RA,Dec B1950 coordinates
            coord_system_ = "BJ1950" ;
            wcInfo->syswcs = 2 ;
        } else if (worldCoor->epoch == 2000.0) {
            // RA,Dec J2000 coordinates
            coord_system_ = "J2000" ;
            wcInfo->syswcs = 1 ;
        } else {
            // While it may not be guaranteed that the epoch actually corresponds to J2000,
            // I will assume that this is the case.
            coord_system_ = "J2000" ;
        }

    } else if ((coord_x_.compare("GLON") == 0) && (coord_y_.compare("GLAT") == 0)) {
        // Galactic longitude and latitude
        coord_system_ = "GALACTIC" ;
        // Set the value of the coordinate system for this map
        wcInfo->syswcs = 3 ;
    } else if ((coord_x_.compare("ELON") == 0) && (coord_y_.compare("ELAT") == 0)) {
        // Galactic longitude and latitude
        coord_system_ = "ECLIPTIC" ;
        // Set the value of the coordinate system for this map
        wcInfo->syswcs = 4 ;
    } else {
        // Default to J2000 coordinates
        coord_system_ = "J2000" ;
        // Set the value of the coordinate system for this map
        wcInfo->syswcs = 1 ;
        
        // Output a warning
        std::cerr << "[WARNING] FitsTemplateModel::getCoordinateSystem : " ;
        std::cerr << "Unknown coordinate system [" << coord_x_ << "," << coord_y_ <<"]. " ;
        std::cerr << "Defaulting to " << coord_system_ << ". If this is incorrect please check your template file." << std::endl;
    }
    
    // Now fill some other information about the wcInfo
    if (wcInfo != nullptr) {
        // Fill in the reference position based on the image
        double raExtent, decExtent;
        wcssize(wcInfo, &wcInfo->xref, &wcInfo->yref, &raExtent, &decExtent);
    }
    
    return ;
}


//________________________________________________________
// The following is used for querying the fits file at different coordinates.
double FitsTemplateModel::EvaluateAt(double xPos, double yPos, const std::string& inputCoordSys) const
{
    // Check whether this object has been filled before
    if (wcInfo == nullptr) {
        std::cerr << "[ERROR] FitsTemplateModel::EvaluateAt() : No coordinate system information stored." << std::endl;
    } else if (image_info_.size() == 0) {
        std::cerr << "[ERROR] FitsTemplateModel::EvaluateAt() : No image information stored." << std::endl;
    }
    
    // The wcInfo and imageInfo objects have been filled, so now we can look up the bin.
    double xpix=0, ypix=0 ; // x,y pixel associated with
    int offscl ;            // 0=OK, 1=offscale (this means coordinates are outside map)
    
    // Check if a coordinate system for xPos and yPos were supplied.
    if (std::strcmp(inputCoordSys.c_str(), std::string().c_str()) == 0) {
        // No coordinate system was specified, so we'll assume we're using the same system
        // as the map
        wcs2pix(wcInfo, xPos, yPos, &xpix, &ypix, &offscl) ;
    } else {
        // A coordinate system was specified, which may be different than the coordinate system of
        // the map. In that case we need to appropriately handle the coordinate transformation when
        // finding the appropriate x,y bin for this position. Fortunately that's done for us.
        
        // Double check that we have a valid value for the 'inputCoordSys'
        if ((std::strcmp(inputCoordSys.c_str(), "J2000") == 0)    ||
            (std::strcmp(inputCoordSys.c_str(), "GALACTIC") == 0) ||
            (std::strcmp(inputCoordSys.c_str(), "B1950") == 0)) {
            
            // The coordinate system checks out, so get the x,y bin in the fits image
            char* cstr = new char[inputCoordSys.length() + 1];
            std::strcpy(cstr, inputCoordSys.c_str()) ;
            
            // Get the bin position of this position
            wcsc2pix(wcInfo, xPos, yPos, cstr, &xpix, &ypix, &offscl) ;
            delete[] cstr ;
            
        } else {
            std::cerr << "[ERROR] FitsTemplateModel::EvaluateAt() : Unknown coordinate system given: " << inputCoordSys << std::endl ;
            std::cerr << "      Please choose from (J2000, GALACTIC, B1950). This method is case sensitive." << std::endl ;
            return 0.0 ;
        }
    }
    
    // Return the appropriate value based on the value of "offscl"
    if ((xpix<=0.0) || (xpix>=wcInfo->nxpix) || (ypix<=0.0) || (ypix>=wcInfo->nypix)) {
        // The coordinates fall outside the fits image
        return 0.0 ;
    } else {
        // xpix counts starting at 0 from the high end to the low end of the x-axis
        // ypix counts starting at 0 from the low end to the high end of the y-axis
        ypix = std::floor(ypix) ;
        xpix = std::floor(xpix) ;
        unsigned int bin = std::floor(xpix) + (wcInfo->nxpix*std::floor(ypix)) ;
        
        // Now we know the x,y bin where the value we want is located, so lets return this bin
        double imageVal = image_info_[bin] ;
        // Handle the cases where this value is either greater/less than the max/min possible value.
//        if (imageVal > std::numeric_limits<double>::max()) return 0.0 ;
//        if (imageVal < std::numeric_limits<double>::lowest()) return 0.0 ;
        return imageVal ;
    }
    
    // If 'offscl' is neither 0, 1, 2 the value is undefined, so print an error and return 0.
    std::cerr << "[ERROR] FitsTemplateModel::evaluateAt : Unknown value of offscl: " << offscl << std::endl ;
    return 0.0 ;
}


//________________________________________________________
// Set the appropriate coordinate information for J2000 coordinates
void FitsTemplateModel::SetCoordJ2000()
{
    // Check if the coordinate system is already in J2000 coordinates
    if (coord_system_.compare("J2000") == 0) return ;
    
    // find the new center position if the coord_system_ has changed
    double new_xref(wcInfo->xref), new_yref(wcInfo->yref) ;
    
    // Convert the coordinates from whatever they were to the new coordinates
    ConvertCoordinates(&new_xref, &new_yref, coord_system_, "J2000") ;
    
    // Get the new coordinates
    char ctype1[9], ctype2[9] ;
    std::snprintf(ctype1, 9, "RA--TAN") ;
    std::snprintf(ctype2, 9, "DEC-TAN") ;
    
    // Reinitialize the wcs information
    wcInfo = wcskinit(wcInfo->nxpix,	/* Number of pixels along x-axis */
                      wcInfo->nypix,	/* Number of pixels along y-axis */
                      ctype1,           /* FITS WCS projection for axis 1 */
                      ctype2,           /* FITS WCS projection for axis 2 */
                      wcInfo->xrefpix,	/* Reference pixel coordinates */
                      wcInfo->yrefpix,	/* Reference pixel coordinates */
                      new_xref,         /* Coordinate at reference pixel in degrees */
                      new_yref,         /* Coordinate at reference pixel in degrees */
                      wcInfo->cd,       /* Rotation matrix, used if not NULL */
                      wcInfo->cdelt[0],	/* scale in degrees/pixel, if cd is NULL */
                      wcInfo->cdelt[1],	/* scale in degrees/pixel, if cd is NULL */
                      wcInfo->rot,      /* Rotation angle in degrees, if cd is NULL */
                      2000,             /* Equinox of coordinates, 1950 and 2000 supported */
                      wcInfo->epoch);

    // Reset the coordinates
    coord_system_ = "J2000" ;
    
    return ;
}


//________________________________________________________
// Set the appropriate coordinate information for B1950 coordinates
void FitsTemplateModel::SetCoordB1950()
{
    // Until we have the coordinate conversion setup, we cant do anything with this class
    std::cerr << "[ERROR] Cannot convert to B1950 coordinates at this time!" << std::endl;
    return ;
    
    // Check if the coordinate system is already in J2000 coordinates
    if (coord_system_.compare("B1950") == 0) return ;
    
    // find the new center position if the coord_system_ has changed
    double new_xref(wcInfo->xref), new_yref(wcInfo->yref) ;
    
    // Convert the coordinates from whatever they were to the new coordinates
    ConvertCoordinates(&new_xref, &new_yref, coord_system_, "B1950") ;
    
    // Get the new coordinates
    char ctype1[9], ctype2[9] ;
    std::snprintf(ctype1, 9, "RA---TAN") ;
    std::snprintf(ctype2, 9, "DEC--TAN") ;
    
    WorldCoor* old_wcInfo = wcInfo ;
    wcInfo = wcskinit(wcInfo->nxpix,	/* Number of pixels along x-axis */
                      wcInfo->nypix,	/* Number of pixels along y-axis */
                      ctype1,	/* FITS WCS projection for axis 1 */
                      ctype2,	/* FITS WCS projection for axis 2 */
                      wcInfo->xrefpix,	/* Reference pixel coordinates */
                      wcInfo->yrefpix,	/* Reference pixel coordinates */
                      new_xref,	/* Coordinate at reference pixel in degrees */
                      new_yref,	/* Coordinate at reference pixel in degrees */
                      wcInfo->cd,	/* Rotation matrix, used if not NULL */
                      wcInfo->cdelt[0],	/* scale in degrees/pixel, if cd is NULL */
                      wcInfo->cdelt[1],	/* scale in degrees/pixel, if cd is NULL */
                      wcInfo->rot,	/* Rotation angle in degrees, if cd is NULL */
                      1950,             /* Equinox of coordinates, 1950 and 2000 supported */
                      wcInfo->epoch);
    
    // Reset the coordinates
    coord_system_ = "B1950" ;
    
    // delete
    delete old_wcInfo ;
    
    return ;
    
}

//________________________________________________________
// Set the appropriate coordinate information for J2000 coordinates
void FitsTemplateModel::SetCoordGalactic()
{
    // Check if the coordinate system is already in J2000 coordinates
    if (coord_system_.compare("GALACTIC") == 0) return ;
    
    // find the new center position if the coord_system_ has changed
    double new_xref(wcInfo->xref), new_yref(wcInfo->yref) ;
    
    // Convert the coordinates from whatever they were to the new coordinates
    ConvertCoordinates(&new_xref, &new_yref, coord_system_, "GALACTIC") ;
    
    // Get the new coordinates
    char ctype1[9], ctype2[9] ;
    std::snprintf(ctype1, 9, "GLON-ARC") ;
    std::snprintf(ctype2, 9, "GLAT-ARC") ;
    
    WorldCoor* old_wcInfo = wcInfo ;
    wcInfo = wcskinit(wcInfo->nxpix,	/* Number of pixels along x-axis */
                      wcInfo->nypix,	/* Number of pixels along y-axis */
                      ctype1,	/* FITS WCS projection for axis 1 */
                      ctype2,	/* FITS WCS projection for axis 2 */
                      wcInfo->xrefpix,	/* Reference pixel coordinates */
                      wcInfo->yrefpix,	/* Reference pixel coordinates */
                      new_xref,	/* Coordinate at reference pixel in degrees */
                      new_yref,	/* Coordinate at reference pixel in degrees */
                      wcInfo->cd,	/* Rotation matrix, used if not NULL */
                      wcInfo->cdelt[0],	/* scale in degrees/pixel, if cd is NULL */
                      wcInfo->cdelt[1],	/* scale in degrees/pixel, if cd is NULL */
                      wcInfo->rot,	/* Rotation angle in degrees, if cd is NULL */
                      wcInfo->equinox, /* Equinox of coordinates, 1950 and 2000 supported */
                      wcInfo->epoch);

    // Reset the coordinates
    coord_system_ = "GALACTIC" ;
    
    // delete
    delete old_wcInfo ;
    
    return ;
}

//________________________________________________________
// Set the appropriate coordinate information for ECLIPTIC coordinates
void FitsTemplateModel::SetCoordEcliptic()
{
    // Until we have the coordinate conversion setup, we cant do anything with this class
    std::cerr << "[ERROR] Cannot convert to ECLIPTIC coordinates at this time!" << std::endl;
    return ;
    
    // Check if the coordinate system is already in J2000 coordinates
    if (coord_system_.compare("ECLIPTIC") == 0) return ;
    
    // find the new center position if the coord_system_ has changed
    double new_xref(wcInfo->xref), new_yref(wcInfo->yref) ;
    
    // Convert the coordinates from whatever they were to the new coordinates
    ConvertCoordinates(&new_xref, &new_yref, coord_system_, "ECLIPTIC") ;
    
    // Get the new coordinates
    char ctype1[9], ctype2[9] ;
    std::snprintf(ctype1, 9, "ELON-TAN") ;
    std::snprintf(ctype2, 9, "ELAT-TAN") ;
    
    // Redirect the current wcInfo so we can safely delete it later
    WorldCoor* old_wcInfo = wcInfo ;
    
    // Initialize the new wcInfo object
    wcInfo = wcskinit(wcInfo->nxpix,	/* Number of pixels along x-axis */
                      wcInfo->nypix,	/* Number of pixels along y-axis */
                      ctype1,	/* FITS WCS projection for axis 1 */
                      ctype2,	/* FITS WCS projection for axis 2 */
                      wcInfo->xrefpix,	/* Reference pixel coordinates */
                      wcInfo->yrefpix,	/* Reference pixel coordinates */
                      new_xref,	/* Coordinate at reference pixel in degrees */
                      new_yref,	/* Coordinate at reference pixel in degrees */
                      wcInfo->cd,	/* Rotation matrix, used if not NULL */
                      wcInfo->cdelt[0],	/* scale in degrees/pixel, if cd is NULL */
                      wcInfo->cdelt[1],	/* scale in degrees/pixel, if cd is NULL */
                      wcInfo->rot,	/* Rotation angle in degrees, if cd is NULL */
                      wcInfo->equinox, /* Equinox of coordinates, 1950 and 2000 supported */
                      wcInfo->epoch);
    
    // Reset the coordinates
    coord_system_ = "ECLIPTIC" ;
    
    // delete
    delete old_wcInfo ;
    
    return ;
}

//________________________________________________________
// Returns true if the data was actually saved to a file
bool FitsTemplateModel::SaveFits(const std::string& filename, bool overwrite)
{
    if (filename.compare("")!=0) {
        fits_file_path_ = filename ;
    } else {
        std::cerr << "[ERROR] FitsTemplateModel::SaveFits() supplied filename is empty!" << std::endl;
        return false ;
    }
    
    // Check whether the file exists already
    if (!overwrite) {
        // Check whether the file exists
        if (FILE *file = std::fopen(fits_file_path_.c_str(), "r")) {
            fclose(file) ;
            std::cerr << "[ERROR] Attempt to save to file failed, as file ("<< fits_file_path_
                      << ") already exists. Use 'SaveFits(filename, true)' to overwrite existing file."
                      << std::endl;
            return false ;
        }
    }
    
    // Essentially, what we're going to do is to duplicate this fits object
    // and save THAT to the file. The code below here was duplicated from the
    // sample code provided at : http://heasarc.nasa.gov/fitsio/ccfits/html/writeimage.html
    
    // Create a FITS primary array containing a 2-D image
    // declare axis arrays.
    long naxis    = wcInfo->naxes ;
    long naxes[2] = { static_cast<long>(wcInfo->nxpix+0.5), static_cast<long>(wcInfo->nypix+0.5) };
    
    // declare auto-pointer to FITS at function scope. Ensures no resources
    // leaked if something fails in dynamic allocation.
    std::shared_ptr<CCfits::FITS> pFits(0);
    
    try {
        // overwrite existing file if the file already exists.
        const std::string fileName("!"+fits_file_path_) ;
        
        // Create a new FITS object, specifying the data type and axes for the primary
        // image. Simultaneously create the corresponding file.
        
        // this image is unsigned short data, demonstrating the cfitsio extension
        // to the FITS standard.
        
        pFits.reset( new CCfits::FITS(fileName, DOUBLE_IMG, naxis, naxes) );
    } catch (CCfits::FITS::CantCreate) {
        // ... or not, as the case may be.
        return false ;
    }
    
    // references for clarity.
    long& vectorLength = naxes[0];
    long& numberOfRows = naxes[1];
    long nelements(naxes[0] * naxes[1]);
    
    // Add all of the keys to the fits object header
    AddKeys(pFits) ;
    
    // The function PHDU& FITS::pHDU() returns a reference to the object representing
    // the primary HDU; PHDU::write( <args> ) is then used to write the data.
    try {
        pFits->pHDU().write(1,nelements,image_info_);
    } catch (CCfits::FitsError & e) {
        std::cerr << e.message() << std::endl;
        return false ;
    }
    
    // PHDU's friend ostream operator. Doesn't print the entire array, just the
    // required & user keywords, and is provided largely for testing purposes [see
    // readImage() for an example of how to output the image array to a stream].
    std::cout << pFits->pHDU() << std::endl;
    
    return true ;
}


//________________________________________________________
FitsTemplateModel& FitsTemplateModel::operator=(const FitsTemplateModel& rhs)
{
    // Check whether we're trying to set myself equal to myself. If so, then
    // just return this object
    if (this == &rhs) return *this ;
    
    // Check that the wcInfo object is nullptr
    if (wcInfo != nullptr) {
        // safely redirect and delete
        WorldCoor* wcInfo_old = wcInfo ;
        wcInfo = nullptr ;
        delete wcInfo_old ;
    }
    
    // Because wcInfo is a struct and does not have a copy constructor, I need
    // to make sure that all the values are set by hand in case rhs is deleted.
    WorldCoor* rhsWC = rhs.GetWorldCoor() ;
    wcInfo = wcskinit(rhsWC->nxpix,	/* Number of pixels along x-axis */
                      rhsWC->nypix,	/* Number of pixels along y-axis */
                      rhsWC->ctype[0],	/* FITS WCS projection for axis 1 */
                      rhsWC->ctype[1],	/* FITS WCS projection for axis 2 */
                      rhsWC->xrefpix,	/* Reference pixel coordinates */
                      rhsWC->yrefpix,	/* Reference pixel coordinates */
                      rhsWC->xref,	/* Coordinate at reference pixel in degrees */
                      rhsWC->yref,	/* Coordinate at reference pixel in degrees */
                      rhsWC->cd,	/* Rotation matrix, used if not NULL */
                      rhsWC->cdelt[0],	/* scale in degrees/pixel, if cd is NULL */
                      rhsWC->cdelt[1],	/* scale in degrees/pixel, if cd is NULL */
                      rhsWC->rot,	/* Rotation angle in degrees, if cd is NULL */
                      rhsWC->equinox, /* Equinox of coordinates, 1950 and 2000 supported */
                      rhsWC->epoch);
    
    // Copy values from the new object into this instance
    coord_system_ = rhs.GetCoordSystem() ;

    // Now handle the image info object by first deleting all of its objects
    image_info_ = rhs.GetImageInfo() ;
    
    // Return *this
    return *this ;
}

//________________________________________________________
// Adds another Fits template to this one
FitsTemplateModel& FitsTemplateModel::operator+=(const FitsTemplateModel& rhs)
{
    return ApplyOperator(rhs, std::plus<double>()) ;
}

//________________________________________________________
// Subtracts another fits template from this one
FitsTemplateModel& FitsTemplateModel::operator-=(const FitsTemplateModel& rhs)
{
    return ApplyOperator(rhs, std::minus<double>()) ;
}

//________________________________________________________
// Multiplies this template by another one
FitsTemplateModel& FitsTemplateModel::operator*=(const FitsTemplateModel& rhs)
{
    return ApplyOperator(rhs, std::multiplies<double>()) ;
}

//________________________________________________________
// Divides this template by another one
FitsTemplateModel& FitsTemplateModel::operator/=(const FitsTemplateModel& rhs)
{
    return ApplyOperator(rhs, std::divides<double>()) ;
}

//________________________________________________________
// Addition operator
const FitsTemplateModel FitsTemplateModel::operator+(const FitsTemplateModel& rhs)
{
    
    FitsTemplateModel ret = *this ; // Create a copy of this object
    ret += rhs ;                    // Add the passed object to it
    return ret ;                    // Return the new object
}

//________________________________________________________
// Subtraction operator
const FitsTemplateModel FitsTemplateModel::operator-(const FitsTemplateModel& rhs)
{
    FitsTemplateModel ret = *this ; // Create a copy of this object
    ret -= rhs ;                    // Subtract the passed object from it
    return ret ;                    // Return the new object
}

//________________________________________________________
// Multiplication operator
const FitsTemplateModel FitsTemplateModel::operator*(const FitsTemplateModel& rhs)
{
    FitsTemplateModel ret = *this ; // Create a copy of this object
    ret *= rhs ;                    // Multiply the new object by the passed one
    return ret ;                    // Return the new object
}

//________________________________________________________
// Division operator
const FitsTemplateModel FitsTemplateModel::operator/(const FitsTemplateModel& rhs)
{
    FitsTemplateModel ret = *this ; // Create a copy of this object
    ret /= rhs ;                    // Divide the new object by the passed one
    return ret ;                    // Return the new object
}


//________________________________________________________
double FitsTemplateModel::Integral() const
{
    // Sum all of the bin values of this histogram
    return std::accumulate(std::begin(image_info_), std::end(image_info_), 0.0) ;
}

//________________________________________________________
void FitsTemplateModel::Normalize()
{
    // Divide each bin by the integral of the entire histogram
    const double norm_factor = Integral() ; // Calculate the normalization factor
    Scale(1.0/norm_factor) ;                // Scale each bin by this value
    return ;
}

//________________________________________________________
// The goal of this is to rotate the image counterclockwise by the
// specified number of degrees. For example, calling Rotate(90) on
// the following map will have the following effect:
//      0 1 0                     0 4 0
//      2 3 4   -> Rotate(90) ->  1 3 5
//      0 5 0                     0 2 0
void FitsTemplateModel::SetRotation(const double rotation_deg)
{
    // Get the current rotation of the coordinate system relative to the
    // image
    
    // TODO: Add method to get the current rotation angle so we can update it instead of setting it.
    // Increment the rotation of the map by the specified number of degrees.
    wcsdeltset( wcInfo, wcInfo->cdelt[0], wcInfo->cdelt[1], rotation_deg) ;
//    wcInfo->rot += rotation_deg ;
}


//________________________________________________________
void FitsTemplateModel::Scale(const double multiplier)
{
    // Multiply every element by the supplied multiplier
    image_info_ *= multiplier ;
}


#pragma mark - Protected Methods

//________________________________________________________
template <class BinaryOperation>
FitsTemplateModel& FitsTemplateModel::ApplyOperator(const FitsTemplateModel& rhs, BinaryOperation binary_op)
{
    // Loop through every bin in the image_info_ object
    double rhs_value(0.0), xcoord(0.0), ycoord(0.0) ;
    int bin(0) ;
    for (int xbin=0; xbin<wcInfo->nxpix; ++xbin) {
        for (int ybin=0; ybin<wcInfo->nypix; ++ybin) {
            // Get the x,y position of this bin in the coordinate system
            pix2wcs(wcInfo, xbin+0.5, ybin+0.5, &xcoord, &ycoord) ;
            
            // Get the value of rhs at the bin position
            bin = (wcInfo->nxpix*ybin) + xbin ;
            image_info_[bin] = binary_op(image_info_[bin],rhs.EvaluateAt(xcoord, ycoord, coord_system_)) ;
        }
    }
    
    return *this ;
}

//________________________________________________________
void FitsTemplateModel::AddKeys(std::shared_ptr<CCfits::FITS> fits_object)
{
    if (fits_object == nullptr) return ;
    
    InitMandatoryFitsHeaderKeywords(fits_object) ;      // Add mandatory fits keywords
    InitUserDefinedFitsHeaderKeywords(fits_object) ;    // Add in user defined keywords
    InitProgramFitsHeaderKeywords(fits_object) ;        // Add keywords defined by this class
}

//________________________________________________________
// Set "mandatory" keywords demanded by the fits standard:
// http://heasarc.gsfc.nasa.gov/docs/fcg/standard_dict.html
void FitsTemplateModel::InitMandatoryFitsHeaderKeywords(std::shared_ptr<CCfits::FITS> fits_object)
{
    if (fits_object == nullptr) return ;
    
    // Add mandatory keywords
    fits_object->pHDU().addKey("GCOUNT", 1, "group count") ;
    fits_object->pHDU().addKey("GROUPS", false, "indicates random groups structure") ;
    fits_object->pHDU().addKey("PCOUNT", 0, "parameter count") ;
    fits_object->pHDU().addKey("SIMPLE", true, "does file conform to standard") ;
    
    // Mandatory keywords associated with image info
    fits_object->pHDU().addKey("TBCOL1", 1, "beginning column number") ;
    fits_object->pHDU().addKey("TFIELDS", 0, "unused") ;
    fits_object->pHDU().addKey("TFORM1", "", "unused") ;
}

//________________________________________________________
void FitsTemplateModel::InitUserDefinedFitsHeaderKeywords(std::shared_ptr<CCfits::FITS> fits_object)
{
    if (fits_object == nullptr) return ;
    
    // TODO: Add functionality to allow users to add their own fits keywords
}

//________________________________________________________
void FitsTemplateModel::InitProgramFitsHeaderKeywords(std::shared_ptr<CCfits::FITS> fits_object)
{
    if (fits_object == nullptr) return ;
    // Update coordinate information
    fits_object->pHDU().addKey("CTYPE1", std::string(wcInfo->ctype[0]), "name of x coordinate axis") ;
    fits_object->pHDU().addKey("CTYPE2", std::string(wcInfo->ctype[1]), "name of y coordinate axis") ;
    fits_object->pHDU().addKey("CRPIX1", wcInfo->xrefpix, "center pixel in x") ;
    fits_object->pHDU().addKey("CRPIX2", wcInfo->yrefpix, "center pixel in y") ;
    fits_object->pHDU().addKey("CRVAL1", wcInfo->xref, "center position in x") ;
    fits_object->pHDU().addKey("CRVAL2", wcInfo->yref, "center position in y") ;
    fits_object->pHDU().addKey("CDELT1", wcInfo->cdelt[0], "bin size in x") ;
    fits_object->pHDU().addKey("CDELT2", wcInfo->cdelt[1], "bin size in y") ;
    
    // Rotation information
    fits_object->pHDU().addKey("CROTA1", wcInfo->rot, "rotation in degrees") ;
    fits_object->pHDU().addKey("CROTA2", wcInfo->rot, "rotation in degrees") ;
    fits_object->pHDU().addKey("CD1_1", wcInfo->cd[0], "rotation matrix 1") ;
    fits_object->pHDU().addKey("CD1_2", wcInfo->cd[1], "rotation matrix 2") ;
    fits_object->pHDU().addKey("CD2_1", wcInfo->cd[2], "rotation matrix 3") ;
    fits_object->pHDU().addKey("CD2_2", wcInfo->cd[3], "rotation matrix 4") ;
    
    // Update axis counters (number of axes, number of bins, etc ...)
    fits_object->pHDU().addKey("NAXIS", wcInfo->naxes, "number of axes") ;
    fits_object->pHDU().addKey("NAXIS1", long(wcInfo->nxpix), "number of columns (x-bins)") ;
    fits_object->pHDU().addKey("NAXIS2", long(wcInfo->nypix), "number of rows (y-bins)") ;
    
    // Any others?
}
