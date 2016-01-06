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
FitsTemplateModel::FitsTemplateModel(const std::string& type, std::vector<double> parameters)
{
    
}


//________________________________________________________
FitsTemplateModel::FitsTemplateModel(const FitsTemplateModel& other)
{
}

#pragma Mark - Public Methods

//________________________________________________________
bool saveFITS(const std::string& filename)
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
FitsTemplateModel& FitsTemplateModel::operator+=(const FitsTemplateModel& rhs)
{
    return *this ;
}

//________________________________________________________
FitsTemplateModel& FitsTemplateModel::operator-=(const FitsTemplateModel& rhs)
{
    return *this ;
}

//________________________________________________________
FitsTemplateModel& FitsTemplateModel::operator*=(const FitsTemplateModel& rhs)
{
    return *this ;
}

//________________________________________________________
FitsTemplateModel& FitsTemplateModel::operator/=(const FitsTemplateModel& rhs)
{
    return *this ;
}

//________________________________________________________
const FitsTemplateModel FitsTemplateModel::operator+(const FitsTemplateModel& rhs)
{
    
    FitsTemplateModel ret = *this ; // Create a copy of this object
    ret += rhs ;                    // Add the new object to it
    return ret ;                    // Return the new object
}

//________________________________________________________
const FitsTemplateModel FitsTemplateModel::operator-(const FitsTemplateModel& rhs)
{
    FitsTemplateModel ret = *this ;
    ret -= rhs ;
    return ret ;
}

//________________________________________________________
const FitsTemplateModel FitsTemplateModel::operator*(const FitsTemplateModel& rhs)
{
    
}

//________________________________________________________
const FitsTemplateModel FitsTemplateModel::operator/(const FitsTemplateModel& rhs)
{
    
}

#pragma Mark - Protected Methods

