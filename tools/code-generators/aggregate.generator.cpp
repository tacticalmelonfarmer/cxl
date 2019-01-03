#include <filesystem>
#include <fstream>
#include <string>

template <typename File>
void write_for_N(int N, File &file)
{
  file << "if constexpr (arity == " << N << ") {\n  auto&& [";
  std::string values;
  if (N >= 1)
  {
    values = "v1";
  }
  if (N > 1)
  {
    for (int i = 2; i <= N; ++i)
    {
      values += ", v" + std::to_string(i);
    }
  }
  file << values << "] = pod;\n  return make_tuple<T&&>(" << values << ");\n}\n";
}

int main(int argc, char *argv[])
{
  if (argc == 3)
  {
    std::filesystem::path install_path(argv[1]);
    std::ofstream file(install_path / "aggregate.generated.h", std::ios::trunc);
    int max = std::stoi(argv[2]);
    for (int n = 1; n <= max; ++n)
    {
      write_for_N(n, file);
    }
  }
  else
  {
    std::ofstream("srcgen_cxl_aggregate.log", std::ios::app)
        << "[error]: wrong number of arguments\n"
           "[note]: usage: {srcgen_cxl_aggregate} <install-path> <generate-amount>\n";
    return 1;
  }
  return 0;
}
