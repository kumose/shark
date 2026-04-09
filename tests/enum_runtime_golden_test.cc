#include <fstream>
#include <iostream>
#include <string>

namespace {

bool contains(const std::string& text, const std::string& needle) {
  return text.find(needle) != std::string::npos;
}

std::string read_all(const std::string& path) {
  std::ifstream in(path);
  if (!in) return {};
  return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

}  // namespace

int main() {
  const std::string header = read_all(SHARK_ENUM_RUNTIME_HEADER);
  const std::string source = read_all(SHARK_ENUM_RUNTIME_SOURCE);
  if (header.empty() || source.empty()) {
    std::cerr << "generated runtime files are missing\n";
    return 1;
  }

  if (!contains(header, "inline Permission operator|(") ||
      !contains(header, "bool from_string(std::string_view name, Color* out);")) {
    std::cerr << "header golden checks failed\n";
    return 1;
  }

  if (!contains(source, "FindEnumTypeByName(\"shark.test.Color\")") ||
      !contains(source, "if (name == \"PERMISSION_READ\")")) {
    std::cerr << "source golden checks failed\n";
    return 1;
  }

  std::cout << "enum golden pass\n";
  return 0;
}
