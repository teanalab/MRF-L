#include "../include/UmlsOrigMetaMapConceptSelector.hpp"

indri::query::UmlsOrigMetaMapConceptSelector::UmlsOrigMetaMapConceptSelector( map<string, set<string> > wikiMedicalTermsExpr,  map<string, string> STYs,
        indri::api::Parameters& param_, OneResourceConceptsParams& umlsOrigMetaMap_ConceptsParams )
    :ConceptSelector(wikiMedicalTermsExpr, STYs, param_, umlsOrigMetaMap_ConceptsParams)
{
    cout << "constructing indri::query::UmlsOrigMetaMapConceptSelector::UmlsOrigMetaMapConceptSelector..." << endl;

    umlsOrigMetaMapConcepts = readMetamapConverted(oneResourceConceptsParams.conceptDocsDirectory);
}

indri::query::UmlsOrigMetaMapConceptSelector::~UmlsOrigMetaMapConceptSelector()
{
}

map<pair<string, string>, int > indri::query::UmlsOrigMetaMapConceptSelector::findOneDocConceptCands_(
    string , indri::api::QueryEnvironment & )
{
    return map<pair<string, string>, int>();
}

map<pair<string, string>, int> indri::query::UmlsOrigMetaMapConceptSelector::findOneBaseConcConceptCands_(
    pair<string, string> , indri::api::QueryEnvironment & )
{
    return map<pair<string, string>, int>();
}

vector< pair< string, vector<pair<string, string > > > > indri::query::UmlsOrigMetaMapConceptSelector::findConceptCands(
        vector< pair<std::string, std::string> > ,
        indri::query::QueryReformulator * ,
        vector<string> ,
        vector<pair<string, vector<string> > > ,
        vector<pair<string, vector<pair<string, string> > > >
        )
{
    cout << "indri::query::UmlsOrigMetaMapConceptSelector::findConceptCands: " << endl;

    vector<pair<string, vector<pair<string, string> > > > umlsOrigMetaMapConcepts_;

    for( auto quC: umlsOrigMetaMapConcepts)
    {
        set<pair<string, string> > tmpS;
        for( auto c: quC.second )
        {
            tmpS.insert(make_pair(get<2>(c), get<1>(c)));
        }
        vector< pair<string,string> > tmp(tmpS.begin(), tmpS.end());
        umlsOrigMetaMapConcepts_.push_back(make_pair(quC.first, tmp));
    }
    /*
    for (auto qc: umlsOrigMetaMapConcepts_)
        for (auto c: qc.second)
            cout << "indri::query::UmlsOrigMetaMapConceptSelector::findConceptCands: q, STY, STR: " << qc.first << ", " << c.first << ", " << c.second << endl;
    */
    cout << "indri::query::UmlsOrigMetaMapConceptSelector::findConceptCands: size of umlsOrigMetaMapConcepts_ = " << umlsOrigMetaMapConcepts_.size() << endl;
    return umlsOrigMetaMapConcepts_;
}


vector<pair<string, vector<tuple<string, string, string> > > > indri::query::UmlsOrigMetaMapConceptSelector::readMetamapConverted(string fileName)
{
    cout << "reading converted-Metamap file: " + fileName << endl;
    vector<pair<string, vector<tuple<string, string, string> > > >  metamapConverted;
    ifstream inFile(fileName.c_str());
    if (!inFile)
    {
        throw runtime_error( "Error in reading file: " + fileName );
    }
    string line;
    while (getline(inFile, line))
    {
//        cout << line << endl;
        stringstream line_ss(line);
        string qId;
        getline(line_ss,qId,';');
        string tuples_;
        vector<tuple<string, string, string> > temp;
        while (getline(line_ss, tuples_, ';'))
        {
            stringstream tuples_ss(tuples_);
            string CUI, STR, STY;
            getline(tuples_ss,CUI,':');
            getline(tuples_ss,STR,':');
            getline(tuples_ss,STY,':');
            temp.push_back(make_tuple(CUI, STR, STY));

            if (strCuiMap_r.find(STR) != strCuiMap_r.end())
            {
                if(CUI != strCuiMap_r[STR])
                {
                    cout << "Warning: indri::query::UmlsPrfMetaMapConceptSelector::runReadMetamapApi: STR, CUI " + STR + ", " + CUI + " is in conflict with STR, CUI: " + STR +", "+strCuiMap_r[STR]<< endl;
                }
            }
            strCuiMap[CUI] = STR;
            strCuiMap_r[STR] = CUI;
//            cout << "indri::query::UmlsPrfMetaMapConceptSelector::runReadMetamapApi: q, CUI, STR: " << qId << ", " << CUI << ", " << STR << endl;

        }
        metamapConverted.push_back(make_pair(qId, temp));
    }
   /* 
    for(auto m: metamapConverted)
    {
        cout << "indri::query::UmlsOrigMetaMapConceptSelector::readMetamapConverted: " << m.first << " ";
        for (auto c: m.second)
        cout << get<0>(c) << " " << get<1>(c) << " " << get<2>(c) << endl;
    }
   */ 
    cout << "indri::query::UmlsOrigMetaMapConceptSelector::readMetamapConverted: size of metamapConverted = " << metamapConverted.size() << endl;
    return metamapConverted;
}
