#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#include <string>
#include <vector>
#include <unordered_set>

class Tokenizer {
 public:
  enum LinkMode { KEEP = 1, STRIP };
  
  Tokenizer() {
    initialize();
  }
  
  std::vector<std::string> tokenize(std::string_view lang, std::string_view input) const;
  std::string filterWord(const std::string & input) const;
  
  void normalizeWords(bool t) { normalize_ = t; }
  void setLinkMode(LinkMode m) { link_mode_ = m; }
  void stripEndings(bool t) { strip_endings_ = t; }
  void skipWhitespace(bool t) { skip_whitespace_ = t; }
			
 protected:
  void initialize();

  const std::unordered_set<std::string> & getNonbreakingPrefixes(std::string_view lang) const {
    if (lang == "fi") return nonbreaking_prefixes_fi_;
    else if (lang == "en") return nonbreaking_prefixes_en_;
    else return nonbreaking_prefixes_empty_;
  }
  
 private:
  std::unordered_set<std::string> nonbreaking_prefixes_common_;
  std::unordered_set<std::string> nonbreaking_prefixes_fi_;
  std::unordered_set<std::string> nonbreaking_prefixes_en_;
    
  LinkMode link_mode_ = STRIP;
  bool strip_numbers_ = false;
  bool strip_endings_ = true;
  bool normalize_ = true;
  bool skip_whitespace_ = false;

  static inline std::unordered_set<std::string> nonbreaking_prefixes_empty_;
};

#endif
