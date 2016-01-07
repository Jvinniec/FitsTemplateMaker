//
//  GaussianFitsTemplateModel.h
//  FitsTemplateMaker
//
//  Created by Josh Cardenzana on 1/5/16.
//  Copyright © 2016 JCardenzana. All rights reserved.
//

#ifndef FitsTemplateMaker_GaussianFitsTemplateModel_h
#define FitsTemplateMaker_GaussianFitsTemplateModel_h

#include <stdio.h>

// PROJECT HEADERS
#include "FitsTemplateModel.h"

class GaussianFitsTemplateModel : public FitsTemplateModel {
public :
    // Use the same default constructor as the parent class
    using FitsTemplateModel::FitsTemplateModel ;
    // Constructor from Gaussian parameters
    
    // Destructor
    virtual ~GaussianFitsTemplateModel() {} ;
protected :

private :
};

#endif /* GaussianFitsTemplateModel_h */
