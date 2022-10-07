#ifndef _TRANSLATIONCONTEXT_H_
#define _TRANSLATIONCONTEXT_H_

#include <memory>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <vector>

#include "Translator.h"

class TranslationContext {
 public:
  TranslationContext(const TranslationContext & other) = delete;
  TranslationContext(const TranslationContext && other) = delete;
  TranslationContext & operator=(const TranslationContext & other) = delete;
  TranslationContext & operator=(const TranslationContext && other) = delete;

  static TranslationContext & getInstance() {
    auto ptr = instance.load();
    if (ptr) return *ptr;
    else {      
      const std::lock_guard<std::mutex> lock(instance_mutex);
      auto ptr2 = instance.load();
      if (ptr2) return *ptr2;
      else {
	auto mgr = new TranslationContext();
	instance.store(mgr);
	return *mgr;
      }
    }
  }
  
  std::string translate(const std::string & source_lang, const std::string & target_lang, const std::string & input) {
    const std::lock_guard<std::mutex> lock(translation_mutex_);
    return getTranslator(source_lang, target_lang).translate(input);
  }

  std::vector<std::string> translate(const std::string & source_lang, const std::string & target_lang, const std::vector<std::string> & input) {
    const std::lock_guard<std::mutex> lock(translation_mutex_);
    return getTranslator(source_lang, target_lang).translate(input);
  }

 protected:
  Translator & getTranslator(const std::string & source, const std::string & target) {
    if (source == target) {
      return passthrough_translator_;
    } else {
      auto it = translators_.find(source);
      if (it != translators_.end()) {
	auto it2 = it->second.find(target);
	if (it2 != it->second.end()) {
	  return *(it2->second);
	}
      }
      return null_translator_;
    }
  }

 private:
  TranslationContext();

  void addTranslator(std::unique_ptr<Translator> translator) {
    translators_[translator->getSourceLang()][translator->getTargetLang()] = std::move(translator);
  }

  std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<Translator> > > translators_;
  PassthroughTranslator passthrough_translator_;
  NullTranslator null_translator_;
  std::mutex translation_mutex_;

  static std::mutex instance_mutex;
  static std::atomic<TranslationContext *> instance;
};

#endif
