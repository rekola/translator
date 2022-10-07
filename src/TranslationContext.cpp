#include "TranslationContext.h"

#include "MarianTranslator.h"

std::mutex TranslationContext::instance_mutex;
std::atomic<TranslationContext *> TranslationContext::instance{nullptr};

TranslationContext::TranslationContext() {
  addTranslator(std::make_unique<MarianTranslator>("en", "fi"));
  addTranslator(std::make_unique<MarianTranslator>("fi", "en"));
  addTranslator(std::make_unique<MarianTranslator>("sv", "en"));
  addTranslator(std::make_unique<MarianTranslator>("et", "en"));
  addTranslator(std::make_unique<MarianTranslator>("ru", "en"));
  addTranslator(std::make_unique<MarianTranslator>("de", "en"));
  addTranslator(std::make_unique<MarianTranslator>("uk", "en"));
}
