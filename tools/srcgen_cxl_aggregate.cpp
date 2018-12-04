#include <filesystem>
#include <fstream>
#include <string>

template<typename File>
void
write_for_N(int N, File& file)
{
  file << "if constexpr (arity == ";
  file << N;
  file << ") {\n  auto&& ["; 
  std::string values;
  if (N >= 1) {
    values = "v1";
  }
  if (N > 1) {
    for (int i = 2; i <= N; ++i) {
      values += ", v" + std::to_string(i);
    }
  }
  file << values << "] = pod;\n  return make_tuple<T&&>(" << values << ");\n}\n";
}

int
main(int argc, char* argv[])
{
  std::filesystem::path path("../include/cxl/aggregate.generated.h");
  if (std::ofstream file(path); argc == 2 && file.good()) {
    int max = std::stoi(argv[1]);
    for (int n = 1; n <= max; ++n) {
      write_for_N(n, file);
    }
  }
}
