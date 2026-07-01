#include "Emit.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include <Spyll/Tool/Core/ReflectionParser.h>

#include "SpyllTool.h"

std::string&
FindAndReplace(std::string& a_str, std::string_view a_find, std::string_view a_replace)
{
	size_t start_pos = 0;
	while ((start_pos = a_str.find(a_find, start_pos)) != std::string::npos)
	{
		a_str.replace(start_pos, a_find.length(), a_replace);
		start_pos += a_replace.length(); // Handles case where 'replace' is a substring of 'find'
	}
	return a_str;
};

void
AddIndentToStringStream(std::stringstream& a_stream, std::string_view a_indent = "\t")
{
	a_stream.seekg(0, std::ios::end);
	auto size = a_stream.tellg();
	a_stream.seekg(0);

	if (size == 0)
	{
		return;
	}

	std::stringstream indentStream;
	while (!a_stream.eof())
	{
		char buf[256];
		a_stream.getline(buf, std::size(buf));
		indentStream << a_indent << buf;
		if (a_stream.peek(), !a_stream.eof())
		{
			indentStream << "\n";
		}
		std::memset(buf, 0, std::size(buf));
	}
	a_stream.swap(indentStream);
	a_stream.seekp(0, std::ios::end);
}

std::string
GetTypeNameAsVar(std::string_view a_name)
{
	std::string typeVar = std::string(a_name);
	std::replace(typeVar.begin(), typeVar.end(), ' ', '_');
	std::replace(typeVar.begin(), typeVar.end(), '-', '_');
	std::replace(typeVar.begin(), typeVar.end(), '.', '_');
	std::replace(typeVar.begin(), typeVar.end(), '<', '_');
	std::replace(typeVar.begin(), typeVar.end(), '>', '_');
	std::replace(typeVar.begin(), typeVar.end(), ',', '_');
	FindAndReplace(typeVar, "::", "_");
	FindAndReplace(typeVar, "*", "ptr_");

	return typeVar;
}

std::string
GetStowTypeName(std::string_view a_name)
{
	return "__Stow_" + GetTypeNameAsVar(a_name);
}

std::string
GetStowCall(std::string_view a_typeName)
{
	std::stringstream call;
	call << "(Oyl::Reflection::Internal::Stowed<" << GetStowTypeName(a_typeName) << ">::value)";
	return call.str();
}

constexpr
std::string_view
GetTrimmedStringView(std::string_view a_view)
{
	size_t start = a_view.find_first_not_of(" \n\t");
	size_t end = a_view.find_last_not_of(" \n\t");
	return a_view.substr(start, end - start + 1);
}

void
EmitHeaderComment(std::string& a_emitString);

void
EmitIncludes(std::string& a_emitString, const Spyll::ReflectionParser* a_parser);

void
EmitDependencies(std::string& a_emitString, const std::vector<std::string_view>& a_tool);

void
PushIndent(std::string& a_indent);

void
PopIndent(std::string& a_indent);

void
EmitReflectionRegister(std::string& a_emitString, const Spyll::ReflectionParser* a_parser);

void
EmitStowDeclarations(std::string& a_emitString, const Spyll::ReflectionParser* a_parser);

void
RegisterTypes(std::stringstream& a_stream, const Spyll::ReflectionParser* a_parser);

void
RegisterVariablesForType(
	std::stringstream& a_stream,
	const Spyll::Type* a_parentType
);

void
RegisterFieldsForType(
	std::stringstream& a_stream,
	const Spyll::Type* a_parentType
);

void
RegisterFunctionsForType(
	std::stringstream& a_stream,
	const Spyll::Type* a_parentType
);

void
RegisterMethodsForType(
	std::stringstream& a_stream,
	const Spyll::Type* a_parentType
);

void
RegisterArgumentForFunction(
	std::stringstream& a_stream,
	const Spyll::Function* a_function
);

void
RegisterGlobalVariables(
	std::stringstream& a_stream,
	const Spyll::ReflectionParser* a_parser
);

void
RegisterGlobalFunctions(
	std::stringstream& a_stream,
	const Spyll::ReflectionParser* a_parser
);

void
RegisterEnums(
	std::stringstream& a_stream,
	const Spyll::ReflectionParser* a_parser
);

constexpr
static
std::string_view g_emitTemplate = GetTrimmedStringView(R"""(
{HEADER_COMMENT}

#include <Core/Reflection/ReflectionFactory.h>

{REFLECT_INCLUDES}

{REFLECT_STOW}

extern "C"
_OYL_EXPORT
void
_ReflectionAssembly_Dependencies(int* a_count, const char** a_dependencies)
{
	*a_count = {DEPENDENCY_COUNT};

{DEPENDENCIES}
	*a_dependencies = *dependencies;
}

extern "C"
_OYL_EXPORT
::Oyl::Reflection::Assembly*
_ReflectionAssembly_Register(::Oyl::Reflection::Internal::ReflectionAllocatorFn a_allocate)
{
	Oyl::Reflection::Internal::AssemblyParams AssemblyParams;
	AssemblyParams.name = "{ASSEMBLY_NAME}";
	Oyl::Reflection::Assembly* AssemblyPtr = Oyl::Reflection::Internal::ReflectionFactory::CreateAssembly(AssemblyParams, a_allocate);

{REFLECTION_REGISTER}

	return AssemblyPtr;
}
)""");

void
EmitCodeFromTool(const Spyll::ReflectionParser* a_parser)
{
	std::filesystem::create_directory("Generated");

	std::ofstream generatedIncludeFile;
	generatedIncludeFile.open("Generated/Spyll.generated.cpp");

	std::string emitString = std::string(g_emitTemplate);

	EmitHeaderComment(emitString);
	EmitIncludes(emitString, a_parser);
	EmitDependencies(emitString, g_tool.dependencies);
	EmitStowDeclarations(emitString, a_parser);
	EmitReflectionRegister(emitString, a_parser);

	FindAndReplace(emitString, "{ASSEMBLY_NAME}", g_tool.assemblyName);

	generatedIncludeFile << emitString;
}

void
EmitHeaderComment(std::string& a_emitString)
{
	std::string_view result =
		"// Auto-Generated Source File, Generated by Oyl.Spyll\n"
		"// {GENERATION_DATE}\n"
		"// DO NOT MODIFY!\n";

	FindAndReplace(a_emitString, "{HEADER_COMMENT}", result);
}

void
EmitIncludes(std::string& a_emitString, const Spyll::ReflectionParser* a_parser)
{
	std::unordered_set<std::string_view> includeSet;

	for (const Spyll::Type* type : a_parser->GetTypes())
	{
		includeSet.insert(type->GetSourceFile());
	}

	for (const Spyll::Variable* variable : a_parser->GetGlobalVariables())
	{
		includeSet.insert(variable->GetSourceFile());
	}

	for (const Spyll::Function* function : a_parser->GetGlobalFunctions())
	{
		includeSet.insert(function->GetSourceFile());
	}

	for (const Spyll::Enum* enum_ : a_parser->GetEnums())
	{
		includeSet.insert(enum_->GetSourceFile());
	}

	std::stringstream includes;
	for (const auto& include : includeSet)
	{
		includes << "#include <" << include << ">\n";
	}
	FindAndReplace(a_emitString, "{REFLECT_INCLUDES}", includes.str());
}

void
EmitDependencies(std::string& a_emitString, const std::vector<std::string_view>& a_dependencies)
{
	std::stringstream dependenciesArr;
	if (a_dependencies.empty())
	{
		dependenciesArr << "static const char** dependencies = nullptr";
	} else
	{
		dependenciesArr << "static const char* dependencies[] = { ";
		for (const auto& dependency : a_dependencies)
		{
			dependenciesArr << dependency << ",";
		}
		dependenciesArr << " }";
	}
	dependenciesArr << ";\n";

	AddIndentToStringStream(dependenciesArr);
	FindAndReplace(a_emitString, "{DEPENDENCIES}", dependenciesArr.str());
	FindAndReplace(a_emitString, "{DEPENDENCY_COUNT}", std::to_string(a_dependencies.size()));
}

void
PushIndent(std::string& a_indent)
{
	a_indent += "\t";
}

void
PopIndent(std::string& a_indent)
{
	a_indent = a_indent.substr(0, a_indent.size() - 1);
}

void
EmitReflectionRegister(std::string& a_emitString, const Spyll::ReflectionParser* a_parser)
{
	std::stringstream stream;
	stream.setf(std::ios::boolalpha);

	RegisterTypes(stream, a_parser);
	RegisterEnums(stream, a_parser);

	// Register type children after all types registered to guarantee calls to Type::Get<T>() succeed
	for (auto type : a_parser->GetTypes())
	{
		RegisterVariablesForType(stream, type);
		RegisterFieldsForType(stream, type);
		RegisterFunctionsForType(stream, type);
		RegisterMethodsForType(stream, type);
	}

	RegisterGlobalVariables(stream, a_parser);
	RegisterGlobalFunctions(stream, a_parser);

	AddIndentToStringStream(stream);
	FindAndReplace(a_emitString, "{REFLECTION_REGISTER}", stream.str());
}

void
EmitStowDeclarations(std::string& a_emitString, const Spyll::ReflectionParser* a_parser)
{
	std::stringstream stream;
	stream.setf(std::ios::boolalpha);

	auto isPublic = [](int a_accessSpec)
	{
		return a_accessSpec == 0;
	};

	for (const auto* type : a_parser->GetTypes())
	{
		for (const auto& variable : type->GetVariables())
		{
			if (isPublic(variable.GetAccessSpecifier()))
				continue;

			auto stowTypeNameAsVar = GetStowTypeName(variable.GetQualifiedName());

			stream << "struct " << stowTypeNameAsVar << " { using type = ";
			stream << variable.GetTypeAsString() << "; };\n";
			stream << "template struct Oyl::Reflection::Internal::StowPrivate<" << stowTypeNameAsVar << ", &" << variable.GetQualifiedName() << ">;\n";
		}

		for (const auto& field : type->GetFields())
		{
			if (isPublic(field.GetAccessSpecifier()))
				continue;

			auto typeNameAsVar = GetStowTypeName(field.GetQualifiedName());

			stream << "struct " << typeNameAsVar << " { using type = ";
			stream << field.GetTypeAsString() << "(" << type->GetQualifiedName() << "::*); };\n";
			stream << "template struct Oyl::Reflection::Internal::StowPrivate<" << typeNameAsVar << ", &" << field.GetQualifiedName() << ">;\n";
		}

		for (const auto& function : type->GetFunctions())
		{
			if (isPublic(function.GetAccessSpecifier()))
				continue;

			auto typeNameAsVar = GetStowTypeName(function.GetQualifiedName());

			stream << "struct " << typeNameAsVar << " { using type = ";
			stream << function.GetReturnTypeAsString() << "(*)(";
			const auto& args = function.GetArguments();
			for (size_t i = 0; i < args.size(); i++)
			{
				const auto& arg = args[i];
				stream << arg.type;
				if (i != args.size() - 1)
				{
					stream << ", ";
				}
			}
			stream << "); };\n";
			stream << "template struct Oyl::Reflection::Internal::StowPrivate<" << typeNameAsVar << ", &" << function.GetQualifiedName() << ">;\n";
		}

		for (const auto& method : type->GetMethods())
		{
			if (isPublic(method.GetAccessSpecifier()))
				continue;

			auto typeNameAsVar = GetStowTypeName(method.GetQualifiedName());

			stream << "struct " << typeNameAsVar << " { using type = ";
			stream << method.GetReturnTypeAsString() << "(" << type->GetQualifiedName() << "::*)(";
			const auto& args = method.GetArguments();
			for (size_t i = 0; i < args.size(); i++)
			{
				const auto& arg = args[i];
				stream << arg.type;
				if (i != args.size() - 1)
				{
					stream << ", ";
				}
			}
			stream << ")";
			if (method.IsConst())
			{
				stream << " const";
			}
			stream << "; };\n";
			stream << "template struct Oyl::Reflection::Internal::StowPrivate<" << typeNameAsVar << ", &" << method.GetQualifiedName() << ">;\n";
		}
	}

	FindAndReplace(a_emitString, "{REFLECT_STOW}", stream.str());
}

void
RegisterTypes(std::stringstream& a_stream, const Spyll::ReflectionParser* a_parser)
{
	for (const auto& type : a_parser->GetTypes())
	{
		if (a_stream.tellp() != 0)
		{
			a_stream << "\n";
		}

		std::string typeVar = GetTypeNameAsVar(type->GetQualifiedName());

		std::string emitString = std::string(
			GetTrimmedStringView(R"""(
Oyl::Reflection::Type* {TYPENAME_AS_VAR}; (void) {TYPENAME_AS_VAR};
{
	Oyl::Reflection::Internal::TypeParams TypeParams;
	TypeParams.assembly = {ASSEMBLY_PTR};
	TypeParams.typeId = Oyl::Reflection::GetTypeId<{QUALIFIED_TYPENAME}>();
	TypeParams.qualifiedName = "{QUALIFIED_TYPENAME}";
	TypeParams.name = "{TYPENAME}";
	TypeParams.size = {SIZE};
	TypeParams.alignment = {ALIGNMENT};
	{TYPENAME_AS_VAR} = Oyl::Reflection::Internal::ReflectionFactory::AddTypeToAssembly({ASSEMBLY_PTR}, TypeParams, a_allocate);
}
)"""
			));

		FindAndReplace(emitString, "{ASSEMBLY_PTR}", "AssemblyPtr");
		FindAndReplace(emitString, "{QUALIFIED_TYPENAME}", type->GetQualifiedName());
		FindAndReplace(emitString, "{TYPENAME}", type->GetName());
		FindAndReplace(emitString, "{SIZE}", std::to_string(type->GetSize()));
		FindAndReplace(emitString, "{ALIGNMENT}", std::to_string(type->GetAlignment()));
		FindAndReplace(emitString, "{TYPENAME_AS_VAR}", GetTypeNameAsVar(type->GetQualifiedName()));

		a_stream << emitString;
	}
}

void
RegisterVariablesForType(
	std::stringstream& a_stream,
	const Spyll::Type* a_parentType
)
{
	for (const auto& variable : a_parentType->GetVariables())
	{
		std::string emitString = GetTrimmedStringView(R"""(
{
	Oyl::Reflection::Internal::VariableParams VariableParams;
	VariableParams.qualifiedName = "{QUALIFIED_NAME}";
	VariableParams.name = "{NAME}";
	VariableParams.type = Oyl::Reflection::Type::Get<{QUALIFIED_TYPE}>();
	VariableParams.parentType = {PARENT_TYPENAME_AS_VAR};
	VariableParams.accessSpecifier = static_cast<Oyl::Reflection::AccessSpecifier>({ACCESS_SPECIFIER});
	Oyl::Reflection::Internal::ReflectionFactory::AddVariableToType({PARENT_TYPENAME_AS_VAR}, VariableParams, a_allocate);
}
)"""
		).data();

		FindAndReplace(emitString, "{QUALIFIED_NAME}", variable.GetQualifiedName());
		FindAndReplace(emitString, "{NAME}", variable.GetName());
		FindAndReplace(emitString, "{QUALIFIED_TYPE}", variable.GetTypeAsString());
		FindAndReplace(emitString, "{ACCESS_SPECIFIER}", std::to_string(variable.GetAccessSpecifier()));
		FindAndReplace(emitString, "{PARENT_TYPENAME_AS_VAR}", GetTypeNameAsVar(a_parentType->GetQualifiedName()));

		a_stream << emitString;
	}
}

void
RegisterFieldsForType(
	std::stringstream& a_stream,
	const Spyll::Type* a_parentType
)
{
	for (const auto& field : a_parentType->GetFields())
	{
		std::string emitString = GetTrimmedStringView(R"""(
{
	Oyl::Reflection::Internal::FieldParams FieldParams;
	FieldParams.qualifiedName = "{QUALIFIED_NAME}";
	FieldParams.name = "{NAME}";
	FieldParams.type = Oyl::Reflection::Type::Get<{QUALIFIED_TYPE}>();
	FieldParams.parentType = {PARENT_TYPENAME_AS_VAR};
	FieldParams.offsetInBits = {OFFSET_IN_BITS};
	FieldParams.accessSpecifier = static_cast<Oyl::Reflection::AccessSpecifier>({ACCESS_SPECIFIER});
	FieldParams.isConst = {IS_CONST};
	{FIELD_PTR}Oyl::Reflection::Internal::ReflectionFactory::AddFieldToType({PARENT_TYPENAME_AS_VAR}, FieldParams, a_allocate);
{ATTRIBUTES}
}
)"""
		).data();

		FindAndReplace(emitString, "{QUALIFIED_NAME}", field.GetQualifiedName());
		FindAndReplace(emitString, "{NAME}", field.GetName());
		FindAndReplace(emitString, "{QUALIFIED_TYPE}", field.GetTypeAsString());
		FindAndReplace(emitString, "{PARENT_TYPENAME_AS_VAR}", GetTypeNameAsVar(a_parentType->GetQualifiedName()));
		FindAndReplace(emitString, "{ACCESS_SPECIFIER}", std::to_string(field.GetAccessSpecifier()));
		FindAndReplace(emitString, "{OFFSET_IN_BITS}", std::to_string(field.GetOffsetInBits()));
		FindAndReplace(emitString, "{IS_CONST}", field.IsConst() ? "true" : "false");

		const auto& attrs = field.GetAttributes();
		std::stringstream attrStream;
		if (attrs.empty())
		{
			FindAndReplace(emitString, "{FIELD_PTR}", "");
		} else
		{
			FindAndReplace(emitString, "{FIELD_PTR}", "auto FieldPtr = ");
			for (const auto& attr : attrs)
			{
				std::string attrString = GetTrimmedStringView(R"""(
{
	auto Attribute = new {ATTR_TYPE}({ATTR_ARGS});
	Oyl::Reflection::Internal::ReflectionFactory::AddAttributeToDeclaration(FieldPtr, Attribute);
}
)"""
				).data();

				FindAndReplace(attrString, "{ATTR_TYPE}", attr.type);

				std::stringstream argStream;
				for (size_t i  = 0; i < attr.arguments.size(); i++)
				{
					const auto& arg = attr.arguments[i];

					constexpr std::string_view typeIdMarker = "__TypeId";
					auto typeIdMarkerIndex = arg.find(typeIdMarker);
					if (typeIdMarkerIndex != std::string::npos)
					{
						auto typeId = arg.substr(typeIdMarker.length() + 1);
						argStream << "::Oyl::Reflection::GetTypeId<" << typeId << ">()";
					} else
					{
						argStream << arg;
					}

					if (i < attr.arguments.size() - 1)
						argStream << ", ";
				}

				FindAndReplace(attrString, "{ATTR_ARGS}", argStream.str());
				attrStream << attrString;
			}

			AddIndentToStringStream(attrStream);
		}

		FindAndReplace(emitString, "{ATTRIBUTES}", attrStream.str());

		a_stream << emitString;
	}
}

void
RegisterFunctionsForType(
	std::stringstream& a_stream,
	const Spyll::Type* a_parentType
)
{
	for (const auto& function : a_parentType->GetFunctions())
	{
		std::string emitString = GetTrimmedStringView(R"""(
{
	Oyl::Reflection::Internal::FunctionParams FunctionParams;
	FunctionParams.qualifiedName = "{QUALIFIED_NAME}";
	FunctionParams.name = "{NAME}";
	FunctionParams.returnType = Oyl::Reflection::Type::Get<{RETURN_TYPE}>();
	FunctionParams.parentType = {PARENT_TYPENAME_AS_VAR};
	FunctionParams.accessSpecifier = static_cast<Oyl::Reflection::AccessSpecifier>({ACCESS_SPECIFIER});
	{RETURN_TYPE}(*address)({ARG_SIGNATURE}) = {RAW_FN_ADDRESS};
	FunctionParams.rawFnPtr = *reinterpret_cast<void**>(&address);
{TYPESAFE_PTR}
	{INVOKABLE_PTR}Oyl::Reflection::Internal::ReflectionFactory::AddFunctionToType({PARENT_TYPENAME_AS_VAR}, FunctionParams, a_allocate);
{ARGUMENTS}
}
)"""
		).data();

		FindAndReplace(emitString, "{QUALIFIED_NAME}", function.GetQualifiedName());
		FindAndReplace(emitString, "{NAME}", function.GetName());
		FindAndReplace(emitString, "{RETURN_TYPE}", function.GetReturnTypeAsString());
		FindAndReplace(emitString, "{PARENT_TYPENAME_AS_VAR}", GetTypeNameAsVar(a_parentType->GetQualifiedName()));
		FindAndReplace(emitString, "{ACCESS_SPECIFIER}", std::to_string(function.GetAccessSpecifier()));
		FindAndReplace(emitString, "{INVOKABLE_PTR}", function.GetArguments().size() != 0 ? "auto InvokablePtr = " : "");

		std::stringstream argSignature;
		for (size_t i = 0; i < function.GetArguments().size(); i++)
		{
			const auto& arg = function.GetArguments()[i];
			argSignature << arg.type;

			if (i < function.GetArguments().size() - 1)
				argSignature << ", ";
		}

		FindAndReplace(emitString, "{ARG_SIGNATURE}", argSignature.str());

		std::stringstream rawAddress;
		if (function.GetAccessSpecifier() == 0)
		{
			// Namespace::Function
			rawAddress << "&" << function.GetQualifiedName();
		} else
		{
			// (Oyl::Reflection::Internal::Stowed<__Stow_Namespace_Function>::value)
			rawAddress << GetStowCall(function.GetQualifiedName());
		}
		FindAndReplace(emitString, "{RAW_FN_ADDRESS}", rawAddress.str());

		std::string typesafePtrString = GetTrimmedStringView(R"""(
auto thunkLambda = []({NAME_SIGNATURE}) -> std::any
{
{RAW_CALL}
};
std::any(*thunkFnPtr)({TYPE_SIGNATURE}) = thunkLambda;
FunctionParams.typeSafeThunkFnPtr = reinterpret_cast<void*>(thunkFnPtr);
)"""
		).data();

		std::stringstream nameSignature;
		std::stringstream typeSignature;
		const auto& args = function.GetArguments();
		for (size_t i = 0; i < args.size(); i++)
		{
			const auto& [type, name] = args[i];
			std::string_view std_any_ref = "std::any&";
			typeSignature << std_any_ref;
			nameSignature << std_any_ref << " " << name;

			if (i != args.size() - 1)
			{
				typeSignature << ", ";
				nameSignature << ", ";
			}
		}
		FindAndReplace(typesafePtrString, "{NAME_SIGNATURE}", nameSignature.str());
		FindAndReplace(typesafePtrString, "{TYPE_SIGNATURE}", typeSignature.str());

		std::stringstream callStatement;
		if (function.GetAccessSpecifier() == 0)
		{
			// Namespace::Function
			callStatement << function.GetQualifiedName();
		} else
		{
			// (Oyl::Reflection::Internal::Stowed<Stow_Namespace_Function, &Namespace::Function>::value)
			callStatement << GetStowCall(function.GetQualifiedName());
		}
		callStatement << "(" << (args.size() != 0 ? "\n" : "");
		for (size_t i = 0; i < args.size(); i++)
		{
			const auto& [argType, argName] = args[i];

			std::string declaration = "\tstd::any_cast<{TYPE}>({NAME})";
			FindAndReplace(declaration, "{TYPE}", argType);
			FindAndReplace(declaration, "{NAME}", argName);

			callStatement << declaration;
			if (i != args.size() - 1)
				callStatement << ", ";
			callStatement << "\n";
		}
		callStatement << ")";

		std::stringstream callReturn;
		if (function.GetReturnTypeAsString() == "void")
		{
			callReturn << callStatement.str() << ";\n";
			callReturn << "return std::any {};";
		} else
		{
			callReturn << "std::any result = " << callStatement.str() << ";\n";
			callReturn << "return result;";
		}
		AddIndentToStringStream(callReturn);
		FindAndReplace(typesafePtrString, "{RAW_CALL}", callReturn.str());

		std::stringstream typesafePtr;
		typesafePtr << typesafePtrString;
		AddIndentToStringStream(typesafePtr);
		FindAndReplace(emitString, "{TYPESAFE_PTR}", typesafePtr.str());

		std::stringstream argumentsStream;
		RegisterArgumentForFunction(argumentsStream, &function);
		AddIndentToStringStream(argumentsStream);
		FindAndReplace(emitString, "{ARGUMENTS}", argumentsStream.str());

		a_stream << emitString;
	}
}

void
RegisterMethodsForType(
	std::stringstream& a_stream,
	const Spyll::Type* a_parentType
)
{
	for (const auto& method : a_parentType->GetMethods())
	{
		std::string emitString = GetTrimmedStringView(R"""(
{
	Oyl::Reflection::Internal::MethodParams MethodParams;
	MethodParams.qualifiedName = "{QUALIFIED_NAME}";
	MethodParams.name = "{NAME}";
	MethodParams.returnType = Oyl::Reflection::Type::Get<{RETURN_TYPE}>();
	MethodParams.parentType = {PARENT_TYPENAME_AS_VAR};
	MethodParams.accessSpecifier = static_cast<Oyl::Reflection::AccessSpecifier>({ACCESS_SPECIFIER});
	MethodParams.isConst = {IS_CONST};
	MethodParams.isVirtual = {IS_VIRTUAL};
	{RETURN_TYPE}({PARENT_QUALIFIED_NAME}::*address)({ARG_SIGNATURE}){FN_CONST_QUAL} = {RAW_FN_ADDRESS};
	MethodParams.rawFnPtr = *reinterpret_cast<void**>(&address);
{TYPESAFE_PTR}
	{INVOKABLE_PTR}Oyl::Reflection::Internal::ReflectionFactory::AddMethodToType({PARENT_TYPENAME_AS_VAR}, MethodParams, a_allocate);
{ARGUMENTS}
}
)"""
		).data();

		FindAndReplace(emitString, "{QUALIFIED_NAME}", method.GetQualifiedName());
		FindAndReplace(emitString, "{NAME}", method.GetName());
		FindAndReplace(emitString, "{RETURN_TYPE}", method.GetReturnTypeAsString());
		FindAndReplace(emitString, "{PARENT_QUALIFIED_NAME}", a_parentType->GetQualifiedName());
		FindAndReplace(emitString, "{PARENT_TYPENAME_AS_VAR}", GetTypeNameAsVar(a_parentType->GetQualifiedName()));
		FindAndReplace(emitString, "{ACCESS_SPECIFIER}", std::to_string(method.GetAccessSpecifier()));
		FindAndReplace(emitString, "{IS_CONST}", method.IsConst() ? "true" : "false");
		FindAndReplace(emitString, "{IS_VIRTUAL}", method.IsVirtual() ? "true" : "false");
		FindAndReplace(emitString, "{INVOKABLE_PTR}", method.GetArguments().size() != 0 ? "auto InvokablePtr = " : "");
		FindAndReplace(emitString, "{FN_CONST_QUAL}", method.IsConst() ? " const" : "");

		std::stringstream argSignature;
		for (size_t i = 0; i < method.GetArguments().size(); i++)
		{
			const auto& arg = method.GetArguments()[i];
			argSignature << arg.type;

			if (i < method.GetArguments().size() - 1)
				argSignature << ", ";
		}

		FindAndReplace(emitString, "{ARG_SIGNATURE}", argSignature.str());

		std::stringstream rawAddress;
		if (method.GetAccessSpecifier() == 0)
		{
			// Namespace::Function
			rawAddress << "&" << method.GetQualifiedName();
		} else
		{
			// (Oyl::Reflection::Internal::Stowed<__Stow_Namespace_Function>::value)
			rawAddress << GetStowCall(method.GetQualifiedName());
		}
		FindAndReplace(emitString, "{RAW_FN_ADDRESS}", rawAddress.str());

		std::string typesafePtrString = GetTrimmedStringView(R"""(
auto thunkLambda = [](std::any& __a_self{NAME_SIGNATURE}) -> std::any
{
{RAW_CALL}
};
std::any(*thunkFnPtr)(std::any&{TYPE_SIGNATURE}) = thunkLambda;
MethodParams.typeSafeThunkFnPtr = reinterpret_cast<void*>(thunkFnPtr);
)"""
		).data();

		std::stringstream nameSignature;
		std::stringstream typeSignature;
		const auto& args = method.GetArguments();
		for (size_t i = 0; i < args.size(); i++)
		{
			typeSignature << ", ";
			nameSignature << ", ";

			const auto& [type, name] = args[i];
			std::string_view std_any_ref = "std::any&";
			typeSignature << std_any_ref;
			nameSignature << std_any_ref << " " << name;
		}
		FindAndReplace(typesafePtrString, "{NAME_SIGNATURE}", nameSignature.str());
		FindAndReplace(typesafePtrString, "{TYPE_SIGNATURE}", typeSignature.str());

		std::stringstream callStatement;
		{
			std::string callFunc;
			if (method.GetAccessSpecifier() == 0)
			{
				// Namespace::Function
				callFunc = method.GetName();
			} else
			{
				// *(Oyl::Reflection::Internal::Stowed<Stow_Namespace_Function, &Namespace::Function>::value)
				callFunc = "*" + GetStowCall(method.GetQualifiedName());
			}
			callStatement << "(__self->" << callFunc << ")";
		}
		callStatement << "(" << (args.size() != 0 ? "\n" : "");
		for (size_t i = 0; i < args.size(); i++)
		{
			const auto& [argType, argName] = args[i];

			std::string declaration = "\tstd::any_cast<{TYPE}>({NAME})";
			FindAndReplace(declaration, "{TYPE}", argType);
			FindAndReplace(declaration, "{NAME}", argName);

			callStatement << declaration;
			if (i != args.size() - 1)
				callStatement << ", ";
			callStatement << "\n";
		}
		callStatement << ")";

		std::stringstream callReturn;
		callReturn << "auto __self = std::any_cast<" << a_parentType->GetQualifiedName() << "*>(__a_self);\n";
		if (method.GetReturnTypeAsString() == "void")
		{
			callReturn << callStatement.str() << ";\n";
			callReturn << "return std::any {};";
		} else
		{
			callReturn << "std::any result = " << callStatement.str() << ";\n";
			callReturn << "return result;";
		}
		AddIndentToStringStream(callReturn);
		FindAndReplace(typesafePtrString, "{RAW_CALL}", callReturn.str());

		std::stringstream typesafePtr;
		typesafePtr << typesafePtrString;
		AddIndentToStringStream(typesafePtr);
		FindAndReplace(emitString, "{TYPESAFE_PTR}", typesafePtr.str());

		std::stringstream argumentsStream;
		RegisterArgumentForFunction(argumentsStream, &method);
		AddIndentToStringStream(argumentsStream);
		FindAndReplace(emitString, "{ARGUMENTS}", argumentsStream.str());

		a_stream << emitString;
	}
}

void
RegisterArgumentForFunction(
	std::stringstream& a_stream,
	const Spyll::Function* a_function
)
{
	for (const auto& [argType, argName] : a_function->GetArguments())
	{
		std::string emitString = GetTrimmedStringView(R"""(
{
	Oyl::Reflection::Internal::ArgumentParams ArgumentParams;
	ArgumentParams.type = Oyl::Reflection::Type::Get<{TYPE}>();
	ArgumentParams.name = "{NAME}";
	Oyl::Reflection::Internal::ReflectionFactory::AddArgumentToInvokable({INVOKABLE_PTR}, ArgumentParams, a_allocate);
}
)"""
		).data();

		FindAndReplace(emitString, "{TYPE}", argType);
		FindAndReplace(emitString, "{NAME}", argName);
		FindAndReplace(emitString, "{INVOKABLE_PTR}", "InvokablePtr");

		a_stream << emitString;
	}
}

void
RegisterGlobalVariables(std::stringstream& a_stream, const Spyll::ReflectionParser* a_parser)
{
	(void) a_stream;
	(void) a_parser;

	for (const auto* variable : a_parser->GetGlobalVariables())
	{
		std::string emitString = GetTrimmedStringView(R"""(
{
	Oyl::Reflection::Internal::VariableParams VariableParams;
	VariableParams.qualifiedName = "{QUALIFIED_NAME}";
	VariableParams.name = "{NAME}";
	VariableParams.type = Oyl::Reflection::Type::Get<{QUALIFIED_TYPE}>();
	VariableParams.parentType = nullptr;
	VariableParams.accessSpecifier = Oyl::Reflection::AccessSpecifier::Global;
	Oyl::Reflection::Internal::ReflectionFactory::AddGlobalVariableToAssembly({ASSEMBLY_PTR}, VariableParams, a_allocate);
}
)"""
		).data();

		FindAndReplace(emitString, "{QUALIFIED_NAME}", variable->GetQualifiedName());
		FindAndReplace(emitString, "{NAME}", variable->GetName());
		FindAndReplace(emitString, "{QUALIFIED_TYPE}", variable->GetTypeAsString());
		FindAndReplace(emitString, "{ASSEMBLY_PTR}", "AssemblyPtr");

		a_stream << emitString;
	}
}

void
RegisterGlobalFunctions(std::stringstream& a_stream, const Spyll::ReflectionParser* a_parser)
{
	for (const auto& function : a_parser->GetGlobalFunctions())
	{
		std::string emitString = GetTrimmedStringView(R"""(
{
	Oyl::Reflection::Internal::FunctionParams FunctionParams;
	FunctionParams.qualifiedName = "{QUALIFIED_NAME}";
	FunctionParams.name = "{NAME}";
	FunctionParams.returnType = Oyl::Reflection::Type::Get<{RETURN_TYPE}>();
	FunctionParams.parentType = nullptr;
	FunctionParams.accessSpecifier = Oyl::Reflection::AccessSpecifier::Global;
	{RETURN_TYPE}(*address)({ARG_SIGNATURE}) = {RAW_FN_ADDRESS};
	FunctionParams.rawFnPtr = reinterpret_cast<void*>(address);
{TYPESAFE_PTR}
	{INVOKABLE_PTR}Oyl::Reflection::Internal::ReflectionFactory::AddGlobalFunctionToAssembly({ASSEMBLY_PTR}, FunctionParams, a_allocate);
{ARGUMENTS}
}
)"""
		).data();

		FindAndReplace(emitString, "{QUALIFIED_NAME}", function->GetQualifiedName());
		FindAndReplace(emitString, "{NAME}", function->GetName());
		FindAndReplace(emitString, "{RETURN_TYPE}", function->GetReturnTypeAsString());
		FindAndReplace(emitString, "{INVOKABLE_PTR}", function->GetArguments().size() != 0 ? "auto InvokablePtr = " : "");
		FindAndReplace(emitString, "{ASSEMBLY_PTR}", "AssemblyPtr");
		FindAndReplace(emitString, "{RAW_FN_ADDRESS}", function->GetQualifiedName());

		std::stringstream argSignature;
		for (size_t i = 0; i < function->GetArguments().size(); i++)
		{
			const auto& arg = function->GetArguments()[i];
			argSignature << arg.type;

			if (i < function->GetArguments().size() - 1)
				argSignature << ", ";
		}

		FindAndReplace(emitString, "{ARG_SIGNATURE}", argSignature.str());

		std::string typesafePtrString = GetTrimmedStringView(R"""(
auto thunkLambda = []({NAME_SIGNATURE}) -> std::any
{
{RAW_CALL}
};
std::any(*thunkFnPtr)({TYPE_SIGNATURE}) = thunkLambda;
FunctionParams.typeSafeThunkFnPtr = reinterpret_cast<void*>(thunkFnPtr);
)"""
		).data();

		std::stringstream nameSignature;
		std::stringstream typeSignature;
		const auto& args = function->GetArguments();
		for (size_t i = 0; i < args.size(); i++)
		{
			const auto& [type, name] = args[i];
			std::string_view std_any_ref = "std::any&";
			typeSignature << std_any_ref;
			nameSignature << std_any_ref << " " << name;

			if (i != args.size() - 1)
			{
				typeSignature << ", ";
				nameSignature << ", ";
			}
		}
		FindAndReplace(typesafePtrString, "{NAME_SIGNATURE}", nameSignature.str());
		FindAndReplace(typesafePtrString, "{TYPE_SIGNATURE}", typeSignature.str());

		std::stringstream callStatement;
		callStatement << function->GetQualifiedName();
		callStatement << "(" << (args.size() != 0 ? "\n" : "");
		for (size_t i = 0; i < args.size(); i++)
		{
			const auto& [argType, argName] = args[i];

			std::string declaration = "\tstd::any_cast<{TYPE}>({NAME})";
			FindAndReplace(declaration, "{TYPE}", argType);
			FindAndReplace(declaration, "{NAME}", argName);

			callStatement << declaration;
			if (i != args.size() - 1)
				callStatement << ", ";
			callStatement << "\n";
		}
		callStatement << ")";

		std::stringstream callReturn;
		if (function->GetReturnTypeAsString() == "void")
		{
			callReturn << callStatement.str() << ";\n";
			callReturn << "return std::any {};";
		} else
		{
			callReturn << "std::any result = " << callStatement.str() << ";\n";
			callReturn << "return result;";
		}
		AddIndentToStringStream(callReturn);
		FindAndReplace(typesafePtrString, "{RAW_CALL}", callReturn.str());

		std::stringstream typesafePtr;
		typesafePtr << typesafePtrString;
		AddIndentToStringStream(typesafePtr);
		FindAndReplace(emitString, "{TYPESAFE_PTR}", typesafePtr.str());

		std::stringstream argumentsStream;
		RegisterArgumentForFunction(argumentsStream, function);
		AddIndentToStringStream(argumentsStream);
		FindAndReplace(emitString, "{ARGUMENTS}", argumentsStream.str());

		a_stream << emitString;
	}
}

void
RegisterEnums(std::stringstream& a_stream, const Spyll::ReflectionParser* a_parser)
{
	for (const auto& enum_ : a_parser->GetEnums())
	{
		std::string emitString = GetTrimmedStringView(R"""(
{
	Oyl::Reflection::Internal::EnumParams EnumParams;
	EnumParams.qualifiedName = "{QUALIFIED_NAME}";
	EnumParams.name = "{NAME}";
	EnumParams.underlyingType = Oyl::Reflection::Type::Get<{UNDERLYING_TYPE}>();
	EnumParams.nValues = static_cast<size_t>({SIZE});
	{ENUM_PTR}Oyl::Reflection::Internal::ReflectionFactory::AddEnumToAssembly({ASSEMBLY_PTR}, EnumParams, a_allocate);
{ENTRIES}
}
)"""
		).data();

		FindAndReplace(emitString, "{QUALIFIED_NAME}", enum_->GetQualifiedName());
		FindAndReplace(emitString, "{NAME}", enum_->GetName());
		FindAndReplace(emitString, "{UNDERLYING_TYPE}", enum_->GetUnderlyingTypeAsString());
		FindAndReplace(emitString, "{SIZE}", std::to_string(enum_->GetEntries().size()));
		FindAndReplace(emitString, "{ASSEMBLY_PTR}", "AssemblyPtr");
		FindAndReplace(emitString, "{ENUM_PTR}", !enum_->GetEntries().empty() ? "auto EnumPtr = " : "");

		std::stringstream entryStream;
		for (const auto& entry : enum_->GetEntries())
		{
			std::string entryString = GetTrimmedStringView(R"""(
{
	Oyl::Reflection::Internal::EnumValueParams EnumValueParams;
	EnumValueParams.identifier = "{IDENTIFIER}";
	EnumValueParams.value = static_cast<{UNDERLYING_TYPE}>({VALUE});
	Oyl::Reflection::Internal::ReflectionFactory::AddValueToEnum(EnumPtr, EnumValueParams, a_allocate);
}
)"""
			).data();

			FindAndReplace(entryString, "{IDENTIFIER}", entry.GetIdentifier());
			FindAndReplace(entryString, "{VALUE}", std::to_string(entry.GetValue()));
			FindAndReplace(entryString, "{UNDERLYING_TYPE}", enum_->GetUnderlyingTypeAsString());

			entryStream << entryString;
		}

		AddIndentToStringStream(entryStream);
		FindAndReplace(emitString, "{ENTRIES}", entryStream.str());

		a_stream << emitString;
	}
}
