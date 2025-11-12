#include "repl.hpp"
#include "syntax.hpp"
#include "interpreter.hpp"

int main(const int argc, const char* const argv[])
{
	// auto command = new CommandOperation();
	// auto pipe = new PipeRedirectionOperation();
	// auto invocation1 = new InvocationOperation();
	// pipe->AppendChild(invocation1);
	// auto word1 = new WordOperation("cat");
	// invocation1->AppendChild(word1);
	// auto arg1 = new ArgumentOperation();
	// invocation1->AppendChild(arg1);
	// auto composition = new ConcatenationOperation();
	// arg1->AppendChild(composition);
	// auto environ = new EnvironmentVariableLoadOperation();
	// composition->AppendChild(environ);
	// auto word2 = new WordOperation("USERNAME");
	// environ->AppendChild(word2);
	// auto word3 = new WordOperation("-log.txt");
	// composition->AppendChild(word3);

	// auto file = new FileRedirectionOperation();
	// pipe->AppendChild(file);
	// auto invocation2 = new InvocationOperation();
	// file->AppendChild(invocation2);
	// auto word4 = new WordOperation("grep");
	// invocation2->AppendChild(word4);
	// auto arg2 = new ArgumentOperation();
	// invocation2->AppendChild(arg2);
	// auto arg3 = new ArgumentOperation();
	// invocation2->AppendChild(arg3);
	// auto word5 = new WordOperation("error");
	// arg2->AppendChild(word5);
	// auto raw_str = new RawStringLiteralOperation("critical warning");
	// arg3->AppendChild(raw_str);
	// auto arg4 = new ArgumentOperation();
	// file->AppendChild(arg4);
	// auto word6 = new WordOperation("errors.txt");
	// arg4->AppendChild(word6);

	auto pipe1 = new PipeRedirectionOperation(
		new InvocationOperation(
			new WordOperation("cat"),
			{
				new WordOperation("Makefile")
			}
		),
		
		new PipeRedirectionOperation(
			new InvocationOperation(
				new WordOperation("grep"),
				{
					new WordOperation("OBJECTS")
				}
			),

			new FileRedirectionOperation(
				new InvocationOperation(
					new WordOperation("grep"),
					{
						new WordOperation("TARGET")
					}
				),

				new ConcatenationOperation({
					new WordOperation("log-"), 
					new EnvironmentVariableLoadOperation(
						new WordOperation("USER")
					),
					new WordOperation(".txt") 
				})
			)
		)
	);

	auto exec = pipe1;

	FILE* fd = fopen("ast.dot", "w");
	exec->DumpTo(fd);
	fclose(fd);

	auto i = Interpreter();
	i.Execute(exec);

	// auto f = REPL();
	// f.Run();

	// return 0;
}
