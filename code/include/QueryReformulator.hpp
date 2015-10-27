//
// QueryReformulator
//
// 4 Aug 2015 -- sbk
//

#ifndef QUERYREFORMULATOR_HPP
#define QUERYREFORMULATOR_HPP

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<queue>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#include <boost/algorithm/string.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/assign/list_of.hpp>

#include "indri/Parameters.hpp"

#include "ExtResourceProcessor.hpp"
#include "Types.hpp"
#include "Utility.hpp"

using namespace std;

namespace indri
{
namespace query
{
class BigramsPrfTermMatchConceptSelector;
class UmlsPrfMetaMapConceptSelector;
class UmlsOrigMetaMapConceptSelector;
class UnigramsOrigConceptSelector;
class ConceptSelector;

class QueryReformulator
{
public:
    QueryReformulator(indri::api::Parameters& param, vector<indri::query::OneResourceConceptsParams*> allResourceConceptsParams_ );
    virtual ~QueryReformulator();
    void setQueries(std::queue< indri::query::query_t* > queries);

    void applyNewParam2OneResourceConceptsParams(tuple<string, string, string, double, double, double, double> t);
    string genQueryTextOneQuOneCT(vector<pair<string, string> > oneLayerConcepts, string resourceName_, double lambdaT , double lambdaPh, double lambdaPr );
//    string genQueryTextOneQuOneCT(vector<string> oneLayerConcepts, double lambdaT , double lambdaPh, double lambdaPr );
//    string genOneQueryText( vector<pair<string, vector<pair<string, string> > > > conceptCandidatesOneQ );
    string genOneQueryText( vector<tuple<vector<pair<string, string> >, tuple<double, double, double, double>, string > > oneQuResourcesConceptsParams );
//    vector<pair<string, string> > genQueryText(vector<pair<string, vector<pair<string, vector<pair<string, string> > > > > > conceptCandidates );
    vector<pair<string, string> > genQueryTextResources(vector<string> resourceNames, vector<string> qIdWorkset= boost::assign::list_of("ALL"),
		pair<string, string> conceptExt = pair<string, string>("", ""), tuple<double, double, double, double> lambdasExt=make_tuple(0,0,0,0));
//    std::queue< indri::query::query_t* > getQueries(vector<string> resourceNames, vector<string> docIdWorkset=boost::assign::list_of("ALL"), vector<string> qIdWorkset=boost::assign::list_of("ALL"));
//    void setCategoriesConcepts(vector<vector<pair<string, vector<pair<string, string> > > > > categoriesConcepts_);
    //void addResourcesConcepts(vector<indri::query::OneResourceConceptsParams> & allResourceConceptsParams);
    //void reformulateQuery(vector<string> resourceNames);
    vector<pair<string, string> > setOrigQuery(std::queue< indri::query::query_t* > _queries);
//    void setUmlsOrigConcepts();
//    vector<pair<string, string> > reformulateQuery(vector<string> resourceNames, map<string, string> STYs,
//                            map<string, set<string> > wikiMedicalTermsExpr, map<string, vector< pair<string, int> > > MRREL_CUI1_CUI2 = map<string, vector< pair<string, int> > >(),
//                            map<string, string> MRCONSO_simp, map<string, set<string> > MRSTY_simp,
//                            vector<string> qIdWorkset = vector<string>{"ALL"});
    vector<pair<string, string> > reformulateQuery(vector<string> resourceNames, map<string, string> STYs,
        map<string, set<string> > wikiMedicalTermsExpr, map<string, vector< pair<string, int> > > MRREL_CUI1_CUI2, map<string, string> MRCONSO_simp,
        map<string, set<string> > MRSTY_simp, map<string, string> styAbbrsTuis, vector<string> qIdWorkset = vector<string>{"ALL"});
	vector<pair<string, string> > testOneConceptAddition2OneQuery(string conceptSty, string concpentStr, string qId, vector<string> resourceNames);
protected:
private:
	bool checkSelectionNecessity(vector<string> resourceNames, string resourceName);
	vector<indri::query::OneResourceConceptsParams*> allResourceConceptsParams;
    //OneResourceConceptsParams * umlsOrigMetaMap_ConceptsParams;
    vector<string> goodSTYs_v;
    indri::api::Parameters& _param;
    indri::query::ExtResourceProcessor * extResourceProcessor;
    std::queue< indri::query::query_t* > _queries_;
    vector<pair<string, string> > origQueriesTexts;
//    vector<pair<string, vector<tuple<string, string, string> > > > umlsOrigConcepts;
//    vector<vector<pair<string, vector<pair<string, string> > > > > allResourcesConcepts;
    //vector<indri::query::OneResourceConceptsParams> allResourceConceptsParams;
};

}
}

#endif // QUERYREFORMULATOR_HPP
