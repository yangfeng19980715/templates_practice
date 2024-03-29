cmake_minimum_required(VERSION 3.22)
project(templates_practice)

set(CMAKE_CXX_STANDARD 23)

add_executable(templates_practice headers.h
    chapter1/ch_1_function_templates.cpp chapter1/default_args_of_template_1.4.cpp chapter1/overload_function_template_1.5.cpp
    chapter2/template_stack_2.1.cpp chapter2/partially_use_class_template_2.3.cpp chapter2/specialization_template_class_2.5.cpp chapter2/partial_specialization_template_class_2.6.cpp chapter2/default_template_args_2.7.cpp chapter2/type_aliases_2.8.cpp chapter2/decltype_for_class_template_2.9.cpp chapter2/templatized_aggregates_2.10.cpp
    chapter3/noneType_class_template_parameters_3.1.cpp chapter3/Nontype_Function_Template_Parameters_3.2.cpp chapter3/Restrictions_for_Nontype_Template_Parameters_3.3.cpp chapter3/Template_Parameter_Type_auto_3.4.cpp
    chapter4/fold_experssions_4.2.cpp chapter4/variadic_templates_4.1.cpp chapter4/Application_of_Variadic_Templates_4.3.cpp chapter4/Variadic_Class_Templates_and_Variadic_Expressions_4.4.cpp
    chapter5/Keyword_typename_5.1.cpp chapter5/Zero_Initialization_5.2.cpp chapter5/Using_this_operator_arrow_5.3.cpp chapter5/Templates_for_Raw_Arrays_and_String_Literals_5.4.cpp chapter5/Member_Templates_5.5.cpp chapter5/Variable_Templates_5.6.cpp chapter5/Template_Template_Parameters_5.7.cpp
    chapter6/Perfect_Forwarding_6.1.cpp chapter6/Special_Member_Function_Templates_6.2.cpp chapter6/Disable_templates_with_enable_if_6.3.cpp chapter6/Using_enable_if_6.4.cpp chapter6/Using_Concepts_to_Simplify_enable_if_6.5.cpp
    chapter7/Passing_by_Value_7.1.cpp chapter7/Passing_by_Reference_7.2.cpp chapter7/Using_std_ref_and_std_cref_7.3.cpp chapter7/Dealing_with_String_Literals_and_Raw_Arrays_7.4.cpp chapter7/Dealing_with_Return_Values_7.5.cpp chapter7/Recommended_Template_Parameter_Declarations_7.6.cpp
    chapter8/Template_Meta_programming_8.1.cpp chapter8/Computing_with_constexpr_8.2.cpp chapter8/Execution_Path_Selection_with_Partial_Specialization_8.3.cpp chapter8/SFINAE_Substitution_Failure_Is_Not_An_Error_8.4.cpp chapter8/compile_time_if_8.5.cpp
    chapter9/The_Inclusion_Model_9.1.cpp chapter9/myfirst.cpp chapter9/myfirst.h chapter9/mysecond.h chapter9/Templates_and_inline_9.2.cpp chapter9/Precompiled_Headers_9.3.cpp chapter9/Decoding_the_Error_Novel_9.4.cpp chapter9/Afternotes_9.5.cpp
    chapter10/Class_Template_or_Template_Class_10.1.cpp
    chapter11/Callables_11_1.cpp chapter11/Other_Utilities_to_Implement_Generic_Libraries_11_2.cpp chapter11/Perfect_Forwarding_Temporaries_11_3.cpp chapter11/References_as_Template_Parameters_11_4.cpp chapter11/Defer_Evaluations_11_5.cpp chapter11/Things_to_Consider_When_Writting_Generic_Libraries_11_6.cpp
    chapter18/Dynamic_Polymorphism_18_1.cpp chapter18/Static_Polymorphism_18_2.cpp chapter18/Dynamic_versus_Static_Polymorphism_18_3.cpp chapter18/Using_Concepts_18_4.cpp chapter18/New_Forms_of_Design_Patterns_18_5.cpp chapter18/Generic_Programming_18_6.cpp chapter18/Afternotes_18_7.cpp
    chapter19/An_Example_Accumulating_a_Sequence_19_1.cpp chapter19/Traits_versus_Policies_and_Policy_Classes_19_2.cpp chapter19/Type_Function_19_3.cpp chapter19/SFINAE_Based_Traits_19_4.cpp chapter19/IsConvertibleT_19_5.cpp chapter19/Detecting_Members_19_6.cpp
    chapter19/Other_Traits_Techniques_19_7.cpp chapter19/Type_Classification_19_8.cpp chapter19/Policy_Traits_19_9.cpp chapter19/In_the_Standard_Library_19_10.cpp
    chapter20/Algorithm_Specialization_20_1.cpp chapter20/Algorithm_Specialization_20_1.cpp chapter20/Tag_Dispatching_20_2.cpp chapter20/Enabling_Disabling_Function_Templates_20_3.cpp chapter20/Class_Specialization_20_4.cpp chapter20/Instantiation_Safe_Templates_20_5.cpp chapter20/In_the_Standard_Library_20_6.cpp chapter20/Afternotes_20_7.cpp
    chapter21/The_Empty_Base_Class_Optimization_EBCO_21_1.cpp chapter21/The_Curiously_Recurring_Template_Pattern_CRTP__21_2.cpp chapter21/Mixins_21_3.cpp chapter21/Named_Template_Arguments_21_4.cpp
    chapter22/Function_Objects_Pointers_and_std_function_22_1.cpp chapter22/Generalized_Function_Pointers_22_2.cpp chapter22/Bridge_Interface_22_3.cpp chapter22/Optional_Bridging_22_5.cpp
    chapter23/The_State_of_Modern_C++_Meta_programmin_23_1.cpp chapter23/The_Dimensions_of_Reflective_Meta_programming_23_2.cpp chapter23/The_Cost_of_Recursive_Instantiation_23_3.cpp chapter23/Computational_Completeness_23_4.cpp chapter23/Recursive_Instantiation_versus_Recursive_Template_Arguments_23_5.cpp chapter23/Enumeration_Values_versus_Static_Constants_23_6.cpp chapter23/Afternotes_23_7.cpp
    chapter24/Anatomy_of_a_Typelist_24_1.cpp chapter24/Typelist_Algorithms_24_2.cpp chapter24/Nontype_Typelists_24_3.cpp chapter24/Optimizing_Algorithms_with_Pack_Expansions_24_4.cpp chapter24/Cons_style_Typelists_24_5.cpp
        chapter25/Basic_Tuple_Design_25_1.cpp chapter25/Basic_Tuple_Operations_25_2.cpp chapter25/Tuple_Algorithms_25_3.cpp chapter25/25_5.cpp)
