//
//  FitsTemplateModel.h
//  FitsTemplateMaker
//
//  Created by Josh Cardenzana on 1/5/16.
//  Copyright © 2016 JCardenzana. All rights reserved.
//

#ifndef FitsTemplateModel_h
#define FitsTemplateModel_h

#include <stdio.h>
#include <vector>

class FitsTemplateModel {
public :
    // Default constructor
    FitsTemplateModel() {} ;
    // Constructor based on simple information. Note that if 'parameters' is left empty then
    // 'name' is assumed to point to a fits file from which the template will be extracted
    FitsTemplateModel(const std::string& name, const std::vector<double> parameters=std::vector<double>(0)) ;
    // Copy constructor
    FitsTemplateModel(const FitsTemplateModel& other) ;
    // Destructor
    virtual ~FitsTemplateModel() {} ;
    
    // Outputs the fits file based on a filename provided
    bool saveFITS(const std::string& filename) ;
    
    // Overloaded methods for equating, subtracting, adding, multiplying, and dividing FITS templates
    FitsTemplateModel& operator=(const FitsTemplateModel& rhs) ;
    FitsTemplateModel& operator+=(const FitsTemplateModel& rhs) ;
    FitsTemplateModel& operator-=(const FitsTemplateModel& rhs) ;
    FitsTemplateModel& operator*=(const FitsTemplateModel& rhs) ;
    FitsTemplateModel& operator/=(const FitsTemplateModel& rhs) ;
    const FitsTemplateModel operator+(const FitsTemplateModel& rhs) ;
    const FitsTemplateModel operator-(const FitsTemplateModel& rhs) ;
    const FitsTemplateModel operator*(const FitsTemplateModel& rhs) ;
    const FitsTemplateModel operator/(const FitsTemplateModel& rhs) ;
    
protected :
    
private :
    
};

#endif /* FitsTemplateModel_h */
