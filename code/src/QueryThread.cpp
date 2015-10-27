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

#include <math.h>
#include <stdio.h>
#include <sstream>

#include "../include/QueryThread.hpp"


// Runs the query, expanding it if necessary.  Will print output as well if verbose is on.
//void indri::thread::QueryThread::_runQuery( std::stringstream& output, map<string, double> coeffs, const std::string& query,
//                const std::string &queryType, const std::vector<std::string> &workingSet, std::vector<std::string> relFBDocs )
//{
void indri::thread::QueryThread::_runQuery( std::stringstream& output, const std::string& query,
        const std::string &queryType, const std::vector<std::string> &workingSet, std::vector<std::string> relFBDocs )
{
    try
    {
        if( _printQuery ) output << "# query: " << query << std::endl;
//    cout << "indri::thread::QueryThread::_runQuery: query->text = " << query << endl;
        std::vector<lemur::api::DOCID_T> docids;
        if (workingSet.size() > 0)
            docids = _environment.documentIDsFromMetadata("docno", workingSet);


        if (relFBDocs.size() == 0)
        {
            if( _printSnippets )
            {
                if (workingSet.size() > 0)
                    _annotation = _environment.runAnnotatedQuery( query, docids, _initialRequested, queryType );
                else
                    _annotation = _environment.runAnnotatedQuery( query, _initialRequested );
                _results = _annotation->getResults();
            }
            else
            {
//                indri::query::ConceptSelector::initialize();
                if (workingSet.size() > 0)
                    _results = _environment.runQuery( query, docids, _initialRequested, queryType );
                else
                    _results = _environment.runQuery( query, _initialRequested, queryType );
            }
        }

        if( _expander )
        {
            std::vector<indri::api::ScoredExtentResult> fbDocs;
            if (relFBDocs.size() > 0)
            {
                docids = _environment.documentIDsFromMetadata("docno", relFBDocs);
                for (size_t i = 0; i < docids.size(); i++)
                {
                    indri::api::ScoredExtentResult r(0.0, docids[i]);
                    fbDocs.push_back(r);
                }
            }
            std::string expandedQuery;
            if (relFBDocs.size() != 0)
                expandedQuery = _expander->expand( query, fbDocs );
            else
                expandedQuery = _expander->expand( query, _results );
            if( _printQuery ) output << "# expanded: " << expandedQuery << std::endl;
//            indri::query::ConceptSelector::initialize();
//    	cout << "-7-------------------------------------expandedQuery = " << expandedQuery << endl;
            if (workingSet.size() > 0)
            {
                docids = _environment.documentIDsFromMetadata("docno", workingSet);
                _results = _environment.runQuery( expandedQuery, docids, _requested, queryType );
            }
            else
            {
                _results = _environment.runQuery( expandedQuery, _requested, queryType );
            }
        }
//        cout << "indri::thread::QueryThread::_runQuery: size of _results:" << _results.size() << endl;
    }
    catch( lemur::api::Exception& e )
    {
    	cout << "indri::thread::QueryThread::_runQuery: lemur::api::Exception" << endl;
        _results.clear();
        LEMUR_RETHROW(e, "QueryThread::_runQuery Exception");
    }
}

void indri::thread::QueryThread::_printResultRegion( std::stringstream& output, std::string queryIndex, int start, int end  )
{
    std::vector<std::string> documentNames;
    std::vector<indri::api::ParsedDocument*> documents;

    std::vector<indri::api::ScoredExtentResult> resultSubset;

    resultSubset.assign( _results.begin() + start, _results.begin() + end );


    // Fetch document data for printing
    if( _printDocuments || _printPassages || _printSnippets )
    {
        // Need document text, so we'll fetch the whole document
        documents = _environment.documents( resultSubset );
        documentNames.clear();

        for( size_t i=0; i<resultSubset.size(); i++ )
        {
//            indri::api::ParsedDocument* doc = documents[i];
            std::string documentName;

            indri::utility::greedy_vector<indri::parse::MetadataPair>::iterator iter = std::find_if( documents[i]->metadata.begin(),
                    documents[i]->metadata.end(),
                    indri::parse::MetadataPair::key_equal( "docno" ) );

            if( iter != documents[i]->metadata.end() )
                documentName = (char*) iter->value;

            // store the document name in a separate vector so later code can find it
            documentNames.push_back( documentName );
        }
    }
    else
    {
        // We only want document names, so the documentMetadata call may be faster
        documentNames = _environment.documentMetadata( resultSubset, "docno" );
    }

    std::vector<std::string> pathNames;
    if ( _inexFormat )
    {
        // retrieve path names
        pathNames = _environment.pathNames( resultSubset );
    }

    // Print results
    for( size_t i=0; i < resultSubset.size(); i++ )
    {
        int rank = start+i+1;
        std::string queryNumber = queryIndex;

        if( _trecFormat )
        {
            // TREC formatted output: queryNumber, Q0, documentName, rank, score, runID
            output << queryNumber << " "
                   << "Q0 "
                   << documentNames[i] << " "
                   << rank << " "
                   << resultSubset[ i ].score << " "
                   << _runID << std::endl;
        }
        else if( _inexFormat )
        {

            output << "    <result>" << std::endl
                   << "      <file>" << documentNames[i] << "</file>" << std::endl
                   << "      <path>" << pathNames[i] << "</path>" << std::endl
                   << "      <rsv>" << resultSubset[i].score << "</rsv>"  << std::endl
                   << "    </result>" << std::endl;
        }
        else
        {
            // score, documentName, firstWord, lastWord
            output << resultSubset[i].score << "\t"
                   << documentNames[i] << "\t"
                   << resultSubset[i].begin << "\t"
                   << resultSubset[i].end << std::endl;
        }

        if( _printDocuments )
        {
            output << documents[i]->text << std::endl;
        }

        if( _printPassages )
        {
            int byteBegin = documents[i]->positions[ resultSubset[i].begin ].begin;
            int byteEnd = documents[i]->positions[ resultSubset[i].end-1 ].end;
            output.write( documents[i]->text + byteBegin, byteEnd - byteBegin );
            output << std::endl;
        }

        if( _printSnippets )
        {
            indri::api::SnippetBuilder builder(false);
            output << builder.build( resultSubset[i].document, documents[i], _annotation ) << std::endl;
        }

        if( documents.size() )
            delete documents[i];
    }
}


void indri::thread::QueryThread::_printResults( std::stringstream& output, std::string queryNumber )
{
    if (_inexFormat)
    {
        // output topic header
        output << "  <topic topic-id=\"" << queryNumber << "\">" << std::endl
               << "    <collections>" << std::endl
               << "      <collection>ieee</collection>" << std::endl
               << "    </collections>" << std::endl;
    }
    for( size_t start = 0; start < _results.size(); start += 50 )
    {
        size_t end = std::min<size_t>( start + 50, _results.size() );
        _printResultRegion( output, queryNumber, start, end );
    }
    if( _inexFormat )
    {
        output << "  </topic>" << std::endl;
    }
    delete _annotation;
    _annotation = 0;
}

indri::thread::QueryThread::QueryThread( std::queue< indri::query::query_t* >& queries,
                 std::priority_queue< indri::query::query_t*, std::vector< indri::query::query_t* >, indri::query::query_t::greater >& output,
                 indri::thread::Lockable& queueLock,
                 indri::thread::ConditionVariable& queueEvent,
                 indri::api::Parameters& params
//                 ,
//                 map<string, vector< pair<string, int> > > & MRREL_CUI1_CUI2, map<string, string> STYs_,
//                 indri::query::QueryReformulator * queryReformulator
                 )
    :
    _queries(queries),
    _output(output),
    _queueLock(queueLock),
    _queueEvent(queueEvent),
    _parameters(params),
    _expander(0),
    _annotation(0)
//    _MRREL_CUI1_CUI2(MRREL_CUI1_CUI2),
//    STYs(STYs_)
{
//    queryReformulator = new indri::query::QueryReformulator(_parameters);
}

indri::thread::QueryThread::~QueryThread()
{
//    delete Lce;
//    delete queryReformulator;
}

UINT64 indri::thread::QueryThread::initialize()
{
    try
    {
        _environment.setSingleBackgroundModel( _parameters.get("singleBackgroundModel", false) );

        std::vector<std::string> stopwords;
        if( wsuIr::expander::Utility::copy_parameters_to_string_vector( stopwords, _parameters, "stopper.word" ) )
            _environment.setStopwords(stopwords);

        std::vector<std::string> smoothingRules;
        if( wsuIr::expander::Utility::copy_parameters_to_string_vector( smoothingRules, _parameters, "rule" ) )
            _environment.setScoringRules( smoothingRules );

        if( _parameters.exists( "index" ) )
        {
            indri::api::Parameters indexes = _parameters["index"];

            for( size_t i=0; i < indexes.size(); i++ )
            {
                _environment.addIndex( std::string(indexes[i]) );
            }
        }

        if( _parameters.exists( "server" ) )
        {
            indri::api::Parameters servers = _parameters["server"];

            for( size_t i=0; i < servers.size(); i++ )
            {
                _environment.addServer( std::string(servers[i]) );
            }
        }

        if( _parameters.exists("maxWildcardTerms") )
            _environment.setMaxWildcardTerms(_parameters.get("maxWildcardTerms", 100));

        _requested = _parameters.get( "count", 1000 );
        _initialRequested = _parameters.get( "fbDocs", _requested );
        _runID = _parameters.get( "runID", "indri" );
        _trecFormat = _parameters.get( "trecFormat" , false );
        _inexFormat = _parameters.exists( "inex" );

        _printQuery = _parameters.get( "printQuery", false );
        _printDocuments = _parameters.get( "printDocuments", false );
        _printPassages = _parameters.get( "printPassages", false );
        _printSnippets = _parameters.get( "printSnippets", false );

        if (_parameters.exists("baseline"))
        {
            // doing a baseline
            std::string baseline = _parameters["baseline"];
            _environment.setBaseline(baseline);
            // need a factory for this...
            if( _parameters.get( "fbDocs", 0 ) != 0 )
            {
                // have to push the method in...
                std::string rule = "method:" + baseline;
                _parameters.set("rule", rule);
                _expander = new indri::query::TFIDFExpander( &_environment, _parameters );
            }
        }
        else
        {
            if( _parameters.get( "fbDocs", 0 ) != 0 )
            {
//                cout << "indri::thread::QueryThread::initialize: RM-expanding query..." << endl;
                _expander = new indri::query::RMExpander( &_environment, _parameters );
//                cout << "indri::thread::QueryThread::initialize: query is RM-expanded." << endl;
//            cout << "indri::thread::QueryThread::initialize: _environment.documentCount() = " << _environment.documentCount() << endl;
            }
        }

        if (_parameters.exists("maxWildcardTerms"))
        {
            _environment.setMaxWildcardTerms((int)_parameters.get("maxWildcardTerms"));
        }
    }
    catch ( lemur::api::Exception& e )
    {
//        cout << "indri::thread::QueryThread::initialize:catch: _queries.size() = " << _queries.size() << endl;
        while( _queries.size() )
        {
            indri::query::query_t *query = _queries.front();
            _queries.pop();
            _output.push( new indri::query::query_t( query->index, query->number, "query: " + query->number + " QueryThread::_initialize exception\n" ) );
            _queueEvent.notifyAll();
            LEMUR_RETHROW(e, "QueryThread::_initialize");
        }
    }
    return 0;
}

void indri::thread::QueryThread::deinitialize()
{
    delete _expander;
    _environment.close();
}

bool indri::thread::QueryThread::hasWork()
{
//    cout << "indri::thread::QueryThread::hasWork: _queries.size()" << _queries.size() << endl;
    indri::thread::ScopedLock sl( &_queueLock );
    return _queries.size() > 0;
}

UINT64 indri::thread::QueryThread::work()
{
    indri::query::query_t* query;
    std::stringstream output;

    // pop a query off the queue
    {
        indri::thread::ScopedLock sl( &_queueLock );
        if( _queries.size() )
        {
//            cout << "indri::thread::QueryThread::work: (returns nonzero) _queries.size() = " << _queries.size() << endl;
            query = _queries.front();
            _queries.pop();
        }
        else
        {
//            cout << "indri::thread::QueryThread::work: (returns 0) _queries.size() = " << _queries.size() << endl;
            return 0;
        }
    }

    // run the query
    try
    {
//        cout << "<query>" << endl;
//        cout << "<number>" << query->number << "</number>" << endl;
//        cout << "indri::thread::QueryThread::work: queryNumber:" << query->number << endl;
//        cout << "<text>/n" << query->text << "/n</text>" << endl;
//        cout << "</query>" << endl;
        if (_parameters.exists("baseline") && ((query->text.find("#") != std::string::npos) || (query->text.find(".") != std::string::npos)) )
        {
            LEMUR_THROW( LEMUR_PARSE_ERROR, "Can't run baseline on this query: " + query->text + "\nindri query language operators are not allowed." );
        }
//        cout << "indri::thread::QueryThread::work: query->text:" << query->text << endl;
        _runQuery( output, query->text, query->qType, query->workingSet, query->relFBDocs );
    }
    catch( lemur::api::Exception& e )
    {
        output << "# EXCEPTION in query " << query->number << ": " << e.what() << std::endl;
    }

    // print the results to the output stream
    _printResults( output, query->number );

    // push that data into an output queue...?
    {
        indri::thread::ScopedLock sl( &_queueLock );
        _output.push( new indri::query::query_t( query->index, query->number, output.str() ) );
        _queueEvent.notifyAll();
    }

    string outputStr = output.str();
	wsuIr::expander::Utility::concatenateOutputStr(query->number, outputStr);
    /*
    string qId = query->number;
    int fbDocs_umlsPrf = _parameters.get("fbDocs_umlsPrf",10);
    cout << "fbDocs_umlsPrf = " << fbDocs_umlsPrf << endl;
    vector< pair<string, double> > outputDocsScores = indri::query::ConceptSelector::parseRunQueryStr(outputStr, fbDocs_umlsPrf);
    vector<string> topDocumentNames;
    for(auto ds: outputDocsScores)
        topDocumentNames.push_back(ds.first);
    vector<pair<string, string> > goodConceptsOfTopDocument_ = indri::query::ConceptSelector::concatenateGoodConceptsOfDocumentVec(_parameters,
                topDocumentNames, STYs, qId, _environment, queryReformulator);
    */
    delete query;
//    cout << "indri::thread::QueryThread::work: end of work function query->number = " << query->number << endl;
    return 0;
}



