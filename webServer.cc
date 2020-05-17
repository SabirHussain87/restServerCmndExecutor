#include <memory>
#include <thread>
#include <cstdlib>
#include <restbed>
#include <sstream>
#include <iostream>
#include "jsonreader.h"
#include "threadpool.h"
#include <cstdio>

using namespace std;
using namespace restbed;

static void ParseMessages(const char* json, MessageMap& messages);
threadpool *thPool;
void post_method_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    size_t content_length = request->get_header( "Content-Length", 0 );

    session->fetch( content_length, [ request ]( const shared_ptr< Session > session, const Bytes & body )
    {
        //fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );
        //cout << body.data() << endl;
        int id=0;
        char ret[100];
        int size=0;
        MessageMap messages;
        const char* json = reinterpret_cast<const char *>(body.data());
        ParseMessages(json, messages);
        for (MessageMap::const_iterator itr = messages.begin(); itr != messages.end(); ++itr){
            cout << itr->first << ": " << itr->second << endl;
            id = thPool->putWork(itr->second);
        }
        sprintf( ret, "%s%d%s", "Command Received,ID=", id, "\n" ); 
        if(id<10) size=22;
        else if(id>99) size=24;
        else size=23;
        std::string s = std::to_string(size);
        //cout << ret << endl;
        session->close( OK, ret, { { "Content-Length", s.c_str() }, { "Connection", "close" } } );
    } );
}

void get_method_handler( const shared_ptr< Session > session )
{
    stringstream id;
    id << ::this_thread::get_id( );
    const auto request = session->get_request( );
    int reqId = request->get_query_parameter( "id", 0 );
    cout << "GET request ID recevied:" << reqId << endl;
    string body;
    if(reqId == 0) //Send overal status of all the commands
    {
        body = "Overal status:\n";
        body += thPool->getStatus();
    } else if( reqId > 0 && reqId <= 500) {
        body = thPool->getOutput(reqId);
    } else {
        body = "Wrong command send, please send either command id(1-500) or without id";
    }
    session->close( OK, body, { { "Content-Length", ::to_string( body.length( ) ) } } );
}

int main(int argc, char** argv)
{
    if(argc != 3) {
		cout << "Usage: webServer <port[1000-65535]> <numberOfThreads[1-20]>" << endl;
		cout << "Example: ./webServe 50001 10" << endl;
		exit(EXIT_FAILURE);
	} 
    int serverPort = atoi(argv[1]);
    int maximumThreads = atoi(argv[2]);
    if( serverPort < 1000 || serverPort > 65535 || maximumThreads < 1 || maximumThreads > 20 ) {
        cout << "Argument Values provided are not correct" << endl;
		cout << "Usage: webServer <port[1000-65535]> <numberOfThreads[1-20]>" << endl;
		cout << "Example: ./webServe 50001 10" << endl;
		exit(EXIT_FAILURE);
	}	
    MessageMap messages;
    const char* json = "{ \"greeting\" : \"Hello!\", \"farewell\" : \"bye-bye!\" }";
    ParseMessages(json, messages);
    thPool = new threadpool(maximumThreads);
    /*************Web server Methods**************************/
    auto resource = make_shared< Resource >( );
    resource->set_path( "/status" );
    resource->set_method_handler( "GET", get_method_handler );
    auto resource1 = make_shared< Resource >( );
    resource1->set_path( "/command" );
    resource1->set_method_handler( "POST", post_method_handler );
 
    
    auto settings = make_shared< Settings >( );
    settings->set_port( serverPort );
    settings->set_worker_limit(10 );
    settings->set_default_header( "Connection", "close" );
    
    Service service;
    service.publish( resource );
    service.publish( resource1 );
    service.start( settings );
    
    return EXIT_SUCCESS;
}
