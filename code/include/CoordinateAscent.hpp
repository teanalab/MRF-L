//
// CoordinateAscent
//
// 30 Jun 2015 -- sbk
//

#ifndef COORDINATEASCENT_HPP
#define COORDINATEASCENT_HPP

#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include "QueryThread.hpp"
#include "QueryReformulator.hpp"
#include "ExtResourceProcessor.hpp"
#include "ConceptSelector.hpp"
#include "Utility.hpp"


using namespace std;

namespace indri
{
namespace query
{

class CoordinateAscent
{
public:
    CoordinateAscent(indri::api::Parameters& param);
    ~CoordinateAscent();

    void run();
    void runLce();
    void runMedPqe();
    void QueryThread_(vector<string> resourceNames, vector<pair<string, string> > queriesTexts = vector<pair<string, string> >());


private:
	vector<string> resourceNames;
	vector<string> primaryResourceNames;
	vector<pair<string, vector<string> > > optResourceNamesParams;
    bool derivativeWatchDog(vector<double> precisionsRecord, int direction);
    void optimization(vector< tuple<string, string, string, double, double, double, double> >& vt, double& trec_eval_avg_);
    multimap<double, string, std::greater<double> > optimization( const set<string>& STYs);
    int findDirection(tuple<string, string, string, double, double, double, double> & t, map<double, double> & precisionsHist, vector<string> resourceNames);
    double optimization_(tuple<string, string, string, double, double, double, double> & t, int direction, map<double, double> & precisionsHist,
                            vector<string> resourceNames);
    void printHistoryLce();
    void printHistoryMedPqe();
    void assignValue2Param( string intOrDouble, string paramOrRuleParams,
                            string paramName, double paramValue,
                            tuple<string, string, string, double, double, double, double> t);
    void apply_t_2_param(tuple<string, string, string, double, double, double, double> t);

    double runAndCalcPrecision(vector<string> resourceNames);

    template <typename T>
    tuple<string, string, string, T, T, T, T> makeTuple(string resourceName, string dType, string paramName);

    void parseSetResourceNames();

private:
	vector<indri::query::OneResourceConceptsParams*> allResourceConceptsParams;

//    std::vector< indri::thread::QueryThread* > threads;
//    std::priority_queue< indri::query::query_t*, std::vector< indri::query::query_t* >, indri::query::query_t::greater > _output;
//
//    std::queue< indri::query::query_t* > _queries;
    std::vector< pair<std::string, std::string> > queriesText;

    map<string, string> STYs;
    map<string, string> styAbbrsTuis;
    map<string, set<string> > wikiMedicalTermsExpr;
    map<string, set<string> > MRSTY_simp;
    map<string, vector< pair<string, int> > > MRREL_CUI1_CUI2;
    map<string, string> MRCONSO_simp;

    indri::api::Parameters& _param;
    indri::api::Parameters& _ruleParam;
    indri::api::Parameters& _stepSizeParams ;

    indri::query::QueryReformulator * queryReformulator;

    vector<tuple<string, double, double> > optimizationHistory;

    indri::query::ExtResourceProcessor * extResourceProcessor;

};
}
}

#endif
