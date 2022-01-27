#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "../proto/helloworld.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

int getSubArrayLength(const std::vector<int>& values){
	std::vector<int> temp = values;
	bool sequence = true;
	std::vector<int> results;
	int sequenceValue = 0;

	for (int i = 0; i < values.size(); i++){
		if (values[i] == 0){
			temp = values;
			temp.erase(temp.begin() + i);
			for (const int& a : temp){
				sequence &= (bool)a;
				if (!sequence){
					results.push_back(sequenceValue);
					sequence = true;
					sequenceValue = 0;
					continue;
				}
				sequenceValue++;
			}
			results.push_back(sequenceValue);
			sequenceValue = 0;
		}
	}
	sort(results.begin(), results.end());

	return results.back();
}

class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    
    std::stringstream data(request->name());
    std::vector<int> parsedData;
    std::string strData = data.str();
    std::cout << std::endl << "Recieved data: " << strData;


   for(const char& c : strData){
      parsedData.push_back(c - '0');
   }


    std::string prefix("The length of the maximum array of units, which will be obtained by removing one number is - ");
	std::string answer = std::to_string(getSubArrayLength(parsedData));
	std::string responce = prefix + answer;
    reply->set_message(responce);
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}