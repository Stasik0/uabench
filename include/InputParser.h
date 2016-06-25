/*
 * InputParser.h
 *
 *  Created on: Jun 24, 2016
 *      Author: sten
 */

#ifndef INPUTPARSER_H_
#define INPUTPARSER_H_

#include <string>
#include <vector>
#include <algorithm>

class InputParser {
public:
    InputParser(int &argc, char **argv);
    const std::string& getCmdOption(const std::string &option);
    bool cmdOptionExists(const std::string &option);
    const std::vector<std::string>& getOrderedParameters();

private:
    std::vector<std::string> tokens;
    std::vector<std::string> orderedParameters; //prefixed commands are not included here
};

#endif /* INPUTPARSER_H_ */
