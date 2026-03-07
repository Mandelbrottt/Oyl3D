//===- PrintFunctionNames.cpp ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Example clang plugin which simply prints the names of all the top-level decls
// in the input file.
//
//===----------------------------------------------------------------------===//

#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/AST/AST.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Sema/Sema.h>
#include <llvm/Support/raw_ostream.h>

#include <vector>
#include <string>

using namespace clang;

namespace
{
	class SpyllConsumer : public ASTConsumer
	{
		CompilerInstance& m_instance;

	public:
		explicit SpyllConsumer(CompilerInstance& Instance)
			: m_instance(Instance) {}

		bool HandleTopLevelDecl(DeclGroupRef DG) override
		{
			for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i)
			{
				const Decl* D = *i;
				if (const NamedDecl* ND = dyn_cast<NamedDecl>(D))
					llvm::errs() << "top-level-decl: \"" << ND->getNameAsString() << "\"\n";
			}

			return true;
		}
	};
	
	class SpyllPluginAction : public PluginASTAction
	{
	protected:
		std::unique_ptr<ASTConsumer>
		CreateASTConsumer(CompilerInstance& CI, StringRef InFile) override
		{
			return std::make_unique<SpyllConsumer>(CI);
		}

		bool ParseArgs(
			const CompilerInstance&         CI,
			const std::vector<std::string>& args
		) override
		{
			if (!args.empty() && args[0] == "help")
				PrintHelp(llvm::errs());

			return true;
		}

		void PrintHelp(llvm::raw_ostream& ros)
		{
			ros << "Help for Spyll plugin goes here\n";
		}
	};
}

static FrontendPluginRegistry::Add<SpyllPluginAction> X("spyll", "Generate and Emit Oyl Reflection Information");
