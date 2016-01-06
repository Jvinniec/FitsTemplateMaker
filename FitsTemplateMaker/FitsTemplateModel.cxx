//
//  FitsTemplateModel.cxx
//  FitsTemplateMaker
//
//  Created by Josh Cardenzana on 1/5/16.
//  Copyright © 2016 JCardenzana. All rights reserved.
//

#include "FitsTemplateModel.h"

#pragma Mark - Constructors

//________________________________________________________
FitsTemplateModel::FitsTemplateModel()
{
}

//________________________________________________________
// Basic constructor from a filename
FitsTemplateModel::FitsTemplateModel(const std::string& filename)
{
}


//________________________________________________________
// Copy constructor
FitsTemplateModel::FitsTemplateModel(const FitsTemplateModel& other)
{
}

#pragma Mark - Public Methods

//________________________________________________________
// Extract a predefined model from a fits file
void FitsTemplateModel::ReadModelFromFile(const std::string& filename)
{
    // Attempt to open the specified fits file.
    try {
        fitsObj.reset(new CCfits::FITS(filename));
    } catch (CCfits::FITS::CantOpen & e) {
        std::cerr << e.message() << std::endl ;
    }
    
    // Now fill the internal storage object
}

//________________________________________________________
bool FitsTemplateModel::saveFITS(const std::string& filename)
{
    return false ;
}


//________________________________________________________
FitsTemplateModel& FitsTemplateModel::operator=(const FitsTemplateModel& rhs)
{
    // Check whether we're trying to set myself equal to myself. If so, then
    // just return this object
    if (this == &rhs) return *this ;
    
    // Deallocate local memory used internally
    
    // Allocate memory to hold the contents of the new object
    
    // Copy values from the new object into this instance
    
    // Return *this
    return *this ;
}

//________________________________________________________
// Adds another Fits template to this one
FitsTemplateModel& FitsTemplateModel::operator+=(const FitsTemplateModel& rhs)
{
    return *this ;
}

//________________________________________________________
// Subtracts another fits template from this one
FitsTemplateModel& FitsTemplateModel::operator-=(const FitsTemplateModel& rhs)
{
    return *this ;
}

//________________________________________________________
// Multiplies this template by another one
FitsTemplateModel& FitsTemplateModel::operator*=(const FitsTemplateModel& rhs)
{
    return *this ;
}

//________________________________________________________
// Divides this template by another one
FitsTemplateModel& FitsTemplateModel::operator/=(const FitsTemplateModel& rhs)
{
    return *this ;
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

#pragma Mark - Protected Methods

