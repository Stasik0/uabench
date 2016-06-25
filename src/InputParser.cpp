/*
 * InputParser.cpp
 *
 *  Code ist mostly from: http://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
 */

#include "InputParser.h"

InputParser::InputParser(int &argc, char **argv) {
    for (int i=1; i < argc; ++i)
        this->tokens.push_back(std::string(argv[i]));

    for (int i=1; i < argc; ++i){
        if(std::string(argv[i]).at(0) == '-' && i!=argc-1){ //skip two tokens if the first token starts with a '-'
            ++i;
            continue;
        }
        this->orderedParameters.push_back(std::string(argv[i]));
    }
}

/// @author iain
const std::string& InputParser::getCmdOption(const std::string &option) {
    static const std::string empty = "";
    std::vector<std::string>::const_iterator itr;
    itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
    if (itr != this->tokens.end() && ++itr != this->tokens.end()){
        return *itr;
    }
    return empty;
}
/// @author iain
bool InputParser::cmdOptionExists(const std::string &option) {
    return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
}

const std::vector<std::string>& InputParser::getOrderedParameters(){
    return this->orderedParameters;
}
