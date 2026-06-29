#include "Attribute.h"

#include <clang/AST/Decl.h>

namespace Spyll
{
	AttributeParser::AttributeParser(const clang::Decl* a_decl)
	{
		for (auto attr : a_decl->attrs())
		{
			auto annotation = clang::dyn_cast<clang::AnnotateAttr>(attr);
			if (!annotation)
			{
				continue;
			}

			auto lexer = clang::Lexer(
				annotation->getLocation(),
				a_decl->getASTContext().getLangOpts(),
				annotation->getAnnotation().data(),
				annotation->getAnnotation().data(),
				annotation->getAnnotation().data() + annotation->getAnnotationLength()
			);
		}
	}
}
