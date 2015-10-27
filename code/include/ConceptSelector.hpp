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
// ConceptSelector
//
// 1 Jul 2015 -- sbk
//

#ifndef CONCEPTSELECTOR_HPP
#define CONCEPTSELECTOR_HPP

#include <string>
#include <vector>
#include <map>
#include <queue>

#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/UtilityThread.hpp"
//#include "../include/lce.hpp"


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
#include "ConceptSelectorFuns.hpp"
//#include "QueryReformulator.hpp"
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
class QueryReformulator;

class ConceptSelector
{
protected:
    string conceptDocsDirectory;
    signed int fbDocs;
    signed int fbConcs;
    OneResourceConceptsParams& oneResourceConceptsParams;
    indri::api::Parameters& param;
    map<string, set<string> > wikiMedicalTermsExpr;
    map<string, string> STYs;
    string listFileName;
    indri::query::ExtResourceProcessor* extResourceProcessor;
    static map<string, string> strProcMap;
    static map<string, string> strProcMap_r;
    static map<string, string> strCuiMap;
    static map<string, string> strCuiMap_r;
    static map<string, int> conceptsFrq;

private:
    map<pair<string, string>, int > findOneDocConceptCands(
        string documentName, indri::api::QueryEnvironment & env);
    map<pair<string, string>, int> findOneBaseConcConceptCands(
        pair<string, string> oneBaseRescConc, indri::api::QueryEnvironment & env);

protected:
    // to be implemented in derived classes for different PRF-based concept selector methods:
    virtual map<pair<string, string>, int > findOneDocConceptCands_(
        string documentName, indri::api::QueryEnvironment & env) = 0;
    // to be implemented in derived classes for different base-concept-based concept selector methods:
    virtual map<pair<string, string>, int> findOneBaseConcConceptCands_(
        pair<string, string> oneBaseRescConc, indri::api::QueryEnvironment & env) = 0;

    ConceptSelectorFuns * conceptSelectorFuns;
public:
    ConceptSelector( map<string, set<string> > wikiMedicalTermsExpr,  map<string, string> STYs, indri::api::Parameters& param_, OneResourceConceptsParams& oneResourceConceptsParams_);

    ~ConceptSelector();


    vector< pair< string, map<pair<string, string>, int> > > findOneQuConceptCands(vector<string> topDocsNames, vector<pair<string, string> > oneQuBaseRescConcs, indri::api::QueryEnvironment & env );
    virtual vector< pair< string, vector<pair<string, string > > > > findConceptCands(
        vector< pair<std::string, std::string> > queriesText,
        indri::query::QueryReformulator * queryReformulator,
        vector<string> resourceNames_,
        vector<pair<string, vector<string> > > topDocsNames,
        vector<pair<string, vector<pair<string, string> > > > baseResrcConcs
        );


    bool isDocIndexed(string documentName);
    set<string> readDocsListIndexed();
    void writeTopDocConcepts(map<pair<string, string>, int > goodConcepts, string documentName);
    map<pair<string, string>, int > readOneDocConcepts(string documentName);


    vector<pair<string, string> > oneQuGatherCombFeaturesSelec(vector<string> topDocsNames, vector<pair<string, string> > oneQuBaseRescConcs, vector<string> resourceNames_, string qId, indri::api::QueryEnvironment & env,
                                                            indri::query::QueryReformulator * queryReformulator);
    vector<pair<string, string> > combinFeaturesSelec(vector<pair<string, string> > concatenatedGoodConcepts, map<string, double > goodConceptsFreqMedian,
                                                        string qId, vector<string> topDocsNames, indri::api::QueryEnvironment & env,
                                                        indri::query::QueryReformulator * queryReformulator, vector<string> resourceNames_);//, vector<pair<string, string> > oneQuBaseRescConcs );


    pair<vector<pair<string, string> >, map<string, double > > preProcessConceptsStats(vector< pair< string, map<pair<string, string>, int> > > conceptCands,
            indri::api::QueryEnvironment & env);

    string processTerm(string STR);
};

}
}

#endif
