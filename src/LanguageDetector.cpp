#include "LanguageDetector.h"

#include <cassert>
#include <sstream>
#include <cstring>

#ifdef WIN32
#include <fasttext.h>
#else
#include <fasttext/fasttext.h>
#endif

#include <fmt/core.h>

#include "text_utils.h"

using namespace std;

LanguageDetector * LanguageDetector::instance = 0;

LanguageDetector &
LanguageDetector::getInstance() {
  if (!instance) instance = new LanguageDetector;
  return *instance;
}

std::string
LanguageDetector::detectLanguage(std::string_view text) noexcept {
  if (text.empty()) {
    return "";
  }

  try {
    const std::lock_guard<std::mutex> lock(mutex_);

    vector<string> words;
    auto tokens = tokenizer_.tokenize("", text);
    for (auto & token : tokens) {
      if (utf8_is_word(token) &&
	  token[0] != '#' &&
	  !(token[0] == '[' && token[token.size() - 1] == ']')) {      
	words.push_back(utf8_lowercase(token));
      }
    }

    if (!words.empty()) {
      if (!language_model_) {
	language_model_ = make_shared<fasttext::FastText>();
	language_model_->loadModel("assets/lid.176.ftz");
      }

      auto it = words.begin();
      auto s = *it++;
      while (it != words.end()) s += " " + *it++;
    
      std::stringstream ioss(s);
    
      std::vector<std::pair<fasttext::real, std::string>> predictions;
      language_model_->predictLine(ioss, predictions, 1, 0.0f);
    
      if (!predictions.empty()) {
	auto & prediction = predictions.front();
	assert(strncmp(prediction.second.c_str(), "__label__", 9) == 0); 
	return prediction.second.substr(9);
      }
    }
  } catch (exception & e) {
    fmt::print(stderr, "language detector failed with exception: {}\n", e.what());
  }
  
  return "";
}
