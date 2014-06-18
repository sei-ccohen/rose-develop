/** 
 * \file lib/sage/driver.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/api.hpp"

#include "sage3basic.h"

#include <boost/filesystem.hpp>

#ifndef PATCHING_SAGE_BUILDER_ISSUES
#  define PATCHING_SAGE_BUILDER_ISSUES 1
#endif

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_driver
 * @{
*/

bool ignore(const std::string & name) {
  return name.find("__builtin") == 0 || name.find("__frontend_specific_variable_to_provide_header_file_path") == 0;
}

bool ignore(SgScopeStatement * scope) {
  return isSgBasicBlock(scope);
}

template <>
void Driver<Sage>::loadSymbols<SgDeclarationStatement>(unsigned file_id, SgSourceFile * file) {
  loadSymbols<SgNamespaceDeclarationStatement>(file_id, file);
  loadSymbols<SgFunctionDeclaration>(file_id, file);
  loadSymbols<SgClassDeclaration>(file_id, file);
  loadSymbols<SgVariableDeclaration>(file_id, file);
  loadSymbols<SgMemberFunctionDeclaration>(file_id, file);
}

unsigned Driver<Sage>::getFileID(const boost::filesystem::path & path) const {
  std::map<boost::filesystem::path, unsigned>::const_iterator it_file_id = path_to_id_map.find(path);
  assert(it_file_id != path_to_id_map.end());
  return it_file_id->second;
}

unsigned Driver<Sage>::getFileID(SgSourceFile * source_file) const {
  std::map<SgSourceFile *, unsigned>::const_iterator it_file_id = file_to_id_map.find(source_file);
  assert(it_file_id != file_to_id_map.end());
  return it_file_id->second;
}

unsigned Driver<Sage>::getFileID(SgScopeStatement * scope) const {
  SgFile * enclosing_file = SageInterface::getEnclosingFileNode(scope);
  assert(enclosing_file != NULL); // FIXME Contingency : Scope Stack

  SgSourceFile * enclosing_source_file = isSgSourceFile(enclosing_file);
  assert(enclosing_source_file != NULL);

  return getFileID(enclosing_source_file);
}

Driver<Sage>::Driver(SgProject * project_) :
  project(project_),
  file_id_counter(1), // 0 is reserved
  path_to_id_map(),
  id_to_file_map(),
  file_to_id_map(),
  file_id_to_accessible_file_id_map(),
  p_symbol_to_file_id_map(),
  p_valid_symbols(),
  p_parent_map(),
  p_namespace_symbols(),
  p_function_symbols(),
  p_class_symbols(),
  p_variable_symbols(),
  p_member_function_symbols()
{ 
  assert(project != NULL);

  if (!CommandlineProcessing::isCppFileNameSuffix("hpp"))
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back("hpp");

  if (!CommandlineProcessing::isCppFileNameSuffix("h"))
    CommandlineProcessing::extraCppSourceFileSuffixes.push_back("h");

  // Load existing files
  const std::vector<SgFile *> & files = project->get_fileList_ptr()->get_listOfFiles();
  std::vector<SgFile *>::const_iterator it_file;
  for (it_file = files.begin(); it_file != files.end(); it_file++) {
    SgSourceFile * src_file = isSgSourceFile(*it_file);
    if (src_file != NULL)
      add(src_file);
  }
}

Driver<Sage>::~Driver() {}

unsigned Driver<Sage>::add(SgSourceFile * file) {
  unsigned id = file_id_counter++;

  id_to_file_map.insert(std::pair<unsigned, SgSourceFile *>(id, file));
  file_to_id_map.insert(std::pair<SgSourceFile *, unsigned>(file, id));
  file_id_to_accessible_file_id_map.insert(std::pair<unsigned, std::set<unsigned> >(id, std::set<unsigned>())).first->second.insert(id);

  /// \todo detect other accessible file (opt: add recursively)

  loadSymbols<SgDeclarationStatement>(id, file);

  return id;
}

unsigned Driver<Sage>::add(const boost::filesystem::path & path) {
  if (boost::filesystem::exists(path)) {
    assert(boost::filesystem::is_regular_file(path));
    std::map<boost::filesystem::path, unsigned>::const_iterator it_file_id = path_to_id_map.find(path);
    if (it_file_id != path_to_id_map.end())
      return it_file_id->second;
    else {
      SgSourceFile * file = isSgSourceFile(SageBuilder::buildFile(path.string(), path.string(), project));
      assert(file != NULL);
      file->set_skip_unparse(true);
      file->set_skipfinalCompileStep(true);

      unsigned id = add(file);

      path_to_id_map.insert(std::pair<boost::filesystem::path, unsigned>(path, id));

      return id;
    }
  }
  else {
    std::map<boost::filesystem::path, unsigned>::const_iterator it_file_id = path_to_id_map.find(path);
    assert(it_file_id == path_to_id_map.end());
    
    SgSourceFile * file = isSgSourceFile(SageBuilder::buildFile(path.string(), path.string(), project));
    assert(file != NULL);
    SageInterface::attachComment(file, "/* File generated by Driver<Model>::getFileID(\"" + path.string() + "\") */");

    unsigned id = file_id_counter++;

    id_to_file_map.insert(std::pair<unsigned, SgSourceFile *>(id, file));
    file_to_id_map.insert(std::pair<SgSourceFile *, unsigned>(file, id));
    path_to_id_map.insert(std::pair<boost::filesystem::path, unsigned>(path, id));
    file_id_to_accessible_file_id_map.insert(std::pair<unsigned, std::set<unsigned> >(id, std::set<unsigned>())).first->second.insert(id);

    return id;
  }
}

void Driver<Sage>::setUnparsedFile(unsigned file_id) const {
  std::map<unsigned, SgSourceFile *>::const_iterator it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());
  it_file->second->set_skip_unparse(false);
}

void Driver<Sage>::setCompiledFile(unsigned file_id) const {
  std::map<unsigned, SgSourceFile *>::const_iterator it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());
  it_file->second->set_skipfinalCompileStep(false);
}

void Driver<Sage>::addIncludeDirectives(unsigned target_file_id, unsigned header_file_id) {
  std::string header_file_name;

  std::map<unsigned, SgSourceFile *>::const_iterator it_file = id_to_file_map.find(target_file_id);
  assert(it_file != id_to_file_map.end());
  SgSourceFile * target_file = it_file->second;
  assert(target_file != NULL);

  it_file = id_to_file_map.find(header_file_id);
  assert(it_file != id_to_file_map.end());
  SgSourceFile * header_file = it_file->second;
  assert(header_file != NULL);

  header_file_name = header_file->getFileName(); /// \todo find minimum file name (based on include path...)
  const std::vector<std::string> & arg_list = project->get_originalCommandLineArgumentList();
  std::vector<std::string>::const_iterator it_arg;
  for (it_arg = arg_list.begin(); it_arg != arg_list.end(); it_arg++) {
    if ((*it_arg).find("-I") == 0) {
      std::string inc_path = (*it_arg).substr(2);
      if (header_file_name.find(inc_path) == 0) {
        header_file_name = header_file_name.substr(inc_path.length());
        while (header_file_name[0] == '/') header_file_name = header_file_name.substr(1);
        break;
      }
    }
  }

  SageInterface::insertHeader(target_file, header_file_name);
}

void Driver<Sage>::addExternalHeader(unsigned file_id, std::string header_name, bool is_system_header) {
  std::map<unsigned, SgSourceFile *>::iterator it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());
  
  SgSourceFile * file = it_file->second;
  assert(file != NULL);

  SageInterface::insertHeader(file, header_name, is_system_header);
}

void Driver<Sage>::attachArbitraryText(unsigned file_id, std::string str) {
  std::map<unsigned, SgSourceFile *>::iterator it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());
  
  SgSourceFile * file = it_file->second;
  assert(file != NULL);

  SageInterface::attachArbitraryText(file->get_globalScope(), str);
}

void Driver<Sage>::addPointerToTopParentDeclaration(SgSymbol * symbol, unsigned file_id) {
  SgSymbol * parent = symbol;
  std::map<SgSymbol *, SgSymbol *>::const_iterator it_parent = p_parent_map.find(symbol);
  assert(it_parent != p_parent_map.end());
  while (it_parent->second != NULL) {
    parent = it_parent->second;
    it_parent = p_parent_map.find(parent);
    assert(it_parent != p_parent_map.end());
  }
  assert(parent != NULL);

  SgDeclarationStatement * decl_to_add = NULL;
  SgVariableSymbol * var_sym = isSgVariableSymbol(parent);
  if (var_sym != NULL) {
    assert(var_sym == symbol);

    SgInitializedName * init_name = isSgInitializedName(var_sym->get_symbol_basis());
    assert(init_name != NULL);

    // TODO
  }
  else
    decl_to_add = isSgDeclarationStatement(parent->get_symbol_basis());
  assert(decl_to_add != NULL);

  std::map<unsigned, SgSourceFile *>::iterator it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());
  SgSourceFile * file = it_file->second;
  assert(file != NULL);

  SgGlobal * global_scope = file->get_globalScope();
  assert(global_scope != NULL);

  const std::vector<SgDeclarationStatement *> & declaration_list = global_scope->getDeclarationList();
  if (find(declaration_list.begin(), declaration_list.end(), decl_to_add) == declaration_list.end())
    SageInterface::prependStatement(decl_to_add, global_scope);
}

api_t * Driver<Sage>::getAPI(unsigned file_id) const {
  api_t * api = new api_t();

  // Namespaces are not local to a file. If a namespace have been detected in any file, it will be in the API
  std::set<SgNamespaceSymbol *>::const_iterator it_namespace_symbol;
  for (it_namespace_symbol = p_namespace_symbols.begin(); it_namespace_symbol != p_namespace_symbols.end(); it_namespace_symbol++)
    api->namespace_symbols.insert(*it_namespace_symbol);

  std::map<SgSymbol *, unsigned>::const_iterator it_sym_decl_file_id;

  std::set<SgFunctionSymbol *>::const_iterator it_function_symbol;
  for (it_function_symbol = p_function_symbols.begin(); it_function_symbol != p_function_symbols.end(); it_function_symbol++) {
    it_sym_decl_file_id = p_symbol_to_file_id_map.find(*it_function_symbol);
    assert(it_sym_decl_file_id != p_symbol_to_file_id_map.end());

    if (it_sym_decl_file_id->second == file_id)
      api->function_symbols.insert(*it_function_symbol);
  }

  std::set<SgClassSymbol *>::const_iterator it_class_symbol;
  for (it_class_symbol = p_class_symbols.begin(); it_class_symbol != p_class_symbols.end(); it_class_symbol++) {
    it_sym_decl_file_id = p_symbol_to_file_id_map.find(*it_class_symbol);
    assert(it_sym_decl_file_id != p_symbol_to_file_id_map.end());

    if (it_sym_decl_file_id->second == file_id)
      api->class_symbols.insert(*it_class_symbol);
  }

  std::set<SgVariableSymbol *>::const_iterator it_variable_symbol;
  for (it_variable_symbol = p_variable_symbols.begin(); it_variable_symbol != p_variable_symbols.end(); it_variable_symbol++) {
    it_sym_decl_file_id = p_symbol_to_file_id_map.find(*it_variable_symbol);
    assert(it_sym_decl_file_id != p_symbol_to_file_id_map.end());

    if (it_sym_decl_file_id->second == file_id)
      api->variable_symbols.insert(*it_variable_symbol);
  }

  std::set<SgMemberFunctionSymbol *>::const_iterator it_member_function_symbol;
  for (it_member_function_symbol = p_member_function_symbols.begin(); it_member_function_symbol != p_member_function_symbols.end(); it_member_function_symbol++) {
    it_sym_decl_file_id = p_symbol_to_file_id_map.find(*it_member_function_symbol);
    assert(it_sym_decl_file_id != p_symbol_to_file_id_map.end());

    if (it_sym_decl_file_id->second == file_id)
      api->member_function_symbols.insert(*it_member_function_symbol);
  }

  return api;
}

api_t * Driver<Sage>::getAPI(const std::set<unsigned> & file_ids) const {
  assert(file_ids.size() > 0);

  std::set<unsigned>::const_iterator it = file_ids.begin();

  api_t * api = getAPI(*it);
  it++;

  while (it != file_ids.end()) {

    std::cerr << "Add file " << *it << " to API..." << std::endl;

    api_t * tmp = getAPI(*it);
    merge_api(api, tmp);
    delete tmp;
    it++;
  }

  return api;
}

api_t * Driver<Sage>::getAPI() const {
  api_t * api = new api_t();

  std::set<SgNamespaceSymbol *>::const_iterator it_namespace_symbol;
  for (it_namespace_symbol = p_namespace_symbols.begin(); it_namespace_symbol != p_namespace_symbols.end(); it_namespace_symbol++)
    api->namespace_symbols.insert(*it_namespace_symbol);

  std::set<SgFunctionSymbol *>::const_iterator it_function_symbol;
  for (it_function_symbol = p_function_symbols.begin(); it_function_symbol != p_function_symbols.end(); it_function_symbol++)
    api->function_symbols.insert(*it_function_symbol);

  std::set<SgClassSymbol *>::const_iterator it_class_symbol;
  for (it_class_symbol = p_class_symbols.begin(); it_class_symbol != p_class_symbols.end(); it_class_symbol++)
    api->class_symbols.insert(*it_class_symbol);

  std::set<SgVariableSymbol *>::const_iterator it_variable_symbol;
  for (it_variable_symbol = p_variable_symbols.begin(); it_variable_symbol != p_variable_symbols.end(); it_variable_symbol++)
    api->variable_symbols.insert(*it_variable_symbol);

  std::set<SgMemberFunctionSymbol *>::const_iterator it_member_function_symbol;
  for (it_member_function_symbol = p_member_function_symbols.begin(); it_member_function_symbol != p_member_function_symbols.end(); it_member_function_symbol++)
    api->member_function_symbols.insert(*it_member_function_symbol);

  return api;
}

/** @} */

}

