#include <iostream>
#include <fstream>
#include <sstream>
#include "./generation.h"
#include "./parser.h"
#include "./tokenization.h"



int main(int argc, char* argv[]) {
   if (argc != 2) {
      std::cerr << "Incorrect Usage. Correct usage is..." << std::endl;
      std::cerr << "hydro <input.hy>" << std::endl;
      return EXIT_FAILURE;
   }


   std::string contents;
   {
      std::stringstream contents_stream;
      std::ifstream input(argv[1],std::ios::in);
      contents_stream << input.rdbuf();
      contents = contents_stream.str();
   }
   // No need to explicitly close the file, as fstream will close it when it goes out of scope (destructor)
   // std::cout <<contents<< std::endl;

   Tokenizer tokenizer(std::move(contents));
   std::vector<Token> things=tokenizer.tokenize();

   Parser parser(std::move(things));
   std::optional<NodeExit> tree = parser.parse();
   if(!tree.has_value()) {
      std::cerr << "Error parsing tree" << std::endl;
      exit(EXIT_FAILURE);
   }

   Generator generator(tree.value());
   {
      std::fstream file("out.asm",std::ios::out);
      file<<generator.generate();;
   }

   system("nasm -felf64 out.asm");
   system("ld -o out out.o");

   return EXIT_SUCCESS;
}