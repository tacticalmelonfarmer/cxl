#include <fstream>
#include <string>

template<typename File>
void
write_for_N(int N, File& file)
{
  file << "if constexpr (arity == " << N << ") {\n  auto&& ["; 
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
  if (std::ofstream file("../include/cxl/aggregate.generated.h"); argc > 1) {
    int max = std::stoi(argv[1]);
    for (int n = 1; n <= max; ++n) {
      write_for_N(n, file);
    }
  }
}
