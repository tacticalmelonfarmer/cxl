#include <filesystem>
#include <fstream>
#include <string>

void destructure_for_N(int N, std::ofstream &file)
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

void make_struct_for_N(int N, std::ofstream &file)
{
  auto digit = std::to_string(N);
  file << "template<typename T>\nstruct make_base<T, " << digit << ">\n{\n  T v" << digit << ";\n};\n";
}

int main(int argc, char *argv[])
{
  if (argc == 4)
  {
    std::string generator_type(argv[1]), filename;
    void (*generator)(int, std::ofstream &);

    if (generator_type == "destructure")
    {
      generator = &destructure_for_N;
      filename = "aggregate.generated.0.h";
    }
    else if (generator_type == "make_struct")
    {
      generator = make_struct_for_N;
      filename = "aggregate.generated.1.h";
    }

    std::filesystem::path install_path(argv[2]);
    std::ofstream file(install_path / filename, std::ios::trunc);

    int max = std::stoi(argv[3]);
    for (int n = 1; n <= max; ++n)
    {
      (*generator)(n, file);
    }
  }
  else
  {
    std::ofstream("aggregate.generator.log", std::ios::app)
        << "[error]: wrong number of arguments\n"
           "[note]: usage: {"
        << argv[0] << "} <generator-type> <install-path> <generate-amount>\n";
    return 1;
  }
  return 0;
}
