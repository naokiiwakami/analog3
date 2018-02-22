#include "server/finder.h"

#include <fcntl.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <iostream>
#include <list>

#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"
#include "server/module.h"
#include "server/node_builder.h"

namespace analog3 {

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("Finder"));

Finder::Finder(const std::string& dir_name)
    : m_dir_name(dir_name) {
}

Finder::~Finder() {
}

Status Finder::load() {
  std::list<std::string> dirs;
  dirs.push_back(m_dir_name + "/models");

  while (!dirs.empty()) {
    std::string current_dir = dirs.front();
    dirs.pop_front();

    DIR *dp = ::opendir(current_dir.c_str());
    if (dp == NULL) {
      LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(current_dir << "file_synth: no such directory"));
      return Status::FILE_NOT_FOUND;
    }

    struct dirent* ep;
    while ((ep = ::readdir(dp)) != NULL) {
      // ignore current, parent, and hidden directories
      if (ep->d_name[0] == '.') {
        continue;
      }
      if (ep->d_type == DT_DIR) {
        dirs.push_back(current_dir + "/" + ep->d_name);
      } else {
        char* ptr = strcasestr(ep->d_name, ".json");
        if (ptr == NULL || *(ptr + 5) != '\0') {
          continue;
        }
        std::string fileName = current_dir;
        fileName += "/";
        fileName += ep->d_name;
        LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("Loading file module " << fileName));

        try {
        rapidjson::Document* doc = MakeDocument(fileName);
        SynthNode *node;
        Status st = BuildComponent(doc, &node);
        if (st != Status::OK) {
          return st;
        }
        // SynthComponent* component = ComponentUtils::loadComponent(fileName);
        } catch (const AppError& error) {
          LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(error.GetMessage()));
          return error.GetStatus();
        }
      }
    }
  }
  return Status::OK;
}

rapidjson::Document* Finder::MakeDocument(const std::string& file_name) {
  int fd = ::open(file_name.c_str(), O_RDONLY);
  if (fd < 0) {
    throw AppError(Status::FILE_NOT_FOUND, "file_name=" + file_name);
  }
  struct stat file_info;
  ::fstat(fd, &file_info);

  // Read file and make Json data byte array
  char* buf = new char[file_info.st_size + 1];
  if (::read(fd, buf, file_info.st_size) < 0) {
    close(fd);
    delete[] buf;
    throw AppError(Status::FILE_READ_ERROR, "file_name=" + file_name);
  }
  buf[file_info.st_size] = '\0';
  close(fd);

  // Make the Json DOM document
  rapidjson::Document* document = new rapidjson::Document();
  document->Parse(buf);
  if (document->HasParseError()) {
    char error_text[4096];
    unsigned int offset = document->GetErrorOffset();
    std::string message;
    snprintf(error_text, sizeof(error_text), "Error (file %s, offset %u): %s\n",
             file_name.c_str(), offset,
             GetParseError_En(document->GetParseError()));
    message = error_text;
    size_t start = offset;
    while (start != 0 && buf[start] != '\n') {
      --start;
    }
    snprintf(error_text, sizeof(error_text), "%s\n", buf + start + 1);
    message += error_text;
    delete[] buf;
    delete document;
    throw AppError(Status::SCHEMA_PARSE_ERROR, message);
  }
  delete[] buf;

  return document;
}

Status Finder::BuildComponent(rapidjson::Document *doc, SynthNode **node) {
  try {
    NodeBuilder builder(doc);
    *node = builder.Build();
  } catch (const AppError& error) {
    // TODO(Naoki): Log it
    LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Failed to load model schema: " << error.GetMessage()));
    return error.GetStatus();
  }
  return Status::OK;
}

}  // namespace analog3
