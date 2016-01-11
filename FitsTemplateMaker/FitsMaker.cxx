//
//  FitsMaker.cxx
//  FitsTemplateMaker
//
//  Created by Josh Cardenzana on 1/5/16.
//  Copyright © 2016 JCardenzana. All rights reserved.
//
//=======================================================================
// CODE DESCRIPTION
//    This is meant to serve as a very light weight production tool
//    for generating FITS template files. It serves as a command line
//    tool allowing for fits file generation based on simple qualifiers
//    such as "gaussian" or "disk" as well as some qualitative parameters
//    describing the object. More details to follow.
//=======================================================================

#include <iostream>

#include "FitsTemplateModel.h"


//________________________________________________________
int main(int argc, const char * argv[])
{
    // Check if we have enough parameters
    if (argc < 3 ) {
        std::cout << "USAGE: FitsMaker <inputfile> <outputfile>" << std::endl;
        return 0 ;
    }
    
    std::string input_file(argv[1]) ;
    std::string output_file(argv[2]) ;
    
    // Open the fits file
    std::cout << "Reading fits file: " << input_file << std::endl;
    FitsTemplateModel* model = new FitsTemplateModel(input_file) ;
    
    // Modify the model in some way
    //model->SetCoordJ2000() ;
//    *model -= *model ;
    FitsTemplateModel* model2 = new FitsTemplateModel(*model) ;
    
    FitsTemplateModel model3 = *model + *model ;
    
    //model->Rotate(45.0) ;
    // Save the fits file
    std::cout << "Saving new fits file: " << output_file << std::endl;
    model3.SaveFits(std::string(argv[2]), true) ;
    
    delete model ;
    delete model2 ;
    
    return 0 ;
}
