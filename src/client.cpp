#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#include <grpcpp/grpcpp.h>
#include "../proto/requestHandler.grpc.pb.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using requestHandler::Request;
using requestHandler::Responce;
using requestHandler::RequestHandler;

class RequestHandlerClient {
 public:
  RequestHandlerClient(std::shared_ptr<Channel> channel)
      : stub_(RequestHandler::NewStub(channel)) {}

  std::string HandleRequest(const std::string& data) {
    Request request;
    request.set_message(data);

    Responce reply;

    ClientContext context;

    Status status = stub_->HandleRequest(&context, request, &reply);

    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<RequestHandler::Stub> stub_;
};

int main(int argc, char** argv) {
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target=" << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  RequestHandlerClient rhc(grpc::CreateChannel(
      target_str, grpc::InsecureChannelCredentials()));

  int arraySize = 0;
  std::cout << "Input array size: ";
  std::cin >> arraySize;
  std::stringstream ss;
  for(int i = 0; i<arraySize; i++){
      int number;
      std::cout << "Array[" << i << "] = ";
      std::cin >> number;
      ss << number;
  }

  std::string request = ss.str();
  std::string response = rhc.HandleRequest(request);
  std::cout << "Server response: " << response << std::endl;

  return  0;
}