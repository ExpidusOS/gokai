#include <gokai/flutter/method-call.h>

using namespace Gokai::Flutter;

MethodCall::MethodCall() {}
MethodCall::MethodCall(std::string method, std::any arguments) : method{method}, arguments{arguments} {}