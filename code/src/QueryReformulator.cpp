#include "../include/QueryReformulator.hpp"
#include "../include/ConceptSelector.hpp"
#include "../include/UmlsOrigMetaMapConceptSelector.hpp"
#include "../include/UnigramsOrigConceptSelector.hpp"

indri::query::QueryReformulator::QueryReformulator(indri::api::Parameters& param, vector<indri::query::OneResourceConceptsParams*> allResourceConceptsParams_): allResourceConceptsParams(allResourceConceptsParams_), _param ( param )
{
    extResourceProcessor = new indri::query::ExtResourceProcessor(param);
    goodSTYs_v = wsuIr::expander::Utility::readGoodSTYs(param);
    //umlsOrigMetaMap_ConceptsParams = new OneResourceConceptsParams("umlsOrigMetaMap", _param);
//    origQueriesTexts = setOrigQuery(_queries);
    /*
    allResourceConceptsParams =
    {
        new OneResourceConceptsParams("unigramsOrig", _param),
        new OneResourceConceptsParams("umlsOrigMetaMap", _param),
        new OneResourceConceptsParams("bigramsPrfTermMatch", _param),
        new OneResourceConceptsParams("umlsPrfMetaMap", _param),
    };
    */
}

void indri::query::QueryReformulator::setQueries(std::queue< indri::query::query_t* > queries)
{
    _queries_ = queries;
}


indri::query::QueryReformulator::~QueryReformulator()
{
    delete extResourceProcessor;
    //delete umlsOrigMetaMap_ConceptsParams;
    //for (auto resrc: allResourceConceptsParams)
    //    delete resrc;
    //allResourceConceptsParams.clear();

}


// generate the first-layer expansion part of the query
string indri::query::QueryReformulator::genQueryTextOneQuOneCT(vector<pair<string, string> > oneLayerConcepts, string resourceName_, double lambdaT, double lambdaPh, double lambdaPr )
{
    // iterate over the one layer concepts:
    string oneLayerConceptsText = "#weight( \n";
    for(auto c: oneLayerConcepts)
    {
        double conceptWeight = 1;

        string STR = c.second;
        boost::replace_all(STR, ", nos", " ");
        boost::replace_all(STR, "nos", " ");
        boost::replace_all(STR, ", NOS", " ");
        boost::replace_all(STR, "NOS", " ");
        boost::replace_all(STR, ", Nos", " ");
        boost::replace_all(STR, ", Nos", " ");
        boost::algorithm::trim_copy_if(STR, boost::algorithm::is_any_of("\n"));
        boost::algorithm::trim_copy_if(STR, boost::algorithm::is_any_of(" "));

        vector<string> str_v;
        boost::split(str_v, STR, boost::is_any_of(","), boost::token_compress_on); // if it has a comma, write whatever after comma in the begining of the term
        if (str_v.size()>1) STR = str_v[1] + " " + str_v[0];

        string STY = c.first;
        boost::replace_all(STY, ",", "");

        // if this concept has a good symantic type
        if ((std::find(goodSTYs_v.begin(), goodSTYs_v.end(), STY) != goodSTYs_v.end()
                && goodSTYs_v[0] != "na") || (goodSTYs_v[0] == "all") || STY == "GOOD")
        {
            vector<string> combs;
            if(resourceName_ == "unigramsOrig")
            {
                combs.push_back(STR);
            }
            else
            {
                std::vector<std::string> words;
                boost::split(words, STR, boost::is_any_of(" "), boost::token_compress_on);

                string queryIndvTerms_ = "";

                string tmp = "";
                for (auto w: words)
                {
                    if(w.size() > 2)
                    {
                        if(tmp.size() > 1)
                        {
                            string comb;
                            comb = tmp + " " + w;
                            combs.push_back(comb);
                        }
                        queryIndvTerms_ = queryIndvTerms_ + w + " " ;
                        tmp = w;
                    }

                }
                if (combs.size()==0)
                {
                    combs.push_back(STR);
                }
            }

//            string queryIndvTerms = " #combine( " + queryIndvTerms_ + ") \n";
//            string queryPrs = " #uw17( " + queryIndvTerms_ + ") \n";

            string queryIndvTerms = " #combine( ";
            string queryPhs = " #combine( ";
            string queryPrs = " #combine( ";
            for (auto cm: combs)
            {
                queryIndvTerms = queryIndvTerms + "#combine(" + cm + ") ";
                queryPhs = queryPhs + "#od4(" + cm + ") ";
                queryPrs = queryPrs + "#uw17(" + cm + ") ";
            }
            queryIndvTerms = queryIndvTerms + ") \n";
            queryPhs = queryPhs + ") \n";
            queryPrs = queryPrs + ") \n";
            oneLayerConceptsText += "\t\t" + to_string(conceptWeight)
                    + " #weight(\n" + "\t\t\t";
            if (lambdaT>0.00001)
            {
                oneLayerConceptsText += to_string(lambdaT)
                        +  queryIndvTerms + "\t\t\t";
            }
            if (lambdaPh>0.00001)
            {
                oneLayerConceptsText += to_string(lambdaPh)
                        + queryPhs + "\t\t\t";
            }
            if (lambdaPr>0.00001)
            {
                oneLayerConceptsText += to_string(lambdaPr)
                        + queryPrs + "\t\t\t";
            }
            oneLayerConceptsText += ")\n";
        }
    }
    oneLayerConceptsText += "\t\t)\n";
    return oneLayerConceptsText;
}

// add up all the layers of the expansion and the original query with bag of words
// each tuple corresponds to one concept
// each vector of tuples corresponds to one concept type
string indri::query::QueryReformulator::genOneQueryText( vector<tuple<vector<pair<string, string> >, tuple<double, double, double, double>, string > > oneQuResourcesConceptsParams )
{
    //string allConcepts = "#weight(\n";
    string _allConcepts_ = "";
    for(auto concsParams: oneQuResourcesConceptsParams) // for each concept types
    {
//        if (!(_param.exists("lambdaT_"+concsParams.first)&&_param.exists("lambdaPh_"+concsParams.first)&&_param.exists("lambdaPr_"+concsParams.first)&&_param.exists("lambdaMrf_"+concsParams.first)))
//            throw runtime_error("some of lambda parameters are not defined:" "lambdaT_"+concsParams.first+", "+"lambdaPh_"+concsParams.first+", "+"lambdaPr_"+concsParams.first+" or "+"lambdaMrf_"+concsParams.first );
        double lambdaT = get<0>(get<1>(concsParams));
        double lambdaPh = get<1>(get<1>(concsParams));
        double lambdaPr = get<2>(get<1>(concsParams));
        double lambdaResrc = get<3>(get<1>(concsParams));

//        cout << "indri::query::QueryReformulator::genOneQueryText: lambdaT = " << lambdaT << " lambdaPh = "
//                << lambdaPh << " lambdaPr = " << lambdaPr << " lambdaMrf  = " << lambdaMrf << endl;

        string allConcepts_ = genQueryTextOneQuOneCT(get<0>(concsParams), get<2>(concsParams),
                lambdaT, lambdaPh, lambdaPr);
        if(std::count(allConcepts_.begin(), allConcepts_.end(), '\n') > 2) // if any concept added to this query
        {
            _allConcepts_ += "\t" + to_string(lambdaResrc) + " "
                    + allConcepts_;
        }
    }
    string allConcepts = "";
    if(_allConcepts_.size() > 2)
        allConcepts += "#weight(\n" +  _allConcepts_ + ")\n";
    else
        allConcepts = "a\n";
    return allConcepts;
}


vector<pair<string, string> > indri::query::QueryReformulator::genQueryTextResources(vector<string> resourceNames, vector<string> qIdWorkset, pair<string,string> conceptExt, tuple<double, double, double, double> lambdasExt)
{
    // find the index of the first non-empty element of the resource-query-concepts
    signed int nonEmptyIndex = 0;
    for(nonEmptyIndex = 0; nonEmptyIndex < static_cast<signed int>(allResourceConceptsParams.size()); nonEmptyIndex++)
        if (allResourceConceptsParams[nonEmptyIndex]->getOneResourceConcepts().size()>0)
            break;

    vector<pair<string, string> > queriesTexts;
    for (auto resrcConcs: allResourceConceptsParams[nonEmptyIndex]->getOneResourceConcepts()) // for each query
    {
        // check if this query is in the workset:
        string qId = resrcConcs.first;
        if ((std::find(qIdWorkset.begin(), qIdWorkset.end(), qId  )==qIdWorkset.end()) && // if it is not qId AND
                (std::find(qIdWorkset.begin(), qIdWorkset.end(), "ALL")==qIdWorkset.end()))// if it is not ALL
            continue;

        string queryText;
        vector<tuple<vector<pair<string, string> >, tuple<double, double, double, double>, string > > oneQuResourcesConceptsParams;
        // for each resources:
        for(auto resourceConceptsParams: allResourceConceptsParams)
        {
            if(std::find_if(resourceNames.begin(), resourceNames.end(), [resourceConceptsParams](string const& s)
        {
            return s == resourceConceptsParams->resourceName;
        }) != resourceNames.end())
            {
                string resourceName_ = resourceConceptsParams->resourceName;
                auto allQuResourceConcepts = resourceConceptsParams->getOneResourceConcepts();
                // find concepts of this resource that belongs to this query:
                auto it = find_if( allQuResourceConcepts.begin(), allQuResourceConcepts.end(), [&qId](const pair<string, vector<pair<string, string> > > & element)
                {
                    return ((element.first == qId));
                } );
                if(it == allQuResourceConcepts.end())
                		continue;
                    //throw runtime_error("indri::query::QueryReformulator::genQueryTextResources: qId: " + qId + " does not exist in resource: " + resourceConceptsParams->resourceName);

                vector<pair<string, string> > oneQuResourceConcepts = it->second;

                tuple<double, double, double, double> lambdas = make_tuple(
                        resourceConceptsParams->getLambdaTR(),
                        resourceConceptsParams->getLambdaPH(),
                        resourceConceptsParams->getLambdaPR(),
                        resourceConceptsParams->getLambdaResrc()
//                            get<0>(resourceConceptsParams->lambdaTR),
//                            get<0>(resourceConceptsParams->lambdaPH),
//                            get<0>(resourceConceptsParams->lambdaPR),
//                            get<0>(resourceConceptsParams->lambdaResrc)
                        );

                oneQuResourcesConceptsParams.push_back(make_tuple(oneQuResourceConcepts, lambdas, resourceName_));

            }
        }

        // add extra (not necessarily categorized) concepts:
        if(conceptExt.second.size()>0)
            oneQuResourcesConceptsParams.push_back(make_tuple(vector<pair<string, string> > {conceptExt}, lambdasExt, ""));

        queryText = genOneQueryText( oneQuResourcesConceptsParams );
        queriesTexts.push_back(make_pair(qId, queryText));

//        cout << "indri::query::QueryReformulator::genQueryTextResources: size of qId = " << qId << endl;
//        cout << "indri::query::QueryReformulator::genQueryTextResources: size of queriesTexts = " << queriesTexts.size() << endl;

    }
//    cout << "indri::query::QueryReformulator::genQueryTextResources: size of queriesTexts = " << queriesTexts.size() << endl;
    //queriesText = genQueryText(allQusResourcesConceptsParams);
    return queriesTexts;
}

// keep the track of concepts that already detected for different resources to avoid over-calculatrtion:
bool indri::query::QueryReformulator::checkSelectionNecessity(vector<string> resourceNames, string resourceName)
{
    // if this resouceName corresponds to one of the chosen resouces
    bool check1 = (std::find_if(resourceNames.begin(), resourceNames.end(), [& resourceName](string const& s)
    {
        return s == resourceName;
    }) != resourceNames.end());

    auto it2 = std::find_if(allResourceConceptsParams.begin(), allResourceConceptsParams.end(), [& resourceName](const OneResourceConceptsParams * resrc)
    {
        return resrc->resourceName == resourceName;
    });
//    // there should be one element in allResourceConceptsParams correspond to each resource
//    if(it2 == allResourceConceptsParams.end())
//        throw runtime_error("indri::query::QueryReformulator::checkSelectionNecessity: resource: " + resourceName + " does not exist in allResourceConceptsParams");

    // if this resource is not already examined
    bool check2 = ((*it2)->oneResourceConcepts.size() == 0);

    cout << "indri::query::QueryReformulator::checkSelectionNecessity: resource = " << resourceName << endl;
    cout << "indri::query::QueryReformulator::checkSelectionNecessity: check1 = " << check1 << endl;
    cout << "indri::query::QueryReformulator::checkSelectionNecessity: check2 = " << check2 << endl;

    bool check = (check1 && check2);

    cout << "indri::query::QueryReformulator::checkSelectionNecessity: check = " << check << endl;
    return check;
}

// a function similar to reformulateQuery that instead of getting new concepts from the resources, it will get it as an argument.
// by calling this function one concept of one resource will be added to one of the queries
vector<pair<string, string> > indri::query::QueryReformulator::testOneConceptAddition2OneQuery(string conceptSty, string conceptStr, string qId, vector<string> resourceNames)
{
    vector<pair<string, string> > queriesTexts;

    queriesTexts = genQueryTextResources(resourceNames, vector<string> {qId}, make_pair(conceptSty, conceptStr), make_tuple(0.8,0.1,0.1,0.05));

    auto it2 = std::find_if(queriesTexts.begin(), queriesTexts.end(), [& qId](const pair<string, string> & qPairs)
    {
        return qPairs.first == qId;
    });

    if(it2 == queriesTexts.end())
        throw runtime_error("indri::query::QueryReformulator::testOneConceptAddition2OneQuery: qId " + qId + "does not exist in the queriesTexts.");

    return vector<pair<string, string> > {make_pair(it2->first, it2->second)};
}

void indri::query::QueryReformulator::applyNewParam2OneResourceConceptsParams(tuple<string, string, string, double, double, double, double> t)
{
    string paramOrRuleParams = std::get<0>(t);
    string paramName         = std::get<1>(t);
    string intOrDouble       = std::get<2>(t);
    //double stepSize          = std::get<3>(t);
    double paramValue        = std::get<4>(t);
    /*
    cout << "indri::query::QueryReformulator::applyNewParam2OneResourceConceptsParams: "
            << " paramOrRuleParams = " << paramOrRuleParams << " paramName = " << paramName
            << " intOrDouble = " << intOrDouble << " stepSize = " << stepSize << " paramValue = " << paramValue << endl;
    */
    vector<string> paramName_v;
    boost::split(paramName_v, paramName, boost::is_any_of("_"));
    string resourceName = paramName_v[0];
    string paramName_ = paramName_v[1];
    auto it2 = std::find_if(allResourceConceptsParams.begin(), allResourceConceptsParams.end(), [& resourceName](const OneResourceConceptsParams * resrc)
    {
        return resrc->resourceName == resourceName;
    });
    // there should be one element in allResourceConceptsParams coreespond to each resource
    if(it2 == allResourceConceptsParams.end())
        throw runtime_error("indri::query::QueryReformulator::applyNewParam2OneResourceConceptsParams: resource: " + resourceName + " does not exist in allResourceConceptsParams");

    if( (get<0>((*it2)->fbDocs) != _param.get(resourceName+"_"+"fbDocs"+"_value", 0))  ||
            (get<0>((*it2)->coeff_prf) != _param.get(resourceName+"_"+"coeff_prf"+"_value", 0))||
            (get<0>((*it2)->coeff_idf) != _param.get(resourceName+"_"+"coeff_idf"+"_value", 0))||
            (get<0>((*it2)->coeff_idfTpRnk) != _param.get(resourceName+"_"+"coeff_idfTpRnk"+"_value", 0))||
            (get<0>((*it2)->thrshMed) != _param.get(resourceName+"_"+"thrshMed"+"_value", 0))
      )
    {
        // oneResourceConcepts is not valid when fbDocs or fbConcs is changed.
        (*it2)->oneResourceConcepts.clear();
//        cout << "indri::query::QueryReformulator::applyNewParam2OneResourceConceptsParams: clearing oneResourceConcepts." << endl;
//        cout << "fbDocs (object, param) = " << get<0>((*it2)->fbDocs) << ", " << _param.get(resourceName+"_"+"fbDocs"+"_value", 0) << endl;
//             << "fbConcs (object, param) = " << get<0>((*it2)->fbConcs) << ", " << _param.get(resourceName+"_"+"fbConcs"+"_value", 0) << endl;
    }

    cout << "indri::query::QueryReformulator::applyNewParam2OneResourceConceptsParams:" << endl;
    cout << resourceName+"_"+"lambdaTR"+"_value: " << get<0>((*it2)->lambdaTR) << " <--> " << _param.get(resourceName+"_"+"lambdaTR"+"_value", 0.0) << endl;
    cout << resourceName+"_"+"lambdaPH"+"_value: " << get<0>((*it2)->lambdaPH) << " <--> " << _param.get(resourceName+"_"+"lambdaPH"+"_value", 0.0) << endl;
    cout << resourceName+"_"+"lambdaPR"+"_value: " << get<0>((*it2)->lambdaPR) << " <--> " << _param.get(resourceName+"_"+"lambdaPR"+"_value", 0.0) << endl;
    cout << resourceName+"_"+"lambdaResrc"+"_value: " << get<0>((*it2)->lambdaResrc) << " <--> " << _param.get(resourceName+"_"+"lambdaResrc"+"_value", 0.0) << endl;
    cout << resourceName+"_"+"fbDocs"+"_value: " << get<0>((*it2)->fbDocs) << " <--> " << _param.get(resourceName+"_"+"fbDocs"+"_value", 0) << endl;
    cout << resourceName+"_"+"fbConcs"+"_value: " << get<0>((*it2)->fbConcs) << " <--> " << _param.get(resourceName+"_"+"fbConcs"+"_value", 0) << endl;
    cout << resourceName+"_"+"coeff_prf"+"_value: " << get<0>((*it2)->coeff_prf) << " <--> " << _param.get(resourceName+"_"+"coeff_prf"+"_value", 0.0) << endl;
    cout << resourceName+"_"+"coeff_idf"+"_value: " << get<0>((*it2)->coeff_idf) << " <--> " << _param.get(resourceName+"_"+"coeff_idf"+"_value", 0.0) << endl;
    cout << resourceName+"_"+"coeff_idfTpRnk"+"_value: " << get<0>((*it2)->coeff_idfTpRnk) << " <--> " << _param.get(resourceName+"_"+"coeff_idfTpRnk"+"_value", 0.0) << endl;

    /*
        if(paramName_ == "lambdaTR")
            get<0>((*it2)->lambdaTR) = paramValue;
        if(paramName_ == "lambdaPH")
            get<0>((*it2)->lambdaPH) = paramValue;
        if(paramName_ == "lambdaPR")
            get<0>((*it2)->lambdaPR) = paramValue;
        if(paramName_ == "lambdaResrc")
            get<0>((*it2)->lambdaResrc) = paramValue;
    */
    if(paramName_ == "fbDocs")
        get<0>((*it2)->fbDocs) = int(paramValue);
    if(paramName_ == "fbConcs")
        get<0>((*it2)->fbConcs) = int(paramValue);

}

vector<pair<string, string> >  indri::query::QueryReformulator::reformulateQuery(vector<string> resourceNames, map<string, string> STYs,
        map<string, set<string> > wikiMedicalTermsExpr, map<string, vector< pair<string, int> > > MRREL_CUI1_CUI2, map<string, string> MRCONSO_simp,
        map<string, set<string> > MRSTY_simp, map<string, string> styAbbrsTuis, vector<string> qIdWorkset)
{
    cout << "indri::query::QueryReformulator::reformulateQuery: size of resourceNames " << resourceNames.size() << endl; //"_param.get(paramName, 0 ) = " << _param.get(paramName, 0.0 ) << endl;
    vector<pair<string, string> > queriesTexts;

    for (auto resrc: resourceNames)
    {
        cout << "indri::query::QueryReformulator::reformulateQuery: resourceNames: " << resrc << endl;
    }

    vector<pair<string, indri::query::ConceptSelector*> > conceptSelectors;

    for(auto resrcConcParams: allResourceConceptsParams)
    {
        string resourceName_ = resrcConcParams->resourceName;
        cout << "indri::query::QueryReformulator::reformulateQuery: resourceName_ = " << resourceName_ << endl;
        if(resourceName_ == "unigramsOrig" && checkSelectionNecessity(resourceNames, "unigramsOrig"))
            conceptSelectors.push_back(make_pair(resourceName_, new indri::query::UnigramsOrigConceptSelector(wikiMedicalTermsExpr, STYs, _param, *resrcConcParams)));
        if(resourceName_ == "umlsOrigMetaMap" && checkSelectionNecessity(resourceNames, "umlsOrigMetaMap"))
            conceptSelectors.push_back(make_pair(resourceName_, new indri::query::UmlsOrigMetaMapConceptSelector(wikiMedicalTermsExpr, STYs, _param, *resrcConcParams)));
    }
    if(_queries_.size()==0)
        throw runtime_error("indri::query::QueryReformulator:: the original query (_queries_) is empty.");

    // initialize queryText with the ones of the original query:
    queriesTexts = setOrigQuery(_queries_);
    if(queriesTexts.size()==0)
        throw runtime_error("indri::query::QueryReformulator:: the text of original query (queriesTexts) is empty.");

    for (auto cs: conceptSelectors)
    {
        string resourceName = cs.first;
        auto itResource = std::find_if(allResourceConceptsParams.begin(), allResourceConceptsParams.end(), [& resourceName](const OneResourceConceptsParams * resrc)
        {
            return resrc->resourceName == resourceName;
        });
//        cout << "indri::query::QueryReformulator::reformulateQuery: number of queries = " << (*itResource)->oneResourceConcepts.size() << endl;
        cout << "indri::query::QueryReformulator::reformulateQuery: resourceName = " << resourceName << endl;

        if(itResource == allResourceConceptsParams.end())
            throw runtime_error("concept selector is invoked for a source without constructor object ");

        vector<pair<string, vector<string> > > topDocsNames = wsuIr::expander::Utility::getTopDocsNames(get<0>((*itResource)->fbDocs));

        // if the concepts for this resource are not already selected or deleted and need to be selected again
        if ((*itResource)->oneResourceConcepts.size() == 0)
        {
            // umlsRelated need data from other resource umlsOrigMetaMap
            if((*itResource)->resourceName == "umlsRelated")
            {
                cout << "indri::query::QueryReformulator::reformulateQuery: extracting concepts for resource: " + (*itResource)->resourceName << endl;

                auto itUmlsRelated = std::find_if(allResourceConceptsParams.begin(), allResourceConceptsParams.end(), [](const OneResourceConceptsParams * resrc)
                {
                    return resrc->resourceName == "umlsOrigMetaMap";
                });
                if(itUmlsRelated == allResourceConceptsParams.end())
                    throw runtime_error("selection from umlsRelated resource should be always preformed after umlsOrigMetaMap. No concept for umlsOrigMetaMap is selected.");

                vector<pair<string, vector<pair<string, string> > > > umlsOrigMetaMapConcs = (*itUmlsRelated)->getOneResourceConcepts();
                (*itResource)->oneResourceConcepts = cs.second->findConceptCands(queriesTexts, this, resourceNames, topDocsNames, umlsOrigMetaMapConcs);
            }
            else if(((*itResource)->resourceName == "umlsPrfMetaMap")||((*itResource)->resourceName == "bigramsPrfTermMatch")
                    ||(*itResource)->resourceName == "htPrf"||(*itResource)->resourceName == "htBigramsPrf")
            {
                cout << "indri::query::ConceptSelector::findConceptCands: number of queries: " << topDocsNames.size() << endl;

                cout << "indri::query::QueryReformulator::reformulateQuery: extracting concepts for resource: " + (*itResource)->resourceName << endl;
                (*itResource)->oneResourceConcepts = cs.second->findConceptCands(queriesTexts, this, resourceNames, topDocsNames, vector<pair<string, vector<pair<string, string> > > >());
            }
            else
            {
                cout << "indri::query::QueryReformulator::reformulateQuery: extracting concepts for resource: " + (*itResource)->resourceName << endl;
                (*itResource)->oneResourceConcepts = cs.second->findConceptCands(queriesTexts, this, resourceNames, vector<pair<string, vector<string> > >(), vector<pair<string, vector<pair<string, string> > > >());

                cout << "indri::query::QueryReformulator::reformulateQuery: resource =  " << (*itResource)->resourceName << endl;
                /*
                for (auto qc: (*itResource)->getOneResourceConcepts())
                    for (auto c: qc.second)
                        cout << "\t\t\tget: q, CUI, STR = " << qc.first << ", " << c.first << ", " << c.second << endl;
                for (auto qc: (*itResource)->oneResourceConcepts)
                    for (auto c: qc.second)
                        cout << "\t\t\tdirect: q, CUI, STR = " << qc.first << ", " << c.first << ", " << c.second << endl;
                */
            }
        }
        if((*itResource)->oneResourceConcepts.size()==0)
            throw runtime_error("indri::query::QueryReformulator::reformulateQuery: size of oneResourceConcepts = 0 for source: " + (*itResource)->resourceName );
    }

    for(auto it = conceptSelectors.begin(); it != conceptSelectors.end(); it++)
        delete it->second;

    cout << "indri::query::QueryReformulator::reformulateQuery: number of concept selectors: " << allResourceConceptsParams.size() << endl;
    int counter = 1;
    for (auto sqc: allResourceConceptsParams) // for all the concept selectors
    {
        auto oneResourceConcepts_ = sqc->getOneResourceConcepts();

        for (auto qc: oneResourceConcepts_) // for all the queries
        {
            for (auto c: qc.second) // for all the concepts
            {
                cout << "indri::query::QueryReformulator::reformulateQuery: conceptCands: " << counter << "- " << sqc->resourceName << " "<< qc.first << " " << c.first << " " << c.second << endl;
            }
        }
        counter++;
    }

    queriesTexts = genQueryTextResources(resourceNames, qIdWorkset);
    return queriesTexts;
}

vector<pair<string, string> > indri::query::QueryReformulator::setOrigQuery(std::queue< indri::query::query_t* > _queries)
{
    vector<pair<string, string> > origQuery;
    origQuery.clear();
    int queryCount = _queries.size();
    for (int i = 0; i < queryCount; i++)
    {
        query_t* query_t_ = _queries.front();
        query_t* _query_t_ = new query_t(*query_t_);
        _queries.pop();

        string textOrig = query_t_->text;
//        cout << "indri::query::QueryReformulator::setOrigQuery: textOrig: " << textOrig << endl;
        origQuery.push_back(make_pair(query_t_->number, query_t_->text));

        _queries.push(query_t_);
        _queries_.push(_query_t_); // keep a copy of _queries
    }
    return origQuery;
}
