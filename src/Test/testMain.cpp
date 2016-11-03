#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <vector>

int main()
{
	std::string a = "a";
	std::string b = "b";
	std::string c = "c";

	std::vector<std::string> test({
		a,
		b,
		c});

	std::cout << test[0] << " " << test[1] << " " << test[2] << std::endl;

	std::cin >> a;
}