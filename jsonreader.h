// Reading a message JSON with Reader (SAX-style API).
// The JSON should be an object with key-string pairs.

#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;
using namespace rapidjson;

typedef map<string, string> MessageMap;

RAPIDJSON_DIAG_PUSH
RAPIDJSON_DIAG_OFF(effc++)

struct MessageHandler
    : public BaseReaderHandler<UTF8<>, MessageHandler> {
    MessageHandler() : messages_(), state_(kExpectObjectStart), name_() {}

    bool StartObject() {
        switch (state_) {
        case kExpectObjectStart:
            state_ = kExpectNameOrObjectEnd;
            return true;
        default:
            return false;
        }
    }

    bool String(const char* str, SizeType length, bool) {
        switch (state_) {
        case kExpectNameOrObjectEnd:
            name_ = string(str, length);
            state_ = kExpectValue;
            return true;
        case kExpectValue:
            messages_.insert(MessageMap::value_type(name_, string(str, length)));
            state_ = kExpectNameOrObjectEnd;
            return true;
        default:
            return false;
        }
    }

    bool EndObject(SizeType) { return state_ == kExpectNameOrObjectEnd; }

    bool Default() { return false; } // All other events are invalid.

    MessageMap messages_;
    enum State {
        kExpectObjectStart,
        kExpectNameOrObjectEnd,
        kExpectValue
    }state_;
    std::string name_;
};

RAPIDJSON_DIAG_POP

static void ParseMessages(const char* json, MessageMap& messages) {
    Reader reader;
    MessageHandler handler;
    StringStream ss(json);
    if (reader.Parse(ss, handler))
        messages.swap(handler.messages_);   // Only change it if success.
    else {
        ParseErrorCode e = reader.GetParseErrorCode();
        size_t o = reader.GetErrorOffset();
        cout << "Error: " << GetParseError_En(e) << endl;;
        cout << " at offset " << o << " near '" << string(json).substr(o, 10) << "...'" << endl;
    }
}

