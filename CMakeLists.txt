cmake_minimum_required(VERSION 3.22)
project(templates_practice)

set(CMAKE_CXX_STANDARD 23)

add_executable(templates_practice headers.h
        chapter1/ch_1_function_templates.cpp
        chapter1/default_args_of_template_1.4.cpp
        chapter1/overload_function_template_1.5.cpp chapter2/template_stack_2.1.cpp chapter2/partially_use_class_template_2.3.cpp chapter2/specialization_template_class_2.5.cpp chapter2/partial_specialization_template_class_2.6.cpp chapter2/default_template_args_2.7.cpp chapter2/type_aliases_2.8.cpp chapter2/decltype_for_class_template_2.9.cpp chapter2/templatized_aggregates_2.10.cpp chapter3/noneType_class_template_parameters_3.1.cpp chapter3/Nontype_Function_Template_Parameters_3.2.cpp chapter3/Restrictions_for_Nontype_Template_Parameters_3.3.cpp
        chapter3/Template_Parameter_Type_auto_3.4.cpp
        chapter4/fold_experssions_4.2.cpp chapter4/variadic_templates_4.1.cpp chapter4/Application_of_Variadic_Templates_4.3.cpp "chapter4/Variadic_Class_Templates_and_Variadic_Expressions_4.4.cpp" chapter5/Keyword_typename_5.1.cpp chapter5/Zero_Initialization_5.2.cpp chapter5/Using_this_operator_arrow_5.3.cpp chapter5/Templates_for_Raw_Arrays_and_String_Literals_5.4.cpp chapter5/Member_Templates_5.5.cpp "chapter5/Variable_Templates_5.6.cpp" chapter5/Template_Template_Parameters_5.7.cpp chapter6/Perfect_Forwarding_6.1.cpp chapter6/Special_Member_Function_Templates_6.2.cpp chapter6/Disable_templates_with_enable_if_6.3.cpp chapter6/Using_enable_if_6.4.cpp chapter6/Using_Concepts_to_Simplify_enable_if_6.5.cpp chapter7/Passing_by_Value_7.1.cpp chapter7/Passing_by_Reference_7.2.cpp chapter7/Using_std_ref_and_std_cref_7.3.cpp chapter7/Dealing_with_String_Literals_and_Raw_Arrays_7.4.cpp chapter7/Dealing_with_Return_Values_7.5.cpp chapter7/Recommended_Template_Parameter_Declarations_7.6.cpp)
