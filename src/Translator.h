#ifndef _TRANSLATOR_H_
#define _TRANSLATOR_H_

#include <string>
#include <vector>

class Translator {
 public:
  Translator(std::string source_lang, std::string target_lang) : source_lang_(std::move(source_lang)), target_lang_(std::move(target_lang)) { }
  virtual ~Translator() { }
  
  virtual std::string translate(const std::string & input) = 0;
  virtual std::vector<std::string> translate(const std::vector<std::string> & input) {
    std::vector<std::string> r;
    for (auto & s : input) {
      r.push_back(translate(s));
    }
    return r;
  }

  const std::string & getSourceLang() const { return source_lang_; }
  const std::string & getTargetLang() const { return target_lang_; }
  
private:
  std::string source_lang_, target_lang_;
};

class PassthroughTranslator : public Translator {
public:
  PassthroughTranslator() : Translator("", "") { }

  std::string translate(const std::string & input) override { return input; }
};

class NullTranslator : public Translator {
public:
  NullTranslator() : Translator("", "") { }

  std::string translate(const std::string & input) override { return std::string(); }
};

#endif
