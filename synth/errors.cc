#include "synth/errors.h"

#define REGISTER(status) _error_strings[Status::status] = #status

namespace analog3 {

AppError::ErrorStrings AppError::error_strings;

AppError::ErrorStrings::ErrorStrings() {
  REGISTER(OK);
  REGISTER(FILE_NOT_FOUND);

  REGISTER(SCHEMA_NOT_AN_OBJECT);
  REGISTER(SCHEMA_REQUIRED_PROPERTY_MISSING);
  REGISTER(SCHEMA_INVALID_PROPERTY_TYPE);
  REGISTER(SCHEMA_PROPERTY_MISUSE);
  REGISTER(SCHEMA_INVALID_NODE_TYPE);

  REGISTER(MODEL_NOT_A_MODEL);
  REGISTER(MODEL_ID_CONFLICT);
}

std::string AppError::ErrorStrings::StrError(Status status) {
  return (_error_strings.find(status) != _error_strings.end()) ? _error_strings[status] : "UNKNOWN";
}

std::string AppError::StrError(Status status) {
  return error_strings.StrError(status);
}

AppError::AppError(Status status, const std::string& message)
    : _status(status) {
  _message = StrError(status) + ": " + message;
}


}  // namespace analog3
