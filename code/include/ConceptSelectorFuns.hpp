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
// ConceptSelectorFuns
//
// 1 Jul 2015 -- sbk
//

#ifndef CONCEPTSELECTORFUNS_HPP
#define CONCEPTSELECTORFUNS_HPP

#include <string>
#include <vector>
#include <map>
#include <queue>

#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/UtilityThread.hpp"

#include <time.h>
#include <algorithm>
#include "indri/QueryEnvironment.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/delete_range.hpp"
#include "indri/NetworkStream.hpp"
#include "indri/NetworkMessageStream.hpp"
#include "indri/NetworkServerProxy.hpp"

#include "indri/ListIteratorNode.hpp"
#include "indri/ListIteratorNode.hpp"
#include "indri/ExtentInsideNode.hpp"
#include "indri/DocListIteratorNode.hpp"
#include "indri/FieldIteratorNode.hpp"

#include "indri/Parameters.hpp"

#include "indri/ParsedDocument.hpp"
#include "indri/Collection.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/TaggedDocumentIterator.hpp"
#include "indri/XMLNode.hpp"


#include "indri/IndriTimer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/delete_range.hpp"
#include "indri/SnippetBuilder.hpp"

#include <stdlib.h>

//#include "Umls.hpp"
#include "Utility.hpp"
#include "modRMExpander.hpp"
#include "ExtResourceProcessor.hpp"
#include "QueryReformulator.hpp"
#include <tuple>
#include "Types.hpp"

#include <queue>


using namespace std;

// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start

namespace indri
{
namespace query
{

class ConceptSelectorFuns
{
protected:

private:
    indri::api::Parameters& param;

protected:


public:
    ConceptSelectorFuns(indri::api::Parameters& param_,
    				OneResourceConceptsParams& oneResourceConceptsParams_
    				);

    ~ConceptSelectorFuns();


    map<string, double> normConceptScoreTpRnkFreqMean(map<string, double > goodConceptsTpRnkFreqMean,
                                                                            map<string, int> conceptsFrq );

    multimap<double, pair<string, string> > normConceptScorePrf(
            vector<pair<string, string> > concatenatedGoodConcepts,
            string qId,
            vector<string> topDocsNames,
            indri::api::QueryEnvironment & env,
            indri::query::QueryReformulator * queryReformulator,
            vector<string> resourceNames_);

    map<string, double> normConceptScoreFreq(indri::api::QueryEnvironment & env,
            map<string, int> conceptsFrq,
            multimap<double, pair<string, string> > scoredConcepts_norm);

    double findConceptScorePrf(string conceptSty, string conceptStr, string qId, std::vector<lemur::api::DOCID_T> topDocIds,
            indri::api::QueryEnvironment & env,
            indri::query::QueryReformulator * queryReformulator,
            vector<string> resourceNames_);

    OneResourceConceptsParams& oneResourceConceptsParams;
};

}
}

#endif
