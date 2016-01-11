//
//  FitsTemplateModel.h
//  FitsTemplateMaker
//
//  Created by Josh Cardenzana on 1/5/16.
//  Copyright © 2016 JCardenzana. All rights reserved.
//
//  Some of this code was adapted from a script written by a collegue of
//  mine, Hugh Dickinson. Specifically the parts written that make use of
//  CCfits objects. Thank you so much, Hugh! You helped me understand a
//  alot


#ifndef FitsTemplateMaker_FitsTemplateModel_h
#define FitsTemplateMaker_FitsTemplateModel_h

#include <map>
#include <stdio.h>
#include <string>
#include <valarray>
#include <vector>

// CCfits Headers
#include "CCfits/FITS.h"
#include "CCfits/FITSBase.h"
#include "CCfits/HDU.h"
#include "CCfits/PHDUT.h"
#include "CCfits/Table.h"
#include "CCfits/ColumnT.h"
#include "CCfits/KeywordT.h"
#include "CCfits/FITSUtilT.h"

// libwcs Headers
#include "wcs.h"
#include "fitshead.h"
#include "fitsfile.h"

class FitsTemplateModel {
public :
    // Default constructor
    FitsTemplateModel() ;
    // Constructor based on simple information. Note that if 'parameters' is left empty then
    // 'name' is assumed to point to a fits file from which the template will be extracted
    FitsTemplateModel(const std::string& filename) ;
    // Copy constructor
    FitsTemplateModel(const FitsTemplateModel&other, const std::string new_file_name=std::string()) ;
    // Destructor
    virtual ~FitsTemplateModel() ;
    
    // Methods for adding keys to the fits file
    void AddKey(const std::string& key, const std::string& description, const double value) ;
    void AddKey(const std::string& key, const std::string& description, const std::string& value) ;
    
    // Methods for getting the key value and the key description
    std::string GetKeyDescription(const std::string& key) {return std::string() ;}
    
    // Method for initializing the various model parameters
    void GetCoordinateSystem(WorldCoor* worldCoor) ;
    
    // Read the model from a file
    virtual void InitModelFromFile(const std::string& filename) ;
    
    // Method for querying the model at a given position
    virtual double EvaluateAt(const double xpos, const double ypos, const std::string& coordsystem) const ;
    
    // "Get" or "set" the "model_name_" for this template
    std::string GetModelName() {return model_name_ ;}
    void SetModelName(const std::string& modelname) {model_name_ = modelname ;}
    
    // Various "getters" for protected data members
    virtual std::string GetCoordX() const {return std::string(wcInfo->ctype[0]) ;}
    virtual std::string GetCoordY() const {return std::string(wcInfo->ctype[1]) ;}
    virtual std::string GetCoordSystem() const {return coord_system_ ;}
    virtual std::string GetFitsFilePath() const {return fits_file_path_ ;}
    virtual std::valarray<double> GetImageInfo() const {return image_info_ ;}
    virtual WorldCoor* GetWorldCoor() const {return wcInfo ;}
    virtual long GetNumColumns() const {return wcInfo->nxpix ;}
    virtual long GetNumRows() const {return wcInfo->nypix ;}
    
    // Various "setters" for changing the coordinate system
    // NOTE: DO NOT USE THESE ON EXISTING FITS FILES OR FILLED FITS TEMPLATES! In the
    //      future this may be an option, but at the moment there is an issue in which
    //      the rotation matrix is not correctly propagated, and thus the orientation
    //      of the image is not maintained.
    virtual void SetCoordJ2000() ;
    virtual void SetCoordB1950() ;
    virtual void SetCoordGalactic() ;
    virtual void SetCoordEcliptic() ;
    
    // Set the path string for the location of the file from which this fits object was extracted
    virtual void SetFitsFilePath(const std::string& new_fits_file_path) {fits_file_path_ = new_fits_file_path ;}
    
    // Outputs the fits file based on a filename provided
    virtual bool SaveFits(const std::string& filename, bool overwrite=false) ;
    
    // Overloaded methods for equating, subtracting, adding, multiplying, and dividing FITS templates
    virtual FitsTemplateModel& operator=(const FitsTemplateModel& rhs) ;
    virtual FitsTemplateModel& operator+=(const FitsTemplateModel& rhs) ;
    virtual FitsTemplateModel& operator-=(const FitsTemplateModel& rhs) ;
    virtual FitsTemplateModel& operator*=(const FitsTemplateModel& rhs) ;
    virtual FitsTemplateModel& operator/=(const FitsTemplateModel& rhs) ;
    virtual const FitsTemplateModel operator+(const FitsTemplateModel& rhs) ;
    virtual const FitsTemplateModel operator-(const FitsTemplateModel& rhs) ;
    virtual const FitsTemplateModel operator*(const FitsTemplateModel& rhs) ;
    virtual const FitsTemplateModel operator/(const FitsTemplateModel& rhs) ;
    
    // Mathematical methods for getting various properties of the image, or manipulating
    // its contents
    virtual double Integral() const ;
    virtual void Normalize() ;
    virtual void SetRotation(const double rotation_deg) ;
    virtual void Scale(const double multiplier) ;
    
protected :
    std::string model_name_ ;           // name of the object represented by this template
    std::string model_type_ ;           // Variable holding the information on the model type
    std::string coord_system_ ;         // Model coordinate system
    std::vector<double> model_params_ ; // Vector holding the passed parameters for the model requested
    std::string fits_file_path_ ;       // Path to the FITS file being handled by this instance.
    
    /**
     * The following variables are used in conjunction with the "EvaluateAt" class. They will be filled
     * the first time "evaluateAt" is called. This means there is a small amount of overhead computation
     * time the first time "evaluateAt" is called, but it reduces the computation time of subsequent
     * calls while also not filling unnecessary parameter should the user never need to use this method.
     */
    std::valarray<double> image_info_ ; // image data information
    WorldCoor* wcInfo ;                 // image coordinate information
    
    // Map of keys. The map has the form <"KEY", <"description", value> >
    std::map<std::string, std::pair<std::string, double> > list_of_keys_ ;
    
    // This method does the bulk of the work and will need to be uniquely implemented for any inheriting class.
    // Note that for this class it doesnt do anything as this class should only be used for filling from a
    // pre-existing fits file
    virtual void FillImageData() {} ;
    
    // Applies a given math operation between bins of two maps
    template <class BinaryOperation>
    FitsTemplateModel& ApplyOperator(const FitsTemplateModel& rhs, BinaryOperation binary_op) ;
    
    // Methods for updating the fits header keywords when writing a new fits file
    virtual void AddKeys(std::shared_ptr<CCfits::FITS> fits_object) ;
    virtual void InitMandatoryFitsHeaderKeywords(std::shared_ptr<CCfits::FITS> fits_object) ;
    virtual void InitUserDefinedFitsHeaderKeywords(std::shared_ptr<CCfits::FITS> fits_object) ;
    virtual void InitProgramFitsHeaderKeywords(std::shared_ptr<CCfits::FITS> fits_object) ;
    
private :
    
};

#endif /* FitsTemplateModel_h */
