# restServerCmndExecutor
A small command executor behind a rest server

#compilation command
g++ -std=c++14 -I restbed/distribution/include/ -L restbed/distribution/library/ -o webserver simpleWebServer.cc -l restbed

#Send the command to the server 
curl -vX POST http://127.0.0.1:1984/command --header "Content-Type: application/json" -d@test.json
