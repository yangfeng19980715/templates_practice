cmake_minimum_required(VERSION 3.22)
project(templates_practice)

set(CMAKE_CXX_STANDARD 23)

add_executable(templates_practice headers.h
        chapter1/ch_1_function_templates.cpp
        chapter1/default_args_of_template_1.4.cpp
        chapter1/overload_function_template_1.5.cpp chapter2/template_stack_2.1.cpp chapter2/partially_use_class_template_2.3.cpp chapter2/specialization_template_class_2.5.cpp chapter2/partial_specialization_template_class_2.6.cpp chapter2/default_template_args_2.7.cpp chapter2/type_aliases_2.8.cpp chapter2/decltype_for_class_template_2.9.cpp chapter2/templatized_aggregates_2.10.cpp chapter3/noneType_class_template_parameters_3.1.cpp chapter3/Nontype_Function_Template_Parameters_3.2.cpp chapter3/Restrictions_for_Nontype_Template_Parameters_3.3.cpp
        chapter3/Template_Parameter_Type_auto_3.4.cpp
        chapter4/fold_experssions_4.2.cpp chapter4/variadic_templates_4.1.cpp)
