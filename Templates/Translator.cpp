#include <vector>
#include <map>
#include <string_view>
#include <iostream>

using namespace std;

class Translator {
public:
    Translator() = default;
    
    void Add(string_view source, string_view target);
    string_view TranslateForward(string_view source) const;
    string_view TranslateBackward(string_view target) const;

private:
    vector<string> find_word;
    map<string,string> word_forward;
    map<string,string> word_backward;
};

void Translator::Add(string_view source, string_view target){
    word_forward[static_cast<string>(source)] = target;
    word_backward[static_cast<string>(target)] = source;
}

string_view Translator::TranslateForward(string_view source) const{
    if(word_forward.count(static_cast<string>(source))){
        return word_forward.at(static_cast<string>(source));
    }
    return {};
}


string_view Translator::TranslateBackward(string_view target) const{
    if(word_backward.count(static_cast<string>(target))){
        return word_backward.at(static_cast<string>(target));
    }
    return {};
}

int main() {
  
}
