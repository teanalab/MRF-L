#ifndef UNIGRAMSORIGCONCEPTSELECTOR_HPP
#define UNIGRAMSORIGCONCEPTSELECTOR_HPP

#include "ConceptSelector.hpp"
//#include "QueryReformulator.hpp"

using namespace std;

namespace indri
{
namespace query
{


class UnigramsOrigConceptSelector : public ConceptSelector
{
public:

    UnigramsOrigConceptSelector(map<string, set<string> > wikiMedicalTermsExpr,  map<string, string> STYs, indri::api::Parameters& param_, OneResourceConceptsParams& oneResourceConceptsParams_ );
    virtual ~UnigramsOrigConceptSelector();

    vector< pair< string, vector<pair<string, string > > > > findConceptCands(
        vector< pair<std::string, std::string> > queriesText,
        indri::query::QueryReformulator * queryReformulator,
        vector<string> resourceNames_,
        vector<pair<string, vector<string> > > topDocsNames = vector<pair<string, vector<string> > >(),
        vector<pair<string, vector<pair<string, string> > > > baseResrcConcs = vector<pair<string, vector<pair<string, string> > > >()
        );

    map<pair<string, string>, int > findOneDocConceptCands_(
        string documentName, indri::api::QueryEnvironment & env);
    map<pair<string, string>, int> findOneBaseConcConceptCands_(
        pair<string, string> oneBaseRescConc, indri::api::QueryEnvironment & env);

protected:
private:
};

}
}
#endif // UNIGRAMSORIGCONCEPTSELECTOR_HPP
