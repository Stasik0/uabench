/*
 * Client.h
 *
 *  Created on: Mar 21, 2016
 *      Author: sten
 */

#include "open62541.h"

#ifndef CLIENT_H_
#define CLIENT_H_

class Client {
public:
    Client(const std::string &str);
    virtual ~Client();
    void setNodeToRead(unsigned int namespaceIndex, std::string stringId);
    void setNodeToRead(unsigned int namespaceIndex, int numericId);
    std::pair<UA_StatusCode,double> readOnce();
private:
    UA_Client* raw_client;
    UA_NodeId nodeToRead;
};

#endif /* CLIENT_H_ */
