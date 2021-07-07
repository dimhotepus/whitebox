// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Compilers configuration.

#ifndef WB_BUILD_COMPILER_CONFIG_H_
#define WB_BUILD_COMPILER_CONFIG_H_

#include "build/build_config.h"

#ifdef WB_COMPILER_MSVC

/*
 * @brief Begins MSVC warning override scope.
 */
#define WB_COMPILER_MSVC_BEGIN_WARNING_OVERRIDE_SCOPE() __pragma(warning(push))

/*
 * @brief Disables MSVC warning |warning_level|.
 */
#define WB_COMPILER_MSVC_DISABLE_WARNING(warning_level) \
  __pragma(warning(disable : warning_level))

/*
 * @brief Ends MSVC warning override scope.
 */
#define WB_COMPILER_MSVC_END_WARNING_OVERRIDE_SCOPE() __pragma(warning(pop))

/*
 * @brief Disable MSVC warning |warning_level| for code |code|.
 */
#define WB_COMPILER_MSVC_SCOPED_DISABLE_WARNING(warning_level, code) \
  WB_COMPILER_MSVC_BEGIN_WARNING_OVERRIDE_SCOPE()                    \
  WB_COMPILER_MSVC_DISABLE_WARNING(warning_level)                    \
  code WB_COMPILER_MSVC_END_WARNING_OVERRIDE_SCOPE()

/*
 * @brief Can be applied to custom memory-allocation functions to make the
 * allocations visible via Event Tracing for Windows (ETW):
 *
 * WB_COMPILER_MSVC_HEAP_ALLOCATOR void* myMalloc(size_t size)
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/allocator
 */
#define WB_COMPILER_MSVC_HEAP_ALLOCATOR __declspec(allocator)

/*
 * @brief Exports functions, data, and objects from a DLL.
 *
 * "If a class is marked declspec(dllexport), any specializations of class
 * templates in the class hierarchy are implicitly marked as
 * declspec(dllexport).  This means that class templates are explicitly
 * instantiated and the class's members must be defined."
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/dllexport-dllimport
 */
#define WB_ATTRIBUTE_DLL_EXPORT __declspec(dllexport)

/*
 * @brief Imports functions, data, and objects from a DLL.
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/dllexport-dllimport
 */
#define WB_ATTRIBUTE_DLL_IMPORT __declspec(dllimport)

/*
 * @brief Compiler generates code without prolog and epilog code.  You can use
 * this feature to write your own prolog/epilog code sequences using inline
 * assembler code:
 *
 * WB_COMPILER_MSVC_NAKED int func( formal_parameters ) {}
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/naked-cpp
 */
#define WB_COMPILER_MSVC_NAKED __declspec(naked)

/*
 * @brief Function call doesn't modify or reference visible global state and
 * only modifies the memory pointed to directly by pointer parameters
 * (first-level indirections).
 *
 * "The noalias annotation only applies within the body of the annotated
 * function.  Marking a function as __declspec(noalias) doesn't affect the
 * aliasing of pointers returned by the function.  For another annotation that
 * can impact aliasing, see __declspec(restrict)."
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/noalias
 */
#define WB_COMPILER_MSVC_NOALIAS __declspec(noalias)

/*
 * Never inline a particular member function (function in a class).
 *
 * "It may be worthwhile to not inline a function if it is small and not
 * critical to the performance of your code.  That is, if the function is small
 * and not likely to be called often, such as a function that handles an error
 * condition.  Keep in mind that if a function is marked noinline, the calling
 * function will be smaller and thus, itself a candidate for compiler inlining."
 *
 * WB_COMPILER_MSVC_NOALIAS int mbrfunc() { return 0; }  // will not inline
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/noinline
 */
#define WB_COMPILER_MSVC_NOINLINE __declspec(noinline)

/*
 * @brief Tells the compiler to disable the address sanitizer on functions,
 * local variables, or global variables.  This specifier is used in conjunction
 * with AddressSanitizer.
 *
 * WB_COMPILER_MSVC_NOSANITIZE_ADDRESS disables compiler behavior, not runtime
 * behavior.
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/no-sanitize-address
 */
#define WB_COMPILER_MSVC_NOSANITIZE_ADDRESS __declspec(no_sanitize_address)

/*
 * @brief Should only be applied to pure interface classes, that is, classes
 * that will never be instantiated on their own.  "The WB_COMPILER_MSVC_NOVTABLE
 * stops the compiler from generating code to initialize the vfptr in the
 * constructor(s) and destructor of the class.  In many cases, this removes the
 * only references to the vtable that are associated with the class and, thus,
 * the linker will remove it."
 *
 * "Using this can result in a significant reduction in code size.  If you
 * attempt to instantiate a class marked with novtable and then access a class
 * member, you will receive an access violation (AV)."
 *
 * struct WB_COMPILER_MSVC_NOVTABLE X {
 *   virtual void mf() = 0;
 * };
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/novtable
 */
#define WB_COMPILER_MSVC_NOVTABLE __declspec(novtable)

/*
 * @brief When applied to a function declaration or definition that returns a
 * pointer type, restrict tells the compiler that the function returns an object
 * that is not aliased, that is, referenced by any other pointers.  This allows
 * the compiler to perform additional optimizations.
 *
 * The compiler propagates __declspec(restrict).  For example, the CRT malloc
 * function has a __declspec(restrict) decoration, and therefore, the compiler
 * assumes that pointers initialized to memory locations by malloc are also not
 * aliased by previously existing pointers.

 * The compiler does not check that the returned pointer is not actually
 * aliased.  It is the developer's responsibility to ensure the program does not
 * alias a pointer marked with the restrict __declspec modifier.
 *
 * WB_COMPILER_MSVC_RESTRICT_FUNCTION pointer_return_type function();
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/restrict
 */
#define WB_COMPILER_MSVC_RESTRICT_FUNCTION __declspec(restrict)

/*
 * @brief Indicates that a symbol isn't aliased in the current scope.  The
 * __restrict keyword differs from the __declspec (restrict) modifier in the
 * following ways:
 * * The __restrict keyword is valid only on variables, and __declspec
 * (restrict) is only valid on function declarations and definitions.
 * * __restrict is similar to restrict for C starting in C99, but __restrict can
 * be used in both C++ and C programs.
 * * When __restrict is used, the compiler won't propagate the no-alias property
 * of a variable.  That is, if you assign a __restrict variable to a
 * non-__restrict variable, the compiler will still allow the non-__restrict
 * variable to be aliased.
 *
 * This is different from the behavior of the C99 C language restrict keyword.
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/extension-restrict
 */
#define WB_COMPILER_MSVC_RESTRICT_VAR __restrict

/*
 * @brief Tells the compiler that the declared global data item (variable or
 * object) is a pick-any COMDAT (a packaged function).
 *
 * "At link time, if multiple definitions of a COMDAT are seen, the linker picks
 * one and discards the rest.  If the linker option /OPT:REF (Optimizations) is
 * selected, then COMDAT elimination will occur to remove all the unreferenced
 * data items in the linker output."
 *
 * WB_COMPILER_MSVC_SELECTANY declarator
 *
 * See https://docs.microsoft.com/en-us/cpp/cpp/selectany
 */
#define WB_COMPILER_MSVC_SELECTANY __declspec(selectany)

#else  // !WB_COMPILER_MSVC

#define WB_COMPILER_MSVC_BEGIN_WARNING_OVERRIDE_SCOPE()
#define WB_COMPILER_MSVC_DISABLE_WARNING(warning_level)
#define WB_COMPILER_MSVC_END_WARNING_OVERRIDE_SCOPE()
#define WB_COMPILER_MSVC_SCOPED_DISABLE_WARNING(warning_level, code) code

// TODO: Add these at least for GCC / Clang.

#define WB_COMPILER_MSVC_HEAP_ALLOCATOR define me

#define WB_ATTRIBUTE_DLL_EXPORT define me
#define WB_ATTRIBUTE_DLL_IMPORT define me

#define WB_COMPILER_MSVC_NAKED define me

#define WB_COMPILER_MSVC_NOALIAS define me

#define WB_COMPILER_MSVC_NOINLINE define me

#define WB_COMPILER_MSVC_NOSANITIZE_ADDRESS define me

#define WB_COMPILER_MSVC_NOVTABLE define me

#define WB_COMPILER_MSVC_RESTRICT_FUNCTION define me
#define WB_COMPILER_MSVC_RESTRICT_VAR define me

#define WB_COMPILER_MSVC_SELECTANY define me

#endif  // WB_COMPILER_MSVC

#endif  // !WB_BUILD_COMPILER_CONFIG_H_
