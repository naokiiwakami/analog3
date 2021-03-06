#include "server/synth.h"
#include "server/errors.h"

namespace analog3 {

Synth::Synth() {
}

Synth::~Synth() {
  for (std::unordered_map<int, models::Module*>::iterator it = models.begin();
       it != models.end();
       ++it) {
    delete it->second;
  }
}

Status Synth::AddModelEntry(models::Module* model_entry) {
  uint16_t model_id = model_entry->GetModelId();
  if (model_id == 0) {
    return Status::MODEL_NOT_A_MODEL;
  }
  if (models.find(model_id) != models.end()) {
    return Status::MODEL_ID_CONFLICT;
  }
  models[model_id] = model_entry;
  return Status::OK;
}

}  // namespace analog3
