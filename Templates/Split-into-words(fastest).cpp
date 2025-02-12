#include <string_view>


vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result;
    str.remove_prefix(std::min(str.find_first_not_of(" "), str.size()));
    
    while (!str.empty()) {
        auto space = str.find(" ");
        result.push_back(str.substr(0,space));
        str.remove_prefix(std::min(str.find_first_of(" "),str.size()));
        str.remove_prefix(std::min(str.find_first_not_of(" "),str.size()));
    }

    return result;
}
