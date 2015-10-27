#ifndef UTILITY_HPP
#define UTILITY_HPP

#include<map>
#include<set>
#include<tuple>
#include<string>
#include<vector>

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

#include "Types.hpp"

using namespace std;

namespace wsuIr
{
namespace expander
{
class Utility
{
public:
    Utility();
    virtual ~Utility();
    static void QueryThread_results_store(vector<pair<string, std::string> > queriesText);
    static void QueryThread_results_update(vector<pair<string, std::string> > queriesText);
    static bool QueryThread_results_isExist(vector<pair<string, std::string> > queriesText);

    static bool runQuery_results_isExist(vector<pair<string, std::string> > queriesText, vector<lemur::api::DOCID_T> topDocIds);
    static std::vector< indri::api::ScoredExtentResult > runQuery_results_get(vector<pair<string, std::string> > queriesText, vector<lemur::api::DOCID_T> topDocIds);
    static void runQuery_results_store(vector<pair<string, std::string> > queriesText, vector<lemur::api::DOCID_T> topDocIds, std::vector< indri::api::ScoredExtentResult > runQuery_result);

    static map<string, lemur::api::DOCID_T> findDocumentNames(indri::api::Parameters& param, string index_s);
    static string getTextOfADocument(string documentName, indri::api::Parameters& param, indri::api::QueryEnvironment & env);
    static std::string GetTag(const std::string &str, const std::string &tagBegin, const std::string &tagEnd );

    template<class Set1, class Set2>
    static bool is_disjoint(const Set1 &set1, const Set2 &set2);

public:
    static vector<string> readGoodSTYs(indri::api::Parameters& param);

    static double trec_eval_avg();
    static bool copy_parameters_to_string_vector( std::vector<std::string>& vec, indri::api::Parameters p, const std::string& parameterName );

    static void parameters_parse( indri::api::Parameters& converted, const std::string& spec );

    static void push_queue( std::queue< indri::query::query_t* >& q, indri::api::Parameters& queries,
            int queryOffset );
    static void push_queue_crv( std::queue< indri::query::query_t* >& q, indri::api::Parameters& queries,
            int queryOffset, indri::api::Parameters& param );

    static string read_trec_rel(indri::api::Parameters& param_);

    static void update_trec_eval(string queryNumber, double trec_res);
    static void print_trec_eval();
    static map<string, double> get_trec_eval();
    static map<string, double> trec_res_precision;
    static string getRunQuery_(string qNumber_, map<int, string> & qidOutputStr_);
    static vector< pair<string, double> > parseRunQueryStr(string runQuery_, int fbDocsL2);

    static void waitForTrecFree();
    static void startWaiting();
    static void stopWaiting();

    static void concatenateOutputStr(string qNumber, string outputStr_);
    static void calcTrecPrecisionMap(vector<string> _trec_eval_argv_v);
    static void calcTrecPrecisionInfNdcg(indri::api::Parameters& param);
    static vector<pair<string, vector<string> > > getTopDocsNames(signed int fbDocs);

    static void initialize();

    static std::string exec(const char* cmd);

protected:
private:
//    static map<string, lemur::api::DOCID_T> documentNamesMap;
    static bool trecWaitEnable;
    static map<int, string> qidOutputStr;
    static map<vector<pair<string, std::string> >, map<int, string>> QueryThread_storeResults;
    static map<pair<vector<pair<string, std::string> >, vector<lemur::api::DOCID_T> >, vector<indri::api::ScoredExtentResult> > runQuery_results;
    static string queryIndex2queryNumber(indri::api::Parameters& queries, int queryOffset, size_t index_);

// ---> cross validation:
public:
    static pair<vector<string>, vector<string> > selectCrossValdQuNums( indri::api::Parameters& param, size_t foldCount, size_t foldNum );
private:
    static void shuffle(vector<string> & v);
    static pair< vector<string>, vector<string> > trainTestVecs(vector<string> allQuNums, size_t foldCount, size_t foldNum, bool trainTestRevrs);
// <--- cross validation

};

// http://stackoverflow.com/questions/1964150/c-test-if-2-sets-are-disjoint :
template<class Set1, class Set2>
bool wsuIr::expander::Utility::is_disjoint(const Set1 &set1, const Set2 &set2)
{
    if(set1.empty() || set2.empty()) return true;

    typename Set1::const_iterator
    it1 = set1.begin(),
    it1End = set1.end();
    typename Set2::const_iterator
    it2 = set2.begin(),
    it2End = set2.end();

    if(*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) return true;

    while(it1 != it1End && it2 != it2End)
    {
        if(*it1 == *it2) return false;
        if(*it1 < *it2)
        {
            it1++;
        }
        else
        {
            it2++;
        }
    }

    return true;
}
}
}
#endif // UTILITY_HPP
