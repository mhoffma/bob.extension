/**
 * @file bob/extension/include/bob.extension/documentation.h
 * @date Fri Feb 21 18:29:37 CET 2014
 * @author Manuel Guenther <manuel.guenther@idiap.ch>
 *
 * @brief Implements a few functions to generate doc strings
 *
 * Copyright (C) 2011-2014 Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_EXTENSION_DOCUMENTATION_H_INCLUDED
#define BOB_EXTENSION_DOCUMENTATION_H_INCLUDED

#include <string>
#include <vector>
#include <set>
#include <stdexcept>
#include <iostream>
#include <string.h>

#include <bob.extension/defines.h>

namespace bob{
  namespace extension{

    /**
     * Use a static object of this class to document a variable.
     * This class can be used to document both global variables as well as class member variables.
     */
    class VariableDoc {
      friend class ClassDoc;
      public:
        /**
         * Generates a VariableDoc object. Please assure that use use this as a static member variable.
         * @param variable_name      The name of the variable
         * @param variable_type      The type of the variable, e.g., "float" or "array_like (float, 2D)"
         * @param short_description  A short description of the variable
         * @param long_description   An optional long description of the variable
         */
        VariableDoc(
          const char* const variable_name,
          const char* const variable_type,
          const char* const short_description,
          const char* const long_description = 0
        );

        /**
         * Returns the name of the variable that is documented, i.e., the "variable_name" parameter of the constructor.
         */
        char* name() const {return const_cast<char*>(variable_name.c_str());}

        /**
         * Generates and returns the documentation string.
         * @param alignment The default alignment is 80 characters.
         *                  Since the package level documentation is automatically indented by 8 spaces in the python documentation, we need to subtract these values here...
         * @return The documentation string, properly aligned, possibly including "ToDo's" for detected problems.
         */
        char* doc(const unsigned alignment = 72) const;

      private:
         // variable name and type
         std::string variable_name;
         std::string variable_type;
         // variable description
         std::string variable_description;

         // an internal string that is generated and returned.
         mutable std::string description;

    };

    /**
     * Use a static object of this class to generate a properly aligned function documentation.
     * Documentation generated by this class can be used for non-member functions as well as for member functions and constructors.
     */
    class FunctionDoc {
      friend class ClassDoc;
      public:
        /**
         * Generates a FunctionDoc object. Please assure that use use this as a static member variable.
         * @param function_name     The name of the function you want to document
         * @param short_description A short description of what the function does
         * @param long_description  An optional long description of the function
         * @param is_member_function  Set this to true if this documentation is the documentation of a member function
         */
        FunctionDoc(
          const char* const function_name,
          const char* const short_description,
          const char* const long_description = 0,
          bool is_member_function = false
        );

        /**
         * Copy constructor; will deep-copy the kwlists
         */
        FunctionDoc(const FunctionDoc& other);

        /** Destrcutor */
        ~FunctionDoc();

        /**
         * Add a prototypical call for this function by defining the parameters and the return values.
         * This function has to be called at least ones.
         * @param variables    A string containing a comma-separated list of parameters, e.g., "param1, param2"
         * @param return_value A string containing a comma-separated list of return values, e.g., "retval1, retval2".
         *                     If the function does not return anything, this value can be left at its default "None".
         *                     To document a constructor, please use "" as return value.
         */
        FunctionDoc& add_prototype(
          const char* const variables,
          const char* const return_value = "None"
        );

        /**
         * Add the documentation for a parameter added with the add_prototype function
         * @param parameter_name   The name of the parameter, e.g. "param1"
         * @param parameter_type   The type of the parameter, e.g. "float" or "array_like (float, 2D)"; indicate if the parameter is optional here
         * @param parameter_description  The description of the parameter
         */
        FunctionDoc& add_parameter(
          const char* const parameter_name,
          const char* const parameter_type,
          const char* const parameter_description
        );
        /**
         * Add the documentation of a return value added with the add_prototype function
         * @param return_name   The name assigned to the return value
         * @param return_type   The tape of the returned value
         * @param return_description  The description of the return value
         */
        FunctionDoc& add_return(
          const char* const return_name,
          const char* const return_type,
          const char* const return_description
        );

        /**
         * Returns the name of the function that is documented (i.e., the function_name parameter of the constructor)
         */
        const char* const name() const {return function_name.c_str();}

        /**
         * Generates and returns the documentation string.
         * A .. todo:: directive is added for each detected mistake.
         * @param alignment The default alignment is 80 characters.
         *                  Since the package level documentation is automatically indented by 8 spaces in the python documentation, we need to subtract these values here...
         * @return The documentation string, properly aligned, possibly including "ToDo's" for detected problems.
         */
        const char* const doc(const unsigned alignment = 72, const unsigned indent = 0) const;

        /**
         * Returns the (NULL-terminated) list of variables for the given prototype index, which can be used as kwlist argument in the bindings.
         * @param index  The index of the prototype
         * @return  A NULL-terminated list of variable names
         */
        char** kwlist(int index) const{
          if (index >= kwlists.size()) throw std::runtime_error("The prototype for the given index is not found");
          return kwlists[index];
        }

        /**
         * Generates and prints the usage string, simply listing the possible ways to call the function.
         *
         * The error meaasge is printed to the std::cerr stream
         */
        void print_usage() const;


      private:
        // the function name
        std::string function_name;
        // the description
        std::string function_description;
        // if this is a member function, the indentation must be shorter
        bool is_member;
        // prototypes
        std::vector<std::string> prototype_variables;
        std::vector<std::string> prototype_returns;
        // parameter documentation
        std::vector<std::string> parameter_names;
        std::vector<std::string> parameter_types;
        std::vector<std::string> parameter_descriptions;
        // return value documentation
        std::vector<std::string> return_names;
        std::vector<std::string> return_types;
        std::vector<std::string> return_descriptions;

        // the list of wey-word arguments
        std::vector<char**> kwlists;

        // an internal string that is generated and returned.
        mutable std::string description;
    };


    /**
     * Use a static object of this class to document a class.
     * Documenting a class includes the documentation of the constructor,
     * but not the documentation of the other member functions.
     * For those, please use the FunctionDoc class.
     */
    class ClassDoc{
      public:
        /**
         * Generates a ClassDoc object. Please assure that use use this as a static member variable.
         * @param class_name  The name of the class to be documented
         * @param short_description  A short description of the class
         * @param long_description  An optional long description of the class
         */
        ClassDoc(
          const char* const class_name,
          const char* const short_description,
          const char* const long_description = 0
        );

        /**
         * Add the documentation of the constructor.
         * This function can be called only once.
         * @param constructor_documentation  An instance of the FunctionDoc class that contains the documentation of the constructor.
         *                                   Please read the documentation of that class on how to generate constructor documentations.
         */
        ClassDoc& add_constructor(
          const FunctionDoc& constructor_documentation
        );

        /**
         * Adds the given function to the highlighted section.
         * @param function_documentation   An instance of the FunctionDoc class that should be highlighted.
         */
        ClassDoc& highlight(
          const FunctionDoc& function_documentation
        );

        /**
         * Adds the given variable to the highlighted section.
         * @param function_documentation   An instance of the FunctionDoc class that should be highlighted.
         */
        ClassDoc& highlight(
          const VariableDoc& variable_documentation
        );

        /**
         * Returns the name of the class that is documented, i.e., the "class_name" parameter of the constructor.
         */
        char* name() const {return const_cast<char*>(class_name.c_str());}

        /**
         * Generates and returns the documentation string.
         * @param alignment The default alignment is 80 characters.
         *                  Since the package level documentation is automatically indented by 8 spaces in the python documentation, we need to subtract these values here...
         * @return The documentation string, properly aligned, possibly including "ToDo's" for detected problems.
         */
        char* doc(const unsigned alignment = 72) const;

        /**
         * Returns the (NULL-terminated) list of variables of the constructor documentation for the given prototype index, which can be used as kwlist argument in the bindings.
         * @param index  The index of the prototype
         * @return  A NULL-terminated list of variable names
         */
        char** kwlist(int index) const{
          if (constructor.empty()) throw std::runtime_error("The class documentation does not have constructor documentation");
          return constructor.front().kwlist(index);
        }

        /**
         * Prints the usage string of the constructor, if available.
         */
        void print_usage() const {if (!constructor.empty()) constructor.front().print_usage();}


      private:
        // class name
        std::string class_name;
        // class description
        std::string class_description;
        // constructor; should be only one, though
        std::vector<FunctionDoc> constructor;

        // highlighting
        std::vector<FunctionDoc> highlighted_functions;
        std::vector<VariableDoc> highlighted_variables;

        // an internal string that is generated and returned.
        mutable std::string description;
    };

  }
}


/////////////////////////////////////////////////////////////
//////////////////// Implementations ////////////////////////
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
/// helper functions

#ifndef BOB_SHORT_DOCSTRINGS
// removes leading and trailing spaces
static std::string _strip(const std::string& str, const std::string& sep = " []()|"){
  unsigned first = 0, last = str.size();
  while (first < str.size() && sep.find(str[first]) != std::string::npos) ++first;
  while (last > 0 && sep.find(str[last-1]) != std::string::npos) --last;
  return str.substr(first, last-first);
}

// splits the given string by the given separator
static std::vector<std::string> _split(const std::string& str, char limit = ' '){
  std::vector<std::string> splits;
  size_t j = str.find_first_not_of(limit);
  size_t i = str.find(limit, j);
  j = 0;
  while (i != std::string::npos){
    splits.push_back(str.substr(j, i-j));
    j = i+1;
    i = str.find(limit, j);
  }
  splits.push_back(str.substr(j));
  return splits;
}

// aligns the given string using the given indent to the given alignment length;
// line breaks are handled carefully.
static std::string _align(std::string str, unsigned indent, unsigned alignment){
  // first, split the newlines
  auto lines = _split(str, '\n');

  std::string aligned;
  unsigned current_indent = indent;
  bool first_line = true;
  // now, split each line
  for (auto line_it = lines.begin(); line_it != lines.end(); ++line_it){
    auto words = _split(*line_it);
    // fill in one line
    unsigned len = 0;
    unsigned new_indent = indent;
    if (!line_it->empty()){
      // increase indent?
      const std::string& w = _strip(words[0], " ");
      if ((w.size() == 2 && w[0] == '.' && w[1] == '.') ||
          (w.size() >= 1 && '0' <= w[0] && '9' >= w[0]) ||
          (w.size() == 1 && '*' == w[0]) ){
        new_indent += w.size() + 1;
      }
      size_t first_word_indent = line_it->find_first_not_of(' ');
      if (first_word_indent != std::string::npos && first_word_indent != 0){
        new_indent += first_word_indent;
      }
    }
    for (auto word_it = words.begin(); word_it != words.end(); ++word_it){
      if (aligned.empty() || len + word_it->size() >= alignment || !first_line){
        // line reached alignment
        if (!aligned.empty()){
          aligned += "\n";
        }
        // add indent and start new line
        for (unsigned j = current_indent; j--;) aligned += " ";
        len = current_indent;
        first_line = true;
      }
      // set new indent
      current_indent = new_indent;
      // add word
      aligned += *word_it + " ";
      len += word_it->size() + 1;
    }
    current_indent = indent;
    first_line = false;
  }

  return aligned;
}

// Aligns the parameter description
static void _align_parameter(std::string& str, const std::string& name, const std::string& type, const std::string& description, unsigned indent, unsigned alignment){
  if (type.find(':') != std::string::npos && type.find('`') != std::string::npos)
    // we expect that this is a :py:class: directive, which is simply written
    str += _align("``" + name + "`` : " + type + "", indent, alignment) + "\n\n";
  else
    // otherwise we emphasize the parameter type with *...*
    str += _align("``" + name + "`` : *" + type + "*", indent, alignment) + "\n\n";
  str += _align(description, indent + 4, alignment) + "\n\n";
}

static std::string _prototype(const std::string& name, const std::string& variables, const std::string& retval){
  if (retval.empty())
    return "**" + name + "** (" + variables + ")";
  else
    return name + "(" + variables + ") -> " + retval;
}

static std::string _usage(const std::string& name, const std::string& variables, const std::string& retval){
  if (retval.empty())
    return name + "(" + variables + ")";
  else
    return name + "(" + variables + ") -> " + retval;
}


static void _check(std::string& doc, const std::vector<std::string>& vars, const std::vector<std::string>& docs, const std::string& type){
  // check that all parameters are documented. If not, add a TODO
  std::set<std::string> undoc;
  std::set<std::string> unused;
  // gather parameters
  for (auto pit = vars.begin(); pit != vars.end(); ++pit){
    const auto splits = _split(*pit, ',');
    for (auto sit = splits.begin(); sit != splits.end(); ++sit){
      undoc.insert(_strip(*sit));
    }
  }
  for (auto pit = docs.begin(); pit != docs.end(); ++pit){
    const auto splits = _split(*pit, ',');
    for (auto sit = splits.begin(); sit != splits.end(); ++sit){
      std::string x = _strip(*sit);
      if (undoc.find(x) == undoc.end()){
        unused.insert(x);
      } else {
        undoc.erase(x);
     }
    }
  }
  if (undoc.size()){
    std::string all;
    for (auto pit = undoc.begin(); pit != undoc.end(); ++pit){
      if (*pit != "None"){
        if (!all.empty()) all += ", ";
        all += *pit;
      }
    }
    if (!all.empty()){
      doc += "\n" + _align(".. todo:: The " + type + "(s) '" + all + "' are used, but not documented.", 0, (unsigned)-1) + "\n";
    }
  }
  if (unused.size()){
    std::string all;
    for (auto pit = unused.begin(); pit != unused.end(); ++pit){
      if (!all.empty()) all += ", ";
      all += *pit;
    }
    doc += "\n" + _align(".. todo:: The " + type + "(s) '" + all + "' are documented, but nowhere used.", 0, (unsigned)-1) + "\n";
  }
}

#endif // ! BOB_SHORT_DOCSTRINGS


/////////////////////////////////////////////////////////////
/// FunctionDoc

inline bob::extension::FunctionDoc::FunctionDoc(
  const char* const function_name,
  const char* const short_description,
  const char* const long_description,
  bool is_member_function
) : function_name(function_name), function_description(short_description), is_member(is_member_function)
{
#ifndef BOB_SHORT_DOCSTRINGS
  if (long_description){
    function_description += "\n\n";
    function_description += long_description;
  }
#endif
}

inline bob::extension::FunctionDoc::FunctionDoc(
  const bob::extension::FunctionDoc& other
)
: function_name(other.function_name),
  function_description(other.function_description),
  is_member(other.is_member),
  prototype_variables(other.prototype_variables),
  prototype_returns(other.prototype_returns),
  parameter_names(other.parameter_names),
  parameter_types(other.parameter_types),
  parameter_descriptions(other.parameter_descriptions),
  return_names(other.return_names),
  return_types(other.return_types),
  return_descriptions(other.return_descriptions)
{
  // Add the variables to the kwlists
  kwlists.resize(other.kwlists.size());
  for (unsigned i = 0; i < kwlists.size(); ++i){
    // copy all kwlists
    unsigned counts = _split(prototype_variables[i], ',').size();
    char** names = new char*[counts + 1];
    for (unsigned j = 0; j < counts; ++j){
      names[j] = const_cast<char*>(strdup(other.kwlists[i][j]));
    }
    // add terminating NULL pointer
    names[counts] = 0;
    kwlists[i] = names;
  }
}

inline bob::extension::FunctionDoc::~FunctionDoc(){

  for (unsigned i = 0; i < kwlists.size(); ++i){
    unsigned counts = _split(prototype_variables[i], ',').size();
    for (unsigned j = 0; j < counts; ++j){
      delete[] kwlists[i][j];
    }
    delete[] kwlists[i];
  }
}

inline bob::extension::FunctionDoc& bob::extension::FunctionDoc::add_prototype(
  const char* const variables,
  const char* const return_values
){
  // Add the variables to the kwlists
  std::vector<std::string> vars = _split(variables, ',');
  char** names = new char*[vars.size() + 1];
  for (unsigned i = 0; i < vars.size(); ++i){
    names[i] = const_cast<char*>(strdup(_strip(vars[i]).c_str()));
  }
  // add terminating NULL pointer
  names[vars.size()] = 0;
  kwlists.push_back(names);

  prototype_variables.push_back(variables);
#ifndef BOB_SHORT_DOCSTRINGS
  if (!return_values)
    prototype_returns.push_back("");
  else
    prototype_returns.push_back(return_values);
#endif // BOB_SHORT_DOCSTRINGS
  return *this;
}

inline bob::extension::FunctionDoc& bob::extension::FunctionDoc::add_parameter(
  const char* const parameter_name,
  const char* const parameter_type,
  const char* const parameter_description
)
{
#ifndef BOB_SHORT_DOCSTRINGS
  parameter_names.push_back(parameter_name);
  parameter_types.push_back(parameter_type);
  parameter_descriptions.push_back(parameter_description);
#endif // BOB_SHORT_DOCSTRINGS
  return *this;
}

inline bob::extension::FunctionDoc& bob::extension::FunctionDoc::add_return(
  const char* const parameter_name,
  const char* const parameter_type,
  const char* const parameter_description
)
{
#ifndef BOB_SHORT_DOCSTRINGS
  return_names.push_back(parameter_name);
  return_types.push_back(parameter_type);
  return_descriptions.push_back(parameter_description);
#endif // BOB_SHORT_DOCSTRINGS
  return *this;
}

inline const char* const bob::extension::FunctionDoc::doc(
  const unsigned alignment,
  const unsigned indent
) const
{
#ifdef BOB_SHORT_DOCSTRINGS
  return function_description.c_str();
#else
  if (description.empty()){
    // in case of member functions, the alignment has to be decreased further since class member function are automatically indented by 4 further spaces.
    unsigned align = is_member ? alignment - 4  : alignment;
    description = "";
    switch(prototype_variables.size()){
      case 0:
        description = _align(".. todo:: Please use ``FunctionDoc.add_prototype`` to add at least one prototypical way to call this function", indent, (unsigned)-1) + "\n";
        break;
      case 1:
        // only one way to call; use the default way
        description = _align(_prototype(function_name, prototype_variables[0], prototype_returns[0]), indent, unsigned(-1)) + "\n";
        break;
      default:
        // several ways to call; list them
        for (unsigned n = 0; n < prototype_variables.size(); ++n)
          description += _align("* " + _prototype(function_name, prototype_variables[n], prototype_returns[n]), indent, unsigned(-1)) + "\n";
    }
    // add function description
    description += "\n" + _align(function_description, indent, align) + "\n";

    // check that all parameters are documented
    _check(description, prototype_variables, parameter_names, "parameter");

    // check that all return values are documented
    _check(description, prototype_returns, return_names, "return value");

    if (!parameter_names.empty()){
      // add parameter description
  //    description += "\n" + _align("Parameters") + _align("----------");
      description += "\n" + _align("**Parameters:**", indent, align) + "\n\n";
      for (unsigned i = 0; i < parameter_names.size(); ++i){
        _align_parameter(description, parameter_names[i], parameter_types[i], parameter_descriptions[i], indent, align);
      }
    }

    if (!return_names.empty()){
      // add return value description
  //    description += "\n" + _align("Returns") + _align("--------");
      description += "\n" + _align("**Returns:**", indent, align) + "\n\n";
      for (unsigned i = 0; i < return_names.size(); ++i){
        _align_parameter(description, return_names[i], return_types[i], return_descriptions[i], indent, align);
      }
    }
  }

  // return the description
  return description.c_str();
#endif // BOB_SHORT_DOCSTRINGS
}

inline void bob::extension::FunctionDoc::print_usage() const
{
#ifdef BOB_SHORT_DOCSTRINGS
  return function_description.c_str();
#else
  // in case of member functions, the alignment has to be decreased further since class member function are automatically indented by 4 further spaces.
  std::cerr << "\nUsage (for details, see help):\n";
  switch(prototype_variables.size()){
    case 0:
      std::cerr << _align("Error: The usage of this function is unknown", 0, (unsigned)-1)  << "\n";
      break;
    case 1:
      // only one way to call; use the default way
      std::cerr << _align(_usage(function_name, prototype_variables[0], prototype_returns[0]), 0, unsigned(-1)) << "\n";
      break;
    default:
      // several ways to call; list them
      for (unsigned n = 0; n < prototype_variables.size(); ++n)
        std::cerr << _align(_usage(function_name, prototype_variables[n], prototype_returns[n]), 0, unsigned(-1)) << "\n";
  }
  std::cerr << std::endl;
#endif // BOB_SHORT_DOCSTRINGS
}



/////////////////////////////////////////////////////////////
/// ClassDoc

inline bob::extension::ClassDoc::ClassDoc(
  const char* const class_name,
  const char* const short_description,
  const char* const long_description
) : class_name(class_name), class_description(short_description)
{
#ifndef BOB_SHORT_DOCSTRINGS
  if (long_description){
    class_description += "\n\n";
    class_description += long_description;
  }
#endif // ! BOB_SHORT_DOCSTRINGS
}

inline bob::extension::ClassDoc& bob::extension::ClassDoc::add_constructor(
  const bob::extension::FunctionDoc& constructor_documentation
)
{
#ifndef BOB_SHORT_DOCSTRINGS
  if (!constructor.empty()){
    throw std::runtime_error("The class documentation can have only a single constructor documentation");
  }
  constructor.push_back(constructor_documentation);
  // since we indent the constructor documentation ourselves, we don't need to consider it to be a member function.
  constructor.back().is_member = false;
  constructor.back().function_name = class_name;
#endif // BOB_SHORT_DOCSTRINGS
  return *this;
}

inline bob::extension::ClassDoc& bob::extension::ClassDoc::highlight(
  const bob::extension::FunctionDoc& function_documentation
)
{
#ifndef BOB_SHORT_DOCSTRINGS
  highlighted_functions.push_back(function_documentation);
#endif // BOB_SHORT_DOCSTRINGS
  return *this;
}

inline bob::extension::ClassDoc& bob::extension::ClassDoc::highlight(
  const bob::extension::VariableDoc& variable_documentation
)
{
#ifndef BOB_SHORT_DOCSTRINGS
  highlighted_variables.push_back(variable_documentation);
#endif // BOB_SHORT_DOCSTRINGS
  return *this;
}


inline char* bob::extension::ClassDoc::doc(
  const unsigned alignment
) const
{
#ifdef BOB_SHORT_DOCSTRINGS
  return const_cast<char*>(class_description.c_str());
#else
  if (description.empty()){
    description = _align(class_description, 0, alignment) + "\n";
    if (!constructor.empty()){
      description += "\n" + _align("**Constructor Documentation:**", 0, alignment) + "\n\n";
      description += constructor.front().doc(alignment, 4) + std::string("\n");
    }
    description += "\n" + _align("**Class Members:**", 0, alignment) + "\n\n";
    if (!highlighted_functions.empty()){
  //    description += "\n" + _align("Methods") + _align("-------");
      description += "\n" + _align("**Highlighted Methods:**", 2, alignment) + "\n\n";
      for (auto hit = highlighted_functions.begin(); hit != highlighted_functions.end(); ++hit){
        description += _align("* :func:`" + hit->function_name + "`", 2, alignment) + _align(_split(hit->function_description, '\n')[0], 4, alignment) + "\n";
      }
    }
    if (!highlighted_variables.empty()){
  //    description += "\n" + _align("Attributes") + _align("----------");
      description += "\n" + _align("**Highlighted Attributes:**", 2, alignment) + "\n\n";
      for (auto hit = highlighted_variables.begin(); hit != highlighted_variables.end(); ++hit){
        description += _align("* :obj:`" + hit->variable_name + "`", 2, alignment) + _align(_split(hit->variable_description, '\n')[0], 4, alignment) + "\n";
      }
    }
  }
  return const_cast<char*>(description.c_str());
#endif // BOB_SHORT_DOCSTRINGS
}

/////////////////////////////////////////////////////////////
/// VariableDoc

inline bob::extension::VariableDoc::VariableDoc(
  const char* const variable_name,
  const char* const variable_type,
  const char* const short_description,
  const char* const long_description
) : variable_name(variable_name), variable_type(variable_type), variable_description(short_description)
{
#ifndef BOB_SHORT_DOCSTRINGS
  if (long_description){
    variable_description += "\n\n";
    variable_description += long_description;
  }
#endif // ! BOB_SHORT_DOCSTRINGS
}

inline char* bob::extension::VariableDoc::doc(
  const unsigned alignment
) const
{
#ifdef BOB_SHORT_DOCSTRINGS
  return const_cast<char*>(variable_description.c_str());
#else
  if (description.empty()){
    if (variable_type.find(':') != std::string::npos && variable_type.find('`') != std::string::npos)
      // we expect that this is a :py:class: directive, which is simply written (otherwise the *...*
      description = _align(variable_type + "  <-- " + variable_description, 0, alignment);
    else
      description = _align("*" + variable_type + "*  <-- " + variable_description, 0, alignment);
  }
  return const_cast<char*>(description.c_str());
#endif // BOB_SHORT_DOCSTRINGS
}

#endif // BOB_EXTENSION_DOCUMENTATION_H_INCLUDED

