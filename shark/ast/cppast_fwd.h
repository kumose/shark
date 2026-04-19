// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


#pragma once

namespace cppast {
    class CodeGenerator;
    class CompileConfig;
    class CppAccessSpecifier;
    class CppAliasTemplate;
    class CppArrayType;
    class CppAttribute;
    class CppAutoType;
    class CppBaseClass;
    class CppBitfield;
    class CppBuiltinType;
    class CppClass;
    class CppClassTemplate;
    class CppClassTemplateSpecialization;
    class CppConcept;
    class CppConstructor;
    class CppConversionOp;
    class CppCvQualifiedType;
    class CppDecltypeAutoType;
    class CppDecltypeType;
    class CppDependentType;
    class CppDestructor;
    class CppEntity;
    class CppEntityIndex;
    class CppEnum;
    class CppEnumValue;
    class CppExpression;
    class CppFile;
    class CppForwardDeclarable;
    class CppFriend;
    class CppFunction;
    class CppFunctionBase;
    class CppFunctionParameter;
    class CppFunctionTemplate;
    class CppFunctionTemplateSpecialization;
    class CppFunctionType;
    class CppIncludeDirective;
    class CppLanguageLinkage;
    class CppLiteralExpression;
    class CppMacroDefinition;
    class CppMacroParameter;
    class CppMemberFunction;
    class CppMemberFunctionBase;
    class CppMemberFunctionType;
    class CppMemberObjectType;
    class CppMemberVariableBase;
    class CppNamespace;
    class CppNonTypeTemplateParameter;
    class CppPointerType;
    class CppReferenceType;
    class CppScopeName;
    class CppStaticAssert;
    class CppTemplate;
    class CppTemplateArgument;
    class CppTemplateInstantiationType;
    class CppTemplateParameter;
    class CppTemplateParameterType;
    class CppTemplateSpecialization;
    class CppTemplateTemplateParameter;
    class CppTemplateTypeParameter;
    class CppTokenString;
    class CppType;
    class CppTypeAlias;
    class CppUnexposedEntity;
    class CppUnexposedExpression;
    class CppUnexposedType;
    class CppUserDefinedType;
    class CppUsingDeclaration;
    class CppUsingDirective;
    class CppVariable;
    class CppVariableBase;
    class CppVariableTemplate;
    class DiagnosticLogger;
    class libclang_compilation_database;
    class libclang_compile_config;
    class libclang_error;
    class libclang_parser;
    class parser;
    class string_view;

    enum class CompileFlag;
    enum class CppAttributeKind;
    enum class CppClassKind;
    enum class CppEntityKind;
    enum class CppExpressionKind;
    enum class CppIncludeKind;
    enum class CppStandard;
    enum class cpp_template_keyword;
    enum class CppTokenKind;
    enum class CppTypeKind;
    enum class CppVirtualFlags;
    enum class FormattingFlags;
    enum class severity;
    enum class visit_filter;

    enum CppAccessSpecifierKind : int;
    enum CppBuiltinTypeKind : int;
    enum CppCv : int;
    enum CppFunctionBodyKind : int;
    enum CppReference : int;
    enum CppStorageClassSpecifiers : int;
    enum VisitorResult : bool;

    struct CppDocComment;
    struct CppEntityId;
    struct CppToken;
    struct Diagnostic;
    struct newl_t;
    struct SourceLocation;
    struct VisitorInfo;
    struct whitespace_t;

    template<class Derived, typename T>
    class CppEntityContainer;
    template<class Parser>
    class simple_file_parser;
    template<typename T, typename Predicate>
    class BasicCppEntityRef;
} // namespace cppast
