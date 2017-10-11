#pragma once
#include <tuple>
#include <functional>
#include <string>

typedef std::tuple<std::string, std::string, std::function<void(std::string)>> Example;
