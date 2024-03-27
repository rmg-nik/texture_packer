#include "cli.hpp"
#include "gui.hpp"

#include <iostream>

int main(int argc, char** argv)
{
  try
  {
    if (argc > 1)
    {
      return TexturePackerApp::run_cli(argc, argv);
    }
    return TexturePackerApp::run_gui(argc, argv);
  }
  catch (const std::exception& ex)
  {
    std::cerr << "An error occurred: " << ex.what() << std::endl;
    return -1;
  }
}
