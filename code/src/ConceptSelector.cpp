//
// ConceptSelector
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

#include "../include/ConceptSelector.hpp"
#include "../include/QueryReformulator.hpp"


#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/QueryEnvironment.hpp"

/*
#ifndef TREC_EVAL_H_
#define TREC_EVAL_H_
extern "C" {
#include "trec_eval_.h"
}
#endif // TREC_EVAL_H_
*/

map<string, string> indri::query::ConceptSelector::strProcMap = map<string, string>();
map<string, string> indri::query::ConceptSelector::strProcMap_r = map<string, string>();
map<string, string> indri::query::ConceptSelector::strCuiMap = map<string, string>();
map<string, string> indri::query::ConceptSelector::strCuiMap_r = map<string, string>();
map<string, int> indri::query::ConceptSelector::conceptsFrq = map<string, int>();

indri::query::ConceptSelector::ConceptSelector(map<string, set<string> > wikiMedicalTermsExpr_,  map<string, string> STYs_, indri::api::Parameters& param_, OneResourceConceptsParams& oneResourceConceptsParams_)
    : conceptDocsDirectory(oneResourceConceptsParams_.conceptDocsDirectory),
      fbDocs(get<0>(oneResourceConceptsParams_.fbDocs)),
      fbConcs(std::numeric_limits<int>::max()), // to keep everything and do this filtering in QueryReformulator
      oneResourceConceptsParams(oneResourceConceptsParams_),
      param(param_),
      wikiMedicalTermsExpr(wikiMedicalTermsExpr_),
      STYs(STYs_)
{
    listFileName = "docsList.txt";
    extResourceProcessor = new ExtResourceProcessor(param_);
    conceptSelectorFuns = new ConceptSelectorFuns(param_, oneResourceConceptsParams);
}
indri::query::ConceptSelector::~ConceptSelector()
{
    delete extResourceProcessor;
    delete conceptSelectorFuns;
}

bool indri::query::ConceptSelector::isDocIndexed(string documentName)
{
    set<string> docsIndexed = indri::query::ConceptSelector::readDocsListIndexed();
//    cout << "indri::query::ConceptSelector::isDocIndexed: documentName = " << documentName << endl;
//    cout << "indri::query::ConceptSelector::isDocIndexed: size of docsIndexed = " << docsIndexed.size() << endl;
    if(docsIndexed.find(documentName)!=docsIndexed.end())
        return true;
    return false;
}

set<string> indri::query::ConceptSelector::readDocsListIndexed()
{
    string listFileName_ = conceptDocsDirectory + listFileName;
    ifstream file(listFileName_.c_str());
    set<string> docsIndexed;
    string docIndexed;
    while(file>>docIndexed)
        docsIndexed.insert(docIndexed);
//    cout << "indri::query::ConceptSelector::readDocsListIndexed: size of docsIndexed: " << docsIndexed.size() << endl;
//    cout << "indri::query::ConceptSelector::readDocsListIndexed: listFileName_: " << listFileName_ << endl;
    return docsIndexed;
}

string indri::query::ConceptSelector::processTerm( string STR)
{
    string repName = param.get("indexFielded", "");
    indri::collection::Repository r;
    r.openRead( repName );
    indri::server::LocalQueryServer local(r);
    indri::collection::Repository::index_state state = r.indexes();

    vector<string> STR_;
    boost::split(STR_,STR,boost::is_any_of(" "));
    string STRTmp = "";
    for (auto t: STR_)
        STRTmp += " " + r.processTerm(t);
    boost::algorithm::trim_if(STR, boost::is_any_of(" \n"));
    boost::algorithm::trim_if(STRTmp, boost::is_any_of(" \n"));
    strProcMap_r[STR] = STRTmp;
    strProcMap[STRTmp] = STR;
    STR = STRTmp;

    return STR;
}


map<pair<string, string>, int > indri::query::ConceptSelector::readOneDocConcepts(string documentName)
{
    cout << "indri::query::ConceptSelector::readTopDocConcepts: documentName: " << documentName << endl;

    string repName = param.get("indexFielded", "");
    indri::collection::Repository r;
    r.openRead( repName );
    indri::server::LocalQueryServer local(r);
    indri::collection::Repository::index_state state = r.indexes();
    map<pair<string, string>, int > goodConcepts;
    string fileName = conceptDocsDirectory + documentName + ".txt";
    ifstream file(fileName.c_str());
    if(!file)
        throw runtime_error("Error: error in opening topDocUmlsConcepts: " + fileName);
    string line;
    while(getline(file, line))
    {
        stringstream lineSs(line);
        string STY, STR;
        string frq_;
        getline(lineSs, STY, ';');
        getline(lineSs, STR, ';');
        getline(lineSs, frq_, ';');
        int frq = atoi(frq_.c_str());
        boost::algorithm::trim_if(STY, boost::is_any_of(" \n"));

        vector<string> STR_;
        boost::split(STR_,STR,boost::is_any_of(" "));
        string STRTmp = "";
        for (auto t: STR_)
            STRTmp += " " + r.processTerm(t);
        boost::algorithm::trim_if(STR, boost::is_any_of(" \n"));
        boost::algorithm::trim_if(STRTmp, boost::is_any_of(" \n"));
        strProcMap_r[STR] = STRTmp;
        strProcMap[STRTmp] = STR;
        STR = STRTmp;

//        cout << "indri::query::ConceptSelector::readTopDocConcepts: STR, STY: " << STR << ", " << STY << endl;

        boost::algorithm::trim_if(STR, boost::is_any_of(" \n"));

        if(goodConcepts.find(make_pair(STY,STR))!=goodConcepts.end())
            throw runtime_error("more than one pair of (STR, STY) = ( " + STR + ", " + STY + " ) is found for document: " + documentName);
        goodConcepts.insert(make_pair(make_pair(STY,STR), frq));
    }
    r.close();
    return goodConcepts;
}

void indri::query::ConceptSelector::writeTopDocConcepts(map<pair<string, string>, int > goodConcepts, string documentName)
{
    string fileName = conceptDocsDirectory + documentName + ".txt";
    cout << "writeTopDocUmlsConcepts: documentName: " << documentName << " fileName = " << fileName << endl;
    ofstream file(fileName.c_str());
    for ( auto styStr: goodConcepts)
    {
        string STY = styStr.first.first;
        string STR = styStr.first.second;
        int frq = styStr.second;
        boost::algorithm::trim_if(STY, boost::is_any_of(" \n"));
        boost::algorithm::trim_if(STR, boost::is_any_of(" \n"));

        cout<<"writeTopDocUmlsConcepts: "<<STY<<";"<<strProcMap.at(STR)<<";"<<frq<<";"<< endl;
        file<<STY<<";"<<strProcMap.at(STR)<<";"<<frq<<";"<< endl;
    }
//    string docsListMetaMapped = conceptDocsDirectory + "docsListMetaMapped.txt";
    string listFileName_ = conceptDocsDirectory + listFileName;
    ofstream file_(listFileName_.c_str(), std::ios_base::app);
    file_<<documentName<< endl;
}


vector< pair< string, map<pair<string, string>, int> > > indri::query::ConceptSelector::findOneQuConceptCands(
        vector<string> topDocsNames, vector<pair<string, string> > oneQuBaseRescConcs, indri::api::QueryEnvironment & env )
{
    vector< pair< string, map<pair<string, string>, int > > > conceptCands;

    // if this is a prf-based selector, i.e., if the concepts should be extracted from topDocs.
    // concept-relationship-based selector are in diffent here from prf methods that they take some concepts as their inputs
    // however, both needs top-ranked documents, one in candidate selection part and scoring and the other only in scoring.
    // So, concept-relationship selectors are not fully independent of top-ranked documents.
    if(oneQuBaseRescConcs.size()>0)
    {
        for (auto oneBaseRescConc: oneQuBaseRescConcs)
        {
            map<pair<string, string>, int> oneBaseConcConceptCands = findOneBaseConcConceptCands(
                    oneBaseRescConc, env);
            conceptCands.push_back(make_pair(oneBaseRescConc.second, oneBaseConcConceptCands));
        }
    }
    else if(topDocsNames.size()>0)
    {
        for (auto topDocName: topDocsNames)
        {
            map<pair<string, string>, int> conceptCandsUmlsOneDoc = findOneDocConceptCands(
                    topDocName, env);
            conceptCands.push_back(make_pair(topDocName, conceptCandsUmlsOneDoc));
        }
    }
//    cout << "indri::query::ConceptSelector::findOneQuConceptCands: number of concept Cands for query: " << conceptCands.size() << endl;
    return conceptCands;
}

map<pair<string, string>, int> indri::query::ConceptSelector::findOneDocConceptCands(
        string documentName, indri::api::QueryEnvironment & env)
{
//    cout << "get good concepts..." << endl;
    map<pair<string, string>, int> goodConcepts;

    if(isDocIndexed(documentName))
        goodConcepts = readOneDocConcepts(documentName);
    else
        goodConcepts = findOneDocConceptCands_(documentName, env);
    return goodConcepts;
}
map<pair<string, string>, int> indri::query::ConceptSelector::findOneBaseConcConceptCands(
        pair<string, string> oneBaseRescConc, indri::api::QueryEnvironment & env)
{
    // because of the computation spped, no need to read or write the related concepts in a text file
    return findOneBaseConcConceptCands_(oneBaseRescConc, env);
}

vector<pair<string, string> > indri::query::ConceptSelector::oneQuGatherCombFeaturesSelec(
        vector<string> topDocsNames,
        vector<pair<string, string> > oneQuBaseRescConcs,
        vector<string> resourceNames_,
        string qId,
        indri::api::QueryEnvironment & env,
        indri::query::QueryReformulator * queryReformulator
)
{
    cout << "gatherFeaturesSelec: contacenate Good Concepts from all the top ranked documents..." << endl;

    indri::collection::Repository r;
    string repName = param.get("index", "");
    r.openRead( repName );

    // find concept candidates in all of the top-ranked documents:
    vector< pair< string, map<pair<string, string>, int> > > conceptCands =
            findOneQuConceptCands(topDocsNames, oneQuBaseRescConcs, env);

    // find statistics of concepts:
    pair<vector<pair<string, string> >, map<string, double > > tmpRes = preProcessConceptsStats(conceptCands, env);
    vector<pair<string, string> > concatenatedConceptCands = tmpRes.first;
    map<string, double > meanFreqConceptsCands = tmpRes.second;


    vector<pair<string, string> > concatenatedConceptCandsSelected =
            combinFeaturesSelec(concatenatedConceptCands, meanFreqConceptsCands,
                    qId, topDocsNames, env, queryReformulator, resourceNames_ );

    cout << "qId = " << qId  << " contacenateGoodConceptsOfDocumentVec = total number of good concepts in top docuemnts = " << concatenatedConceptCandsSelected.size() << endl;
    r.close();
    return concatenatedConceptCandsSelected;
}

pair<vector<pair<string, string> >, map<string, double > > indri::query::ConceptSelector::preProcessConceptsStats(vector< pair< string, map<pair<string, string>, int> > > conceptCands,
        indri::api::QueryEnvironment & env)
{
    set<string> uniqueConceptCands;
    vector<pair<string, string> > concatenatedGoodConcepts;
    map<string, multiset<int> > freqConceptCands;
    map<string, double > meanFreqConceptsCands;
    for(auto goodConceptsOfDocumentVec_: conceptCands) // for all the top-ranked documents
    {
        string topDocumentName = goodConceptsOfDocumentVec_.first;
        for(auto _goodConceptsOfDocumentVec_: goodConceptsOfDocumentVec_.second) // for all the extracted concepts
        {
            string concName = _goodConceptsOfDocumentVec_.first.second;
            concName = boost::algorithm::trim_copy_if(concName, boost::algorithm::is_any_of(" "));
            string concSty = _goodConceptsOfDocumentVec_.first.first;
            int concFrq = _goodConceptsOfDocumentVec_.second;

            freqConceptCands[concName].insert(concFrq);

            // if not already examined and if not having concept name to be empty or very small after stemming
            if (conceptsFrq.find(concName)==conceptsFrq.end() && concName.size() > 2 )
            {
//                string expression = "#uw17( " + strProcMap.at(concName) + " )" ;
                string expression = "#od4( " + strProcMap.at(concName) + " )" ;

                std::cout << "gatherFeaturesSelec: expression = " << expression << std::endl;
                double resultExpr = env.expressionCount( expression );
                std::cout << "gatherFeaturesSelec: resultExpr = " << resultExpr << std::endl;
                conceptsFrq[concName] = resultExpr; // if by frq, we mean number of all candidates

                cout << "gatherFeaturesSelec: size of concepts for this query: "
                        << conceptsFrq[concName]
                        << " "
                        << conceptsFrq.size()
                        << endl;
            }
            // if not already added to the concatenated good concepts
            if (uniqueConceptCands.find(concName)==uniqueConceptCands.end() && concName.size() > 2 )
            {
                uniqueConceptCands.insert(concName);
                concatenatedGoodConcepts.push_back(make_pair(concSty,concName));
            }
        }
    }
    for(auto c: freqConceptCands)
    {
        multiset<int> goodConceptFreq = c.second;
        double sum = 0;
        for(auto i: goodConceptFreq)
            sum += i;
        string conceptName = c.first;
        meanFreqConceptsCands[conceptName] = sum/double(conceptCands.size());
    }
    return {concatenatedGoodConcepts, meanFreqConceptsCands};
}

vector<pair<string, string> > indri::query::ConceptSelector::combinFeaturesSelec(
        vector<pair<string, string> > concatenatedGoodConcepts,
        map<string, double > goodConceptsTpRnkFreqMean,
        string qId,
        vector<string> topDocsNames,
        indri::api::QueryEnvironment & env,
        indri::query::QueryReformulator * queryReformulator,
        vector<string> resourceNames_)
{
    vector<pair<string, string> > concatenatedBestConcepts;

    multimap<double, pair<string, string> > scoredConceptsPrf_norm = conceptSelectorFuns->normConceptScorePrf(
                                                                                                concatenatedGoodConcepts,
                                                                                                qId,
                                                                                                topDocsNames,
                                                                                                env,
                                                                                                queryReformulator,
                                                                                                resourceNames_);

    map<string, double> conceptsFrqExtr_norm1 = conceptSelectorFuns->normConceptScoreFreq(
                                                                            env,
                                                                            conceptsFrq,
                                                                            scoredConceptsPrf_norm );

    map<string, double> goodConceptsTpRnkFreqMean_norm1 = conceptSelectorFuns->normConceptScoreTpRnkFreqMean(
                                                                                        goodConceptsTpRnkFreqMean,
                                                                                        conceptsFrq );
		/*
    // combine values of the scores
    string coeff_prf_s = oneResourceConceptsParams.resourceName + "_coeff_prf" + "_value";
    string coeff_idf_s = oneResourceConceptsParams.resourceName + "_coeff_idf" + "_value";
    string coeff_idfTpRnk_s = oneResourceConceptsParams.resourceName + "_coeff_idfTpRnk" + "_value";
    if(!param.exists( coeff_prf_s ))
        throw runtime_error("indri::query::ConceptSelector::combinFeaturesSelec: " + coeff_prf_s + " does not exist in parameter object.");
    if(!param.exists( coeff_idf_s ))
        throw runtime_error("indri::query::ConceptSelector::combinFeaturesSelec: " + coeff_idf_s + " does not exist in parameter object.");
    if(!param.exists( coeff_idfTpRnk_s ))
        throw runtime_error("indri::query::ConceptSelector::combinFeaturesSelec: " + coeff_idfTpRnk_s + " does not exist in parameter object.");

    double coeff_prf = param.get(coeff_prf_s, 0.0);
    double coeff_idf = param.get(coeff_idf_s, 0.0);
    double coeff_idfTpRnk = param.get(coeff_idfTpRnk_s, 0.0);
		*/
    double coeff_prf = oneResourceConceptsParams.getCoeff_prf();
    double coeff_idf = oneResourceConceptsParams.getCoeff_idf();
    double coeff_idfTpRnk = oneResourceConceptsParams.getCoeff_idfTpRnk();
	//cout << "indri::query::ConceptSelector::combinFeaturesSelec: coeff_idf: " << coeff_idf << endl;
	//cout << "indri::query::ConceptSelector::combinFeaturesSelec: coeff_idfTpRnk: " << coeff_idfTpRnk << endl;

    multimap<double, pair<string, string>, std::greater<double> > scoredConcepts;
    for (auto itSc = scoredConceptsPrf_norm.begin(); itSc != scoredConceptsPrf_norm.end(); itSc++)
    {
        double conceptScore = coeff_prf*itSc->first -
                coeff_idf       * conceptsFrqExtr_norm1[(itSc->second).second] +
                coeff_idfTpRnk  * goodConceptsTpRnkFreqMean_norm1[(itSc->second).second];
        scoredConcepts.insert(make_pair(conceptScore, make_pair((itSc->second).first, (itSc->second).second)));
        cout << "indri::query::ConceptSelector::combinFeaturesSelec: scoredConcepts: "
                << (itSc->second).first << " " << (itSc->second).second << endl
                << "\t\t" << conceptScore << " <-  "
                << coeff_prf << "*" << itSc->first
                << " - " << coeff_idf  << "*" << conceptsFrqExtr_norm1[(itSc->second).second]
                << " + " << coeff_idfTpRnk << "*" << goodConceptsTpRnkFreqMean_norm1[(itSc->second).second]
                << endl;
    }

    signed int counter=0;
    for( auto c: scoredConcepts )
    {
        pair<string, string> concs = c.second;
        if (counter++ < fbConcs)
        {
            concatenatedBestConcepts.push_back(make_pair(concs.first, strProcMap.at(concs.second)));
            cout << "indri::query::ConceptSelector::combinFeaturesSelec: --chosen-- counter, STY, STR, and score of best concepts: " << counter << " " << concs.first << " " << concs.second << " " << c.first << endl;
        }
        else
        {
            cout << "indri::query::ConceptSelector::combinFeaturesSelec: --rejected-- counter, STY, STR, and score of best concepts: " << counter << " " << concs.first << " " << concs.second << " " << c.first << endl;
        }
    }
    return concatenatedBestConcepts;
}


vector< pair< string, vector<pair<string, string > > > > indri::query::ConceptSelector::findConceptCands(
        vector< pair<std::string, std::string> > queriesText, indri::query::QueryReformulator * queryReformulator, vector<string> resourceNames_,
        vector<pair<string, vector<string> > > topDocsNames,
        vector<pair<string, vector<pair<string, string> > > > baseRescConcs)
{

    indri::api::QueryEnvironment env;

    string indexName = param.get("index", "");

    // compute the expression list using the QueryEnvironment API
    env.addIndex( indexName );

    cout << "indri::query::ConceptSelector::findConceptCands: number of queries: " << topDocsNames.size() << endl;
    vector<string> qIdV;
    if (topDocsNames.size()>0)
        for (auto q: topDocsNames)
            qIdV.push_back(q.first);
    else if (baseRescConcs.size()>0)
        for (auto q: baseRescConcs)
            qIdV.push_back(q.first);

    //for (auto q: queriesText)
    		//cout <<"indri::query::ConceptSelector::findConceptCands: queriesText: "  << q.first << " - " << q.second << endl;


    vector< pair< string, vector<pair<string, string > > > > conceptCands;
    int counter = 0;
    for (auto qId: qIdV)
    {
        cout << "indri::query::ConceptSelector::findConceptCands: qId: " << qId << endl;
        string qId_;
        string queryText;
		do{
				qId_ = queriesText[counter].first;
				queryText = queriesText[counter++].second;
		}while(queryText.size()<3); // to ignore queries belong to testing step

        if(qId_ != qId)
            throw runtime_error("indri::query::ConceptSelector::findConceptCands: error in query IDs set:" + qId_ + "!=" + qId);

        vector<string> oneQuTopDocsNames;

        vector<pair<string, string> > oneQuBaseRescConcs;

        for (auto qConc: baseRescConcs)
            if (qConc.first == qId)
                oneQuBaseRescConcs = qConc.second;
        cout << "indri::query::ConceptSelector::findConceptCands: size of oneQuBaseRescConcs: " << oneQuBaseRescConcs.size() << endl;

        for (auto qDoc: topDocsNames)
            if (qDoc.first == qId)
                oneQuTopDocsNames = qDoc.second;
        cout << "indri::query::ConceptSelector::findConceptCands: size of oneQuTopDocsNames: " << oneQuTopDocsNames.size() << endl;

        vector<pair<string, string> > conceptCands_ = oneQuGatherCombFeaturesSelec(oneQuTopDocsNames,
                oneQuBaseRescConcs,
                resourceNames_,
                qId,
                env,
                queryReformulator);

        conceptCands.push_back(make_pair(qId, conceptCands_));
    }

    for(auto qc: conceptCands)
    {
        cout << "indri::query::ConceptSelector::findConceptCands: query, concept: " << qc.first << ", ";
        for (auto c: qc.second)
            cout << c.first << " -- " << c.second << " ---- ";
        cout << endl;
    }
    cout << "indri::query::ConceptSelector::findConceptCands: number of queries in conceptCands: " << conceptCands.size() << endl;

    env.close();
    return conceptCands;
}
