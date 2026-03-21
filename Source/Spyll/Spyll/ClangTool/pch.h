#pragma once

#pragma warning(push)
#pragma warning(disable : 4702) // Unreachable code in MSVC
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#pragma warning(pop)

#include <memory>
#include <string>
#include <vector>

#include <unordered_map>
#include <unordered_set>

#include "Descriptors/DescriptorId.h"
