//
//  DiskFitsTemplateModel.h
//  FitsTemplateMaker
//
//  Created by Josh Cardenzana on 1/5/16.
//  Copyright © 2016 JCardenzana. All rights reserved.
//

#ifndef FitsTemplateMaker_DiskFitsTemplateModel_h
#define FitsTemplateMaker_DiskFitsTemplateModel_h

#include <stdio.h>
#include "FitsTemplateModel.h"

class DiskFitsTemplateModel : public FitsTemplateModel {
public :
    // Piggy back off of the base class' default constructor
    using FitsTemplateModel::FitsTemplateModel ;
    // Constructor from a disk radius
    
    // Destructor
    virtual ~DiskFitsTemplateModel() {} ;
    
protected :
    
private :
    
};

#endif /* DiskFitsTemplateModel_h */
