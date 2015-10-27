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

#include <math.h>
#include <stdio.h>
#include <sstream>
//#include "indri/RMExpander.hpp"
#include "../include/modRMExpander.hpp"

indri::query::modRMExpander::modRMExpander( indri::api::QueryEnvironment * env , indri::api::Parameters& param ) : indri::query::QueryExpander( env, param ) { }

std::string indri::query::modRMExpander::expand( std::string originalQuery , std::vector<indri::api::ScoredExtentResult>& results )
{
    return "";
}

std::vector< std::pair<std::string, double> > indri::query::modRMExpander::modExpand( std::string originalQuery , std::vector<indri::api::ScoredExtentResult>& results ) {
  int fbDocs = _param.get( "fbDocs" , 10 );
  int fbTerms = _param.get( "fbTerms" , 10 );
  double fbOrigWt = _param.get( "fbOrigWeight", 0.5 );
  double mu = _param.get( "fbMu", 0 );

  std::string rmSmoothing = "";
  if (mu != 0) // specify dirichlet smoothing
    rmSmoothing = "method:dirichlet,mu:" + (std::string)_param.get( "fbMu", "0");

  // this should be a parameter, have to change the generation to
  // account for phrases then.
  int maxGrams = 1;

  indri::query::RelevanceModel rm(*_env, rmSmoothing, maxGrams, fbDocs);
  rm.generate( originalQuery, results );

  const std::vector<indri::query::RelevanceModel::Gram*>& grams = rm.getGrams();
  std::vector< std::pair<std::string, double> > probs;
  // sorted grams came from rm
  for( size_t j=0; j<grams.size(); j++ ) {
    double w = grams[j]->weight;
    // multi terms need to be handled if maxGrams becomes a parameter
    std::string &term = grams[j]->terms[0];
    probs.push_back( std::pair<std::string, double>( term, w ) );
  }
  return probs;
}

