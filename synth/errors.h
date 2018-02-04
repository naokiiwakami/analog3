#ifndef SYNTH_ERRORS_H_
#define SYNTH_ERRORS_H_

#include <map>
#include <string>

namespace analog3 {

enum class StatusCategory {
  COMMON = 0,
  SCHEMA,
  MODEL,
};

enum class Status {
  OK = 0,
  FILE_NOT_FOUND,

  SCHEMA_NOT_AN_OBJECT  = (static_cast<int>(StatusCategory::SCHEMA) << 16) + 1,
  SCHEMA_REQUIRED_PROPERTY_MISSING,
  SCHEMA_INVALID_PROPERTY_TYPE,
  SCHEMA_PROPERTY_MISUSE,
  SCHEMA_INVALID_NODE_TYPE,

  MODEL_NOT_A_MODEL = (static_cast<int>(StatusCategory::MODEL) << 16) + 1,
  MODEL_ID_CONFLICT,
};

class AppError {
 public:
  explicit AppError(Status status)
      : _status(status)
  {}

  explicit AppError(Status status, const std::string& message);

  std::string StrError(Status status);

  Status GetStatus() const { return _status; }

  const std::string& GetMessage() const { return _message; }

 private:
  Status _status;
  std::string _message;

  class ErrorStrings {
   public:
    ErrorStrings();
    std::string StrError(Status status);
   private:
    std::map<Status, std::string> _error_strings;
  };

  static ErrorStrings error_strings;
};

}  // namespace analog3
#endif  // SYNTH_ERRORS_H_
