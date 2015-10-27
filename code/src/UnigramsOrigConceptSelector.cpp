#include "../include/UnigramsOrigConceptSelector.hpp"
#include <boost/algorithm/string.hpp>

indri::query::UnigramsOrigConceptSelector::UnigramsOrigConceptSelector( map<string, set<string> > wikiMedicalTermsExpr,  map<string, string> STYs, indri::api::Parameters& param_, OneResourceConceptsParams& oneResourceConceptsParams_ )
    :ConceptSelector(wikiMedicalTermsExpr, STYs, param_, oneResourceConceptsParams_)
{
    cout << "constructing UnigramsOrigConceptSelector..." << endl;
}

indri::query::UnigramsOrigConceptSelector::~UnigramsOrigConceptSelector()
{
}

//vector< pair< string, vector<pair<string, string > > > > indri::query::UnigramsOrigConceptSelector::findConceptCands(
//    vector< pair<std::string, std::string> > queriesText, indri::query::QueryReformulator *, vector<string>, vector<pair<string, vector<pair<string, string> > > >  )
vector< pair< string, vector<pair<string, string > > > > indri::query::UnigramsOrigConceptSelector::findConceptCands(
        vector< pair<std::string, std::string> > queriesText,
        indri::query::QueryReformulator * ,
        vector<string> ,
        vector<pair<string, vector<string> > > ,
        vector<pair<string, vector<pair<string, string> > > >
        )
{
//	cout << "indri::query::UnigramsOrigConceptSelector::findConceptCands: size of queriesText = " << queriesText.size() << endl;
    vector< pair< string, vector<pair<string, string > > > > conceptCands;

    for(auto q: queriesText)
    {
        string qText = q.second;
        boost::trim(qText);
        conceptCands.push_back(make_pair(q.first, vector< pair<std::string, std::string> > {make_pair("GOOD", qText)} ));
    }
    cout << "indri::query::UnigramsOrigConceptSelector::findConceptCands: size of concept cands: " << conceptCands.size() << endl;
//    int counter = 0;
//    for (auto qc: conceptCands)
//    {
//        cout << "indri::query::UnigramsOrigConceptSelector::findConceptCands: concepts: " << counter++ << "- " << qc.first << " ";
//        for (auto c: qc.second)
//            cout << c.first << " " << c.second << endl;
//    }
    return conceptCands;
}

map<pair<string, string>, int > indri::query::UnigramsOrigConceptSelector::findOneDocConceptCands_(
    string , indri::api::QueryEnvironment & )
{
//	since it is a query-dependent selctor:
    return map<pair<string, string>, int>();
}
map<pair<string, string>, int> indri::query::UnigramsOrigConceptSelector::findOneBaseConcConceptCands_(
    pair<string, string>, indri::api::QueryEnvironment &)
{
//	since it is a query-dependent selctor:
    return map<pair<string, string>, int>();
}
