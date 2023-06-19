#include <iostream>
#include <string_view>

namespace {
	constexpr std::string_view funni_thing[] { "▀▄─", "▄▀─", "▀─▄", "▄─▀" };
}

void print_thing() {
	for (int y{}, p{}; y != 6; ++y, p = ((p + 1) % 4)) {
		for (int x{}; x != 16; ++x) {
			std::cout << funni_thing[p];
		}
		std::cout << '\n';
	}
}
