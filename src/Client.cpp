/*
 * Client.cpp
 *
 *  Created on: Mar 21, 2016
 *      Author: sten
 */
#include <iostream>
#include <chrono>

#include "open62541.h"
#include "Client.h"

using namespace std;

void Client::setNodeToRead(const unsigned int namespaceIndex, const std::string stringId){
    this->nodeToRead.identifierType = UA_NODEIDTYPE_STRING;
    this->nodeToRead.namespaceIndex = namespaceIndex;
    this->nodeToRead.identifier.string = UA_String_fromChars(stringId.c_str());
}

void Client::setNodeToRead(unsigned int namespaceIndex, int numericId){
    this->nodeToRead.identifierType = UA_NODEIDTYPE_NUMERIC;
    this->nodeToRead.namespaceIndex = namespaceIndex;
    this->nodeToRead.identifier.numeric = numericId;
}

size_t UA_calcSizeBinary(void *p, const UA_DataType *type);

std::pair<UA_StatusCode,double> Client::readOnce(){
    UA_ReadRequest rReq;
    UA_ReadRequest_init(&rReq);
    UA_StatusCode re = UA_STATUSCODE_GOOD;
    rReq.nodesToRead = UA_ReadValueId_new();
    rReq.nodesToReadSize = 1;
    UA_NodeId_copy(&this->nodeToRead, &rReq.nodesToRead[0].nodeId);
    rReq.nodesToRead[0].attributeId = UA_ATTRIBUTEID_VALUE;

    auto t1 = chrono::high_resolution_clock::now();
    UA_ReadResponse rResp = UA_Client_Service_read(this->raw_client, rReq);
    auto t2 = chrono::high_resolution_clock::now();
    re = rResp.responseHeader.serviceResult;
    if(re != UA_STATUSCODE_GOOD || rResp.resultsSize != 1 || !rResp.results[0].hasValue ||
            !UA_Variant_isScalar(&rResp.results[0].value) ||
            rResp.results[0].value.type != &UA_TYPES[UA_TYPES_DATETIME]){
        re = UA_STATUSCODE_BADINTERNALERROR;
    }

    UA_ReadRequest_deleteMembers(&rReq);
    UA_ReadResponse_deleteMembers(&rResp);
    return std::make_pair(re, std::chrono::duration<double, std::milli>(t2-t1).count());
}

Client::Client(const std::string &str) {
    //cout << "Object is being created" << endl;
    this->raw_client =  UA_Client_new(UA_ClientConfig_standard);
    UA_Client_connect(this->raw_client, str.c_str());
}

Client::~Client() {
    //cout << "Object is being deleted" << endl;
    if(this->raw_client)
        UA_Client_delete(this->raw_client);
    UA_NodeId_deleteMembers(&this->nodeToRead);
}

