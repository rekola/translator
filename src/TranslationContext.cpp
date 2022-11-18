#include "TranslationContext.h"

#include "MarianTranslator.h"

TranslationContext::TranslationContext() {
  addTranslator(std::make_unique<MarianTranslator>("en", "fi"));

  addTranslator(std::make_unique<MarianTranslator>("fi", "ru"));
  
  addTranslator(std::make_unique<MarianTranslator>("fi", "en"));
  addTranslator(std::make_unique<MarianTranslator>("sv", "en"));
  addTranslator(std::make_unique<MarianTranslator>("et", "en"));
  addTranslator(std::make_unique<MarianTranslator>("ru", "en"));
  addTranslator(std::make_unique<MarianTranslator>("de", "en"));
  addTranslator(std::make_unique<MarianTranslator>("uk", "en"));
}
