/*==========================================================================
 * Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */

//
// mainProgram
//
// 6 June 2015 -- sbk
//

#include "../include/CoordinateAscent.hpp"

void crossValidation(indri::api::Parameters & param )
{
    if(!param.exists("foldCount"))
        throw runtime_error("foldCount doesn't exist in the parameter object.");
    if(!param.exists("foldNum"))
        throw runtime_error("foldNum doesn't exist in the parameter object. The value of this parameter may not be used but should be initialized to avoid confusion.");

    if(!param.exists("query"))
        throw runtime_error("query doesn't exist in the parameter object. The value of this parameter may not be used but should be initialized to avoid confusion.");

    string paramTemp;
    param.write(paramTemp);

    size_t foldCount = param.get("foldCount", 0);
    for (size_t foldNum = 0; foldNum < foldCount; foldNum++)
    {
        cout << "cross-validation: training phrase: " << endl;
        cout << "cross-validation: foldNum = " << param.get("foldNum", 0) << endl;
        param.clear();
        param.load(paramTemp);
        param.set("foldNum", foldNum);
        indri::query::CoordinateAscent coordinateAscent_train(param);

        cout << "cross-validation: testing phrase: " << endl;
        cout << "cross-validation: foldNum = " << param.get("foldNum", 0) << endl;
//        param.clear();
//        param.load(paramTemp);
        param.set("train",false);
        param.set("foldNum", foldNum);
        indri::query::CoordinateAscent coordinateAscent_test(param);
    }
}


int main(int argc, char * argv[])
{

    try
    {
        indri::api::Parameters& param = indri::api::Parameters::instance();

        param.loadCommandLine( argc, argv );

        if(param.get("train", false) == false) // do we need to test or there is also a need for training phase
        {
            indri::query::CoordinateAscent coordinateAscent_(param);
        }
        else
        {
            crossValidation(param);
        }
    }
    catch( lemur::api::Exception& e )
    {
        LEMUR_ABORT(e);
    }
    catch(std::exception const& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }
    catch( ... )
    {
        std::cout << "Caught unhandled exception" << std::endl;
        return -1;
    }
    return 0;
}
