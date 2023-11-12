#ifndef _LANGUAGEDETECTOR_H_
#define _LANGUAGEDETECTOR_H_

#include "Tokenizer.h"

#include <string>
#include <memory>
#include <mutex>

namespace fasttext { class FastText; };

class LanguageDetector {
 public:
  static LanguageDetector & getInstance();

  std::string detectLanguage(std::string_view text) noexcept;

 private:
  LanguageDetector() {
    tokenizer_.skipWhitespace(true);
    tokenizer_.normalizeWords(false);
    tokenizer_.stripEndings(false);
    tokenizer_.setLinkMode(Tokenizer::STRIP);
  }
  ~LanguageDetector() = delete;

  std::shared_ptr<fasttext::FastText> language_model_;
  Tokenizer tokenizer_;
  
  mutable std::mutex mutex_;

  static LanguageDetector * instance;
};

#endif
