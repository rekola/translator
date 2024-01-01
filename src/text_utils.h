// Various text processing utils

#ifndef _TRANSLATOR_TEXT_UTILS_H_
#define _TRANSLATOR_TEXT_UTILS_H_

#include <utf8proc.h>
#include <memory>

static inline std::string utf8_lowercase(std::string_view input) {
  if (!input.empty()) {
    utf8proc_uint8_t * dest = nullptr;
    auto options = utf8proc_option_t(UTF8PROC_IGNORE | UTF8PROC_STRIPCC | UTF8PROC_CASEFOLD | UTF8PROC_COMPOSE);
    auto s = utf8proc_map(reinterpret_cast<const unsigned char *>(input.data()),
			  static_cast<utf8proc_ssize_t>(input.size()),
			  &dest,
			  options
			  );
    if (s >= 0) {
      std::string r(reinterpret_cast<char *>(dest), s);
      free(dest);
      return r;
    } else {
      free(dest);
    }
  }

  return std::string();
}

static inline bool utf8_is_word(const std::string & input) {
  if (input.empty() || input[0] == '@' || input == "[link]") {
    return false;
  } else if (input.compare(0, 7, "http://") == 0 || input.compare(0, 8, "https://") == 0) {
    return false;
  } else {
    auto codepoints = std::make_unique<utf8proc_int32_t[]>(input.size());
    auto options = utf8proc_option_t(UTF8PROC_IGNORE | UTF8PROC_STRIPCC | UTF8PROC_COMPOSE);
    auto n_codepoints = utf8proc_decompose(reinterpret_cast<const unsigned char *>(input.c_str()), input.size(), codepoints.get(), input.size(), options);

    if (n_codepoints < 2) {
      return false;
    }
    
    for (size_t i = 0; i < static_cast<size_t>(n_codepoints); i++) {
      auto cat = utf8proc_category(codepoints[i]);
      if (cat == UTF8PROC_CATEGORY_LU || cat == UTF8PROC_CATEGORY_LL || cat == UTF8PROC_CATEGORY_LT || cat == UTF8PROC_CATEGORY_LM || cat == UTF8PROC_CATEGORY_LO) {
	return true;
      }	
    }
    
    return false;
  }
}        

#endif
