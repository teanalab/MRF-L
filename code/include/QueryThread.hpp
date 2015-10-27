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
// QueryThread
//
// 1 Jul 2015 -- sbk
//

#ifndef QUERYTHREAD_HPP
#define QUERYTHREAD_HPP

#include "ConceptSelector.hpp"
#include "indri/QueryExpander.hpp"
#include "indri/RMExpander.hpp"
#include "indri/PonteExpander.hpp"
// need a QueryExpanderFactory....
#include "indri/TFIDFExpander.hpp"


using namespace std;



namespace indri
{
namespace thread
{


class QueryThread : public UtilityThread
{
public:
private:
    std::queue< indri::query::query_t* >& _queries;
    std::priority_queue< indri::query::query_t*, std::vector< indri::query::query_t* >,
                indri::query::query_t::greater >& _output;
    indri::thread::Lockable& _queueLock;
    indri::thread::ConditionVariable& _queueEvent;
    indri::api::Parameters& _parameters;
    indri::query::QueryExpander* _expander;
    indri::api::QueryAnnotation* _annotation;

    indri::api::QueryEnvironment _environment;
    int _requested;
    int _initialRequested;

    bool _printDocuments;
    bool _printPassages;
    bool _printSnippets;
    bool _printQuery;


    std::string _runID;
    bool _trecFormat;
    bool _inexFormat;

    std::vector<indri::api::ScoredExtentResult> _results;

//    indri::query::lce * Lce;

//    map<string, vector< pair<string, int> > > & _MRREL_CUI1_CUI2;
//    map<string, string> STYs;

//    indri::query::QueryReformulator * queryReformulator;

//    std::queue< indri::query::query_t* > _queries;

private:

    // Runs the query, expanding it if necessary.  Will print output as well if verbose is on.
    void _runQuery( std::stringstream& output, const std::string& query,
                    const std::string &queryType, const std::vector<std::string> &workingSet, std::vector<std::string> relFBDocs );

    void _printResultRegion( std::stringstream& output, std::string queryIndex, int start, int end);

    void _printResults( std::stringstream& output, std::string queryNumber);


public:
    QueryThread( std::queue< indri::query::query_t* >& queries,
                 std::priority_queue< indri::query::query_t*, std::vector< indri::query::query_t* >, indri::query::query_t::greater >& output,
                 indri::thread::Lockable& queueLock,
                 indri::thread::ConditionVariable& queueEvent,
                 indri::api::Parameters& params
//                 ,
//                 map<string, vector< pair<string, int> > > & MRREL_CUI1_CUI2, map<string, string> STYs_,
//                 indri::query::QueryReformulator * queryReformulator
                 );

    ~QueryThread();

    UINT64 initialize();

    void deinitialize();

    bool hasWork();

    UINT64 work();   // the function that will run the query


};

}
}

#endif // QUERYTHREAD_HPP
