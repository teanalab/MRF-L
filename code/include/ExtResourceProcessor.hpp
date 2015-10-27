//
// ExtResourceProcessor
//
// 6 Jul 2015 -- sbk
//

#ifndef EXTRESOURCEPROCESSOR_HPP
#define EXTRESOURCEPROCESSOR_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "indri/Parameters.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/UtilityThread.hpp"
//#include "../include/lce.hpp"


#include <time.h>
#include <algorithm>
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

using namespace std;


namespace indri
{
namespace query
{

class ExtResourceProcessor
{
public:
    ExtResourceProcessor(indri::api::Parameters& param);
    ~ExtResourceProcessor() {};

    map<string, vector< pair<string, int> > > read_MRREL_CUI1_CUI2();
    vector<pair<string, vector<tuple<string, string, string> > > > readMetamapConverted(string fileName);
    pair<map<string, string>, map<string, string> > readSTYs();
    map<string, set<string> > readWikiMedicalEntities();
    std::map<std::string, std::set<std::string> > readMRSTY_simp();
//    vector<string> readGoodSTYs();
    map<string, string> readMRCONSO_simp();
//    map<string, vector< pair<string, int> > > get_MRREL_CUI1_CUI2() {return MRREL_CUI1_CUI2;};

private:
//    map<string, vector< pair<string, int> > > MRREL_CUI1_CUI2;
    indri::api::Parameters& _param;

private:

};
}
}

#endif // EXTRESOURCEPROCESSOR
