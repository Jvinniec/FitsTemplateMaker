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

#include <stdio.h>
#include <string>
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
    FitsTemplateModel(const FitsTemplateModel& other) ;
    // Destructor
    virtual ~FitsTemplateModel() {} ;
    
    // Read the model from a file
    virtual void ReadModelFromFile(const std::string& filename) ;
    
    // "Get" or "set" the "model_name_" for this template
    std::string getModelName() {return model_name_ ;}
    void setModelName(const std::string& modelname) {model_name_ = modelname ;}
    
    // Outputs the fits file based on a filename provided
    bool saveFITS(const std::string& filename) ;
    
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
    
protected :
    // name of the object represented by this template
    std::string model_name_ ;
    // Variable holding the information on the model type
    std::string model_type_ ;
    // Vector holding the passed parameters for the model requested
    std::vector<double> model_params_ ;
    
    /// Path to the FITS file being handled by this instance.
    std::string FitsFilePath;
    
    // WCS related objects
    WorldCoor* wcInfo ;                 // image coordinate information
    
    // Fits related objects
    std::unique_ptr<CCfits::FITS> fitsObj ;
    
private :
    
};

#endif /* FitsTemplateModel_h */
