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
// modRMExpander
//
// 18 Aug 2004 -- dam
// modified for lce at
// 13 Jun 2015 -- sbk
//

#ifndef INDRI_modRMExpander_HPP
#define INDRI_modRMExpander_HPP

#include <string>
#include <vector>
#include <map>

#include "indri/QueryExpander.hpp"
#include "indri/QueryEnvironment.hpp"
#include "indri/Parameters.hpp"
#include "indri/RelevanceModel.hpp"
namespace indri
{
  namespace query
  {

    class modRMExpander : public QueryExpander  {
    public:
      modRMExpander( indri::api::QueryEnvironment * env , indri::api::Parameters& param );

      std::vector< std::pair<std::string, double> > modExpand( std::string originalQuery , std::vector<indri::api::ScoredExtentResult>& results );
      virtual std::string expand( std::string originalQuery , std::vector<indri::api::ScoredExtentResult>& results );
    };
  }
}

#endif
