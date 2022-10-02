#ifndef _LANGUAGEDETECTOR_H_
#define _LANGUAGEDETECTOR_H_

#include <language/Tokenizer.h>

#include <string>
#include <memory>
#include <mutex>

namespace fasttext { class FastText; };

class LanguageDetector {
 public:
  static LanguageDetector & getInstance();

  std::string detectLanguage(const std::string & text);

 private:
  LanguageDetector() : tokenizer_en_("en") {
    tokenizer_en_.skipWhitespace(true);
    tokenizer_en_.normalizeWords(false);
    tokenizer_en_.stripEndings(false);
    tokenizer_en_.setLinkMode(Tokenizer::STRIP);
  }
  ~LanguageDetector() = delete;

  std::shared_ptr<fasttext::FastText> language_model_;
  Tokenizer tokenizer_en_;
  
  mutable std::mutex mutex_;

  static LanguageDetector * instance;
};

#endif
