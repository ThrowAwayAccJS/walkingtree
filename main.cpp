#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Tokenizer.h"
#include "Parser.h"
#include "Interpreter.h"

const std::string language_extension = ".sloth";

using namespace parser;

std::optional<std::string> simple_pre_processor(std::string code)
{
	std::string include_start_string = "#include";

	if (code.rfind(include_start_string, 0) == 0)
	{
		code = code.substr(include_start_string.length(), code.length());
		code = code.substr(code.find_first_not_of(" "), code.length());

		std::ifstream file("" + code + language_extension);
		
		if (file.fail())
		{
			std::cerr << "#include expects a FILENAME" << std::endl;
			return std::nullopt;
		}

		std::string return_value;
		std::string line;

		while (std::getline(file, line))
		{
			return_value += line + "\n";
		}

		return return_value;
	}

	return std::nullopt;
}

int main(int argc, char** argv)
{
	try
	{
		std::ifstream fh("" + language_extension);

		std::string contents;
		std::string temp;

		while (getline(fh, temp))
		{
			if (simple_pre_processor(temp).has_value())
			{
				contents += simple_pre_processor(temp).value();
			}
			else
			{
				contents.append(temp);
			}

			contents.append(1, '\n');
		}

		fh.close();

		//std::cout << contents << std::endl;

		Tokenizer tokenizer;
		std::vector<Token> tokens = tokenizer.parse(contents);

/* 		for (Token token : tokens)
		{
			token.debug_print();
		} */

		std::cout << std::endl;

		Parser parser;
		auto functions = parser.parse(tokens);

		parser.debug_print();

		Interpreter intrepreter(functions);
		std::optional<std::shared_ptr<parser::VariableType>> return_val = intrepreter.Parse();

		std::cout << "\n";

		if (return_val.has_value())
		{
			std::cout << "Program exited with code " << return_val.value()->GetValue() << std::endl;
		}
		else 
		{
			std::cout << "Program exited with code 0" << std::endl; 
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		std::cin.get();
		return -1;
	}
	catch(...)
	{
		std::cerr << "Unknown error" << std::endl;
		std::cin.get();
		return -2;
	}

	std::cin.get();
}