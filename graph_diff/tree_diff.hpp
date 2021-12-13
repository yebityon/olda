#include "../omni_graph/omni_graph.hpp"

namespace olda
{
    Graph diff(OmniGraph origin, OmniGraph target, std::map<std::string, std::string> & opt);
    Graph easy_diff(OmniGraph origin, OmniGraph target, std::map<std::string, std::string> & opt);
}