#ifndef SYNTH_FINDER_H_
#define SYNTH_FINDER_H_

#include <dirent.h>
#include <string>
#include "rapidjson/document.h"
#include "synth/errors.h"
#include "synth/synth_node.h"

namespace analog3 {

class SynthComponent;

class Finder {
 public:
  explicit Finder(const std::string& dir_name);
  virtual ~Finder();

  Status load();

 private:
  const std::string m_dir_name;

  void IterateDirectory(DIR* dp);

  rapidjson::Document* MakeDocument(const std::string& file_name);
  Status BuildComponent(rapidjson::Document* doc, SynthNode **node);
  Status BuildModule(rapidjson::Document* doc, SynthNode **node);

  // disable the default constructor
  Finder();
};

}  // namespace analog3
#endif  // SYNTH_FINDER_H_
