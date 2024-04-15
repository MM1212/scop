#include <App.h>
#include <stdexcept>
#include <iostream>

int main(int ac, char**av) {
  ac--;
  av++;
  Scop::App app{};

  try {
    std::vector<std::string_view> modelPaths;
    if (ac == 0)
      modelPaths.push_back("assets/models/colored_cube.obj");
    for (int i = 0; i < ac; i++)
      modelPaths.push_back(av[i]);
    app.loadEntities(modelPaths);
    app.run();
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}