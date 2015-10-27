//
// ConceptSelectorFuns
//
// 1 Jul 2015 -- sbk
//

#include <math.h>
#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <limits>

#include "../include/ConceptSelectorFuns.hpp"
#include "../include/QueryReformulator.hpp"


#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/QueryEnvironment.hpp"

indri::query::ConceptSelectorFuns::ConceptSelectorFuns(indri::api::Parameters& param_, OneResourceConceptsParams& oneResourceConceptsParams_)
		:param(param_), oneResourceConceptsParams(oneResourceConceptsParams_)
{
}
indri::query::ConceptSelectorFuns::~ConceptSelectorFuns()
{
}


map<string, double> indri::query::ConceptSelectorFuns::normConceptScoreTpRnkFreqMean(map<string, double > goodConceptsTpRnkFreqMean,
                                                                            map<string, int> conceptsFrq )
{
    map<string, double> goodConceptsTpRnkFreqMean_norm;
    double max_tpRnkcf = 0;
    double min_tpRnkcf = std::numeric_limits<double>::infinity();
    for (auto itCfm = goodConceptsTpRnkFreqMean.begin(); itCfm != goodConceptsTpRnkFreqMean.end(); itCfm++)
    {
//        for(auto iii: conceptsFrq)
//        {
//            cout << "indri::query::ConceptSelectorFuns::normConceptScoreTpRnkFreqMean: goodConceptsTpRnkFreqMean: " << iii.first << " " << iii.second << " " << itCfm->first << " " << (itCfm->first==iii.first) << endl;
//        }
        if(conceptsFrq.find(itCfm->first)!=conceptsFrq.end())
        {
            if(conceptsFrq.at(itCfm->first)>0)
            {
                goodConceptsTpRnkFreqMean_norm[itCfm->first] = itCfm->second/double(conceptsFrq.at(itCfm->first));
                max_tpRnkcf = max(max_tpRnkcf, goodConceptsTpRnkFreqMean_norm[itCfm->first]);
                min_tpRnkcf = min(min_tpRnkcf, goodConceptsTpRnkFreqMean_norm[itCfm->first]);
                cout << "indri::query::ConceptSelectorFuns::normConceptScoreTpRnkFreqMean: goodConceptsTpRnkFreqMean_norm: " << itCfm->first
                        << " " << goodConceptsTpRnkFreqMean.at(itCfm->first) << endl
                        << "\t\t" << goodConceptsTpRnkFreqMean_norm.at(itCfm->first)
                        << " = " << itCfm->second
                        << " / " << conceptsFrq.at(itCfm->first) << endl
                        << "\t\tmin_tpRnkcf = " << min_tpRnkcf
                        << " max_tpRnkcf = " << max_tpRnkcf
                        << endl;
            }
            else
                goodConceptsTpRnkFreqMean_norm[itCfm->first] = 0;
        }
        else
            goodConceptsTpRnkFreqMean_norm[itCfm->first] = 0;

    }
    map<string, double> goodConceptsTpRnkFreqMean_norm1;
    for (auto itCfm = goodConceptsTpRnkFreqMean_norm.begin(); itCfm != goodConceptsTpRnkFreqMean_norm.end(); itCfm++)
    {
        goodConceptsTpRnkFreqMean_norm1[itCfm->first] = (itCfm->second-min_tpRnkcf)/(max_tpRnkcf-min_tpRnkcf);
        cout << "indri::query::ConceptSelectorFuns::normConceptScoreTpRnkFreqMean: goodConceptsTpRnkFreqMean_norm1: " << goodConceptsTpRnkFreqMean_norm1[itCfm->first]
                << " str = "<< itCfm->first << " non-normalized = "<< itCfm->second
                << " min_tpRnkcf = " << min_tpRnkcf << " max_tpRnkcf = " << max_tpRnkcf << endl;
    }
    return goodConceptsTpRnkFreqMean_norm1;
}

multimap<double, pair<string, string> > indri::query::ConceptSelectorFuns::normConceptScorePrf(
                                                            vector<pair<string, string> > concatenatedGoodConcepts,
                                                            string qId,
                                                            vector<string> topDocsNames,
                                                            indri::api::QueryEnvironment & env,
                                                            indri::query::QueryReformulator * queryReformulator,
                                                            vector<string> resourceNames_)
{
    std::vector<lemur::api::DOCID_T> topDocIds = env.documentIDsFromMetadata("docno", topDocsNames);
    multimap<double, pair<string, string>, std::greater<double> > scoredConcepts_;
    for(auto concStyStrPair: concatenatedGoodConcepts) // for each each extracted concept
    {
        string conceptSty = concStyStrPair.first;
        string conceptStr = concStyStrPair.second;
        double conceptScore = indri::query::ConceptSelectorFuns::findConceptScorePrf(conceptSty,
                                                                                        conceptStr,
                                                                                        qId,
                                                                                        topDocIds,
                                                                                        env,
                                                                                        queryReformulator,
                                                                                        resourceNames_);

        scoredConcepts_.insert(make_pair(conceptScore, make_pair(conceptSty, conceptStr)));
        cout << "indri::query::ConceptSelectorFuns::normConceptScorePrf: conceptScore = " << conceptStr << " -> " << conceptScore << endl;
    }

    double max_sc = 0;
    double min_sc = std::numeric_limits<double>::infinity();
    for (auto sc: scoredConcepts_)
    {
        max_sc = max(max_sc, sc.first);
        min_sc = min(min_sc, sc.first);
    }
    cout << "indri::query::ConceptSelectorFuns::normConceptScorePrf: min_sc, max_sc: " << min_sc << ", " << max_sc << endl;

    // min-max normalize socores in scoredConcepts_
    multimap<double, pair<string, string> > scoredConcepts_norm;
    for (auto itSc = scoredConcepts_.begin(); itSc != scoredConcepts_.end(); itSc++)
    {
        double conceptScore = (itSc->first- min_sc)/(max_sc- min_sc);
        scoredConcepts_norm.insert(make_pair(conceptScore, make_pair((itSc->second).first, (itSc->second).second)));
        cout << "indri::query::ConceptSelectorFuns::normConceptScorePrf: scoredConcepts_norm: scoredConcepts_ = " << itSc->first  << endl;
        cout << "indri::query::ConceptSelectorFuns::normConceptScorePrf: scoredConcepts_norm: conceptScore = " << conceptScore << " =  (" << itSc->first << " - " << min_sc << " )/( " << max_sc << " - " << min_sc << " )" << endl;
    }

    return scoredConcepts_norm;
}


map<string, double> indri::query::ConceptSelectorFuns::normConceptScoreFreq(indri::api::QueryEnvironment & env,
                                                            map<string, int> conceptsFrq,
                                                            multimap<double, pair<string, string> > scoredConcepts_norm )
{

    // extract concept freq for the concepts exist in scoredConcepts_norm and store it in a container
    map<string, double> conceptsFrqExtr_norm;
    double max_cf = 0;
    double min_cf = std::numeric_limits<double>::infinity();;
    for (auto itSc = scoredConcepts_norm.begin(); itSc != scoredConcepts_norm.end(); itSc++)
    {
        conceptsFrqExtr_norm[(itSc->second).second] = conceptsFrq[(itSc->second).second] / double(env.documentCount());
        max_cf = max(max_cf, conceptsFrqExtr_norm[(itSc->second).second]);
        min_cf = min(min_cf, conceptsFrqExtr_norm[(itSc->second).second]);

        cout << "indri::query::ConceptSelectorFuns::normConceptScoreFreq: conceptsFrqExtr_norm: min_cf, max_cf: " << min_cf << ", " << max_cf << endl;
        cout << "indri::query::ConceptSelectorFuns::normConceptScoreFreq: scoredConcepts_norm: " << (itSc->second).second << " " << itSc->first << endl;
        cout << "indri::query::ConceptSelectorFuns::normConceptScoreFreq: conceptsFrq: " << (itSc->second).second << " " << conceptsFrq[(itSc->second).second] << endl;
    }

    // normalize the extracted concepts' scores
    map<string, double> conceptsFrqExtr_norm1;
    for (auto itCf = conceptsFrqExtr_norm.begin(); itCf != conceptsFrqExtr_norm.end(); itCf++)
    {
        conceptsFrqExtr_norm1[itCf->first] = (itCf->second-min_cf)/(max_cf-min_cf);
        cout << "indri::query::ConceptSelectorFuns::normConceptScoreFreq: conceptsFrqExtr_norm1: " << itCf->first << " " << conceptsFrqExtr_norm1[itCf->first] << endl;
    }
    return conceptsFrqExtr_norm1;
}

double indri::query::ConceptSelectorFuns::findConceptScorePrf(string conceptSty, string conceptStr, string qId, std::vector<lemur::api::DOCID_T> topDocIds,
        indri::api::QueryEnvironment & env,
        indri::query::QueryReformulator * queryReformulator,
        vector<string> resourceNames_)
{
    // runQuery the new query text on these workset of top-ranked documents
    vector<pair<string, vector<pair<string, string> > > > candConcepts_;
    vector<pair<string, string> > tmp = {make_pair(conceptSty, conceptStr)};
    candConcepts_ = {make_pair(qId, tmp )};
    oneResourceConceptsParams.oneResourceConcepts = candConcepts_;

    vector<pair<string, string> > queriesText = queryReformulator->testOneConceptAddition2OneQuery(conceptSty, conceptStr, qId, resourceNames_);

    std::vector< indri::api::ScoredExtentResult > results_;
    if(wsuIr::expander::Utility::runQuery_results_isExist(queriesText, topDocIds))
    {
        results_ = wsuIr::expander::Utility::runQuery_results_get(queriesText, topDocIds);
    }
    else
    {
        results_ = env.runQuery(queriesText.front().second, topDocIds, topDocIds.size());
        wsuIr::expander::Utility::runQuery_results_store(queriesText, topDocIds, results_);
    }

    if(results_.size() != topDocIds.size())
        throw runtime_error("RunQUery.cpp: some of top-ranked documents are not scored");

    double conceptScore = 0;
    for(auto r: results_)
    {
        conceptScore += r.score;
    }

    return conceptScore;
}
