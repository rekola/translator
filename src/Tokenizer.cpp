#include "Tokenizer.h"

#include "../text_utils.h"

using namespace std;

static inline bool is_alpha_unicode(uint32_t cp) {
  auto cat = utf8proc_category(cp);
  return cat == UTF8PROC_CATEGORY_LU || cat == UTF8PROC_CATEGORY_LL || cat == UTF8PROC_CATEGORY_LT || cat == UTF8PROC_CATEGORY_LM || cat == UTF8PROC_CATEGORY_LO || cat == UTF8PROC_CATEGORY_ND;
}

static inline bool is_alnum_unicode(uint32_t cp) {
  auto cat = utf8proc_category(cp);
  return cat == UTF8PROC_CATEGORY_LU || cat == UTF8PROC_CATEGORY_LL || cat == UTF8PROC_CATEGORY_LT || cat == UTF8PROC_CATEGORY_LM || cat == UTF8PROC_CATEGORY_LO || cat == UTF8PROC_CATEGORY_ND;
}

static inline bool is_complex_symbol_unicode(uint32_t cp) {
  if ((cp >= 0x02B00 && cp <= 0x02BFF) || // Miscellaneous Symbols and Arrows
      (cp >= 0x025A0 && cp <= 0x025FF) || // Geometric Shapes
      cp == 10084) { // LINEAR B IDEOGRAM B105F MARE
    return true;
  } else if (cp >= 0x2190 && cp <= 0x21FF) { // Arrows
    return true;
  } else if (cp >= 0x2500 && cp <= 0x257f) { // Box drawing
    return true;
  } else if (cp >= 0x2580 && cp <= 0x259F) { // Block Elements
    return true;
  } else if (cp >= 0x2600 && cp <= 0x26ff) { // Miscellaneous Symbols
    return true;      
  } else if (cp >= 0x2700 && cp <= 0x27BF) { // Dingbats
    return true;
  } else if (cp >= 0x1F600 && cp <= 0x1F64F) { // Emoticons
    return true;
  } else if (cp >= 0x1F300 && cp <= 0x1F5FF) { // Miscellaneous Symbols and Pictograms
    return true;
  } else if (cp >= 0x1F680 && cp <= 0x1F6FF) { // Transport And Map Symbols
    return true;
  } else if (cp >= 0x1F900 && cp <= 0x1F9FF) { // Supplemental Symbols and Pictographs
    return true;
  } else if (cp >= 0x2300 && cp <= 0x23ff) { // Miscellaneous Technical
    return true;
  } else if (cp >= 0x3000 && cp <= 0x303F) { // CJK Symbols and Punctuation
    return true;
  } else if (cp > 0xffff) { // temporary
    return true;
  } else {
    return false;
  }
}

static inline bool is_syllabary_unicode(uint32_t cp) {
  if ((cp >= 0x3040 && cp <= 0x309f) || // Hiragana
      (cp >= 0x30a0 && cp <= 0x30ff) || // Katakana
      (cp >= 0x31f0 && cp <= 0x31ff) || // Katakana phonetic extensions
      (cp >= 0xAC00 && cp <= 0xD7AF) || // Hangul Syllables
      (cp >= 0x1100 && cp <= 0x1112) || // Hangul jamos
      (cp >= 0x1161 && cp <= 0x1175) || // Hangul jamos
      (cp >= 0x11a8 && cp <= 0x11c2) // Hangul jamos      
      ) {
    return true;
  } else {
    return false;
  }
}

static inline bool is_ideograph_unicode(uint32_t cp) {
  if ((cp >= 0x4E00 && cp < 0x62FF) || // CJK Unified Ideographs
      (cp >= 0x6300 && cp < 0x77FF) || // CJK Unified Ideographs
      (cp >= 0x7800 && cp < 0x8CFF) || // 
      (cp >= 0x8D00 && cp < 0x9FFF) //
) {
    return true;
  } else {
    return false;
  }
}

static inline bool is_symbol_unicode(uint32_t cp) {
  if (cp == '$' || // dollar
      cp == 163 || // POUND SIGN
      cp == 174 || // REGISTERED SIGN
      cp == 215 || // MULTIPLICATION SIGN
      cp == 3647 || // THAI CURRENCY SYMBOL BAHT
      cp == 8364 || // EURO SIGN
      cp == 8467 || // letterlike symbol: script small l
      cp == 10068 || // White Question Mark Ornament 
      cp == 10071 || // Heavy Exclamation Mark Symbol      
      cp == 8419 // COMBINING ENCLOSING KEYCAP
      ) {
    return true;
  } else if (is_complex_symbol_unicode(cp)) {
    return true;
  } else if (cp >= 0x2000 && cp <= 0x206F) { // General punctuation
    return true;
  } else if (cp >= 0x3000 && cp <= 0x303F) { // CJK Symbols and Punctuation
    return true;
  } else if (cp >= 0xFF00 && cp <= 0xFFEF) { // Halfwidth and Fullwidth Forms Block 
    return true;
  } else {
    return false;
  }
}

static inline bool is_blank_unicode(uint32_t c ) {
  switch (c) {
  case ' ': return true; // space
  case '\t': return true; // tab
  case 8194: return true; // en space
  case 8195: return true; // em space
  case 8201: return true; // THIN SPACE
  case 160: return true; // nbsp
  case 0x3000: return true; // ideographic space
  default: return false;
  }
}

static inline bool is_control_unicode(uint32_t c) {
  return c == 0xad || // soft hyphen
    c == 0x200b || // ZERO WIDTH SPACE
    c == 0x200e || // LEFT-TO-RIGHT MARK
    c == 0x202c || // POP DIRECTIONAL FORMATTING
    c == 0x2003 ||
    c == 0x202c || // LEFT-TO-RIGHT MARK / POP DIRECTIONAL FORMATTING
    c == 0xfeff; // Zero width no-break space
}

static inline bool is_invalid_unicode(uint32_t c) {
  return c == 0xfefe || c == 0xfffe;
}

static inline bool is_space_unicode(uint32_t cp) { 
  return is_blank_unicode(cp) ||
    is_control_unicode(cp) ||
    cp == '\b' ||
    cp == 0x0a ||
    cp == 0x0d;    
}

static inline bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static inline bool is_number(const std::string & s) {
  if (s.empty()) return false;
  
  unsigned int i = 0;
  if (!is_digit(s[i]) && s[i] != '+' && s[i] != '-') {
    return false;
  }
  for (i++; s[i] != 0; i++) {
    if (!is_digit(s[i])) {
      return false;
    }
  }
  return true;
}

void
Tokenizer::initialize() {
  // for all languages:
  nonbreaking_prefixes_common_.insert("vs");
  nonbreaking_prefixes_common_.insert("etc");
  
  nonbreaking_prefixes_fi_.insert("ko");
  nonbreaking_prefixes_fi_.insert("esim");
  nonbreaking_prefixes_fi_.insert("n");
  nonbreaking_prefixes_fi_.insert("mm");
  nonbreaking_prefixes_fi_.insert("ns");
  nonbreaking_prefixes_fi_.insert("ps");
  nonbreaking_prefixes_fi_.insert("nro");
  nonbreaking_prefixes_fi_.insert("v");
  nonbreaking_prefixes_fi_.insert("synt");
  nonbreaking_prefixes_fi_.insert("jne");
  nonbreaking_prefixes_fi_.insert("ts");
  nonbreaking_prefixes_fi_.insert("em");
  nonbreaking_prefixes_fi_.insert("ym");
  nonbreaking_prefixes_fi_.insert("yms");
  nonbreaking_prefixes_fi_.insert("vrt");
  nonbreaking_prefixes_fi_.insert("kts");
  nonbreaking_prefixes_fi_.insert("tri");
    
  // usually upper case letters are initials in a name
  nonbreaking_prefixes_en_.insert("A");
  nonbreaking_prefixes_en_.insert("B");
  nonbreaking_prefixes_en_.insert("C");
  nonbreaking_prefixes_en_.insert("D");
  nonbreaking_prefixes_en_.insert("E");
  nonbreaking_prefixes_en_.insert("F");
  nonbreaking_prefixes_en_.insert("G");
  nonbreaking_prefixes_en_.insert("H");
  nonbreaking_prefixes_en_.insert("I");
  nonbreaking_prefixes_en_.insert("J");
  nonbreaking_prefixes_en_.insert("K");
  nonbreaking_prefixes_en_.insert("L");
  nonbreaking_prefixes_en_.insert("M");
  nonbreaking_prefixes_en_.insert("N");
  nonbreaking_prefixes_en_.insert("O");
  nonbreaking_prefixes_en_.insert("P");
  nonbreaking_prefixes_en_.insert("Q");
  nonbreaking_prefixes_en_.insert("R");
  nonbreaking_prefixes_en_.insert("S");
  nonbreaking_prefixes_en_.insert("T");
  nonbreaking_prefixes_en_.insert("U");
  nonbreaking_prefixes_en_.insert("V");
  nonbreaking_prefixes_en_.insert("W");
  nonbreaking_prefixes_en_.insert("X");
  nonbreaking_prefixes_en_.insert("Y");
  nonbreaking_prefixes_en_.insert("Z");
  
  // List of titles. These are often followed by upper-case names, but do not indicate sentence breaks
  nonbreaking_prefixes_en_.insert("Adj");
  nonbreaking_prefixes_en_.insert("Adm");
  nonbreaking_prefixes_en_.insert("Adv");
  nonbreaking_prefixes_en_.insert("Asst");
  nonbreaking_prefixes_en_.insert("Bart");
  nonbreaking_prefixes_en_.insert("Bldg");
  nonbreaking_prefixes_en_.insert("Brig");
  nonbreaking_prefixes_en_.insert("Bros");
  nonbreaking_prefixes_en_.insert("Capt");
  nonbreaking_prefixes_en_.insert("Cmdr");
  nonbreaking_prefixes_en_.insert("Col");
  nonbreaking_prefixes_en_.insert("Comdr");
  nonbreaking_prefixes_en_.insert("Con");
  nonbreaking_prefixes_en_.insert("Corp");
  nonbreaking_prefixes_en_.insert("Cpl");
  nonbreaking_prefixes_en_.insert("DR");
  nonbreaking_prefixes_en_.insert("Dr");
  nonbreaking_prefixes_en_.insert("Drs");
  nonbreaking_prefixes_en_.insert("Ens");
  nonbreaking_prefixes_en_.insert("Gen");
  nonbreaking_prefixes_en_.insert("Gov");
  nonbreaking_prefixes_en_.insert("Hon");
  nonbreaking_prefixes_en_.insert("Hr");
  nonbreaking_prefixes_en_.insert("Hosp");
  nonbreaking_prefixes_en_.insert("Insp");
  nonbreaking_prefixes_en_.insert("Lt");
  nonbreaking_prefixes_en_.insert("MM");
  nonbreaking_prefixes_en_.insert("MR");
  nonbreaking_prefixes_en_.insert("MRS");
  nonbreaking_prefixes_en_.insert("MS");
  nonbreaking_prefixes_en_.insert("Maj");
  nonbreaking_prefixes_en_.insert("Messrs");
  nonbreaking_prefixes_en_.insert("Mlle");
  nonbreaking_prefixes_en_.insert("Mme");
  nonbreaking_prefixes_en_.insert("Mr");
  nonbreaking_prefixes_en_.insert("Mrs");
  nonbreaking_prefixes_en_.insert("Ms");
  nonbreaking_prefixes_en_.insert("Msgr");
  nonbreaking_prefixes_en_.insert("Op");
  nonbreaking_prefixes_en_.insert("Ord");
  nonbreaking_prefixes_en_.insert("Pfc");
  nonbreaking_prefixes_en_.insert("Ph");
  nonbreaking_prefixes_en_.insert("Prof");
  nonbreaking_prefixes_en_.insert("Pvt");
  nonbreaking_prefixes_en_.insert("Rep");
  nonbreaking_prefixes_en_.insert("Reps");
  nonbreaking_prefixes_en_.insert("Res");
  nonbreaking_prefixes_en_.insert("Rev");
  nonbreaking_prefixes_en_.insert("Rt");
  nonbreaking_prefixes_en_.insert("Sen");
  nonbreaking_prefixes_en_.insert("Sens");
  nonbreaking_prefixes_en_.insert("Sfc");
  nonbreaking_prefixes_en_.insert("Sgt");
  nonbreaking_prefixes_en_.insert("Sr");
  nonbreaking_prefixes_en_.insert("St");
  nonbreaking_prefixes_en_.insert("Supt");
  nonbreaking_prefixes_en_.insert("Surg");

  // odd period-ending items that NEVER indicate breaks (p.m. does NOT fall into this category - it sometimes ends a sentence)

  nonbreaking_prefixes_en_.insert("v");
  nonbreaking_prefixes_en_.insert("i.e");
  nonbreaking_prefixes_en_.insert("rev");
  nonbreaking_prefixes_en_.insert("e.g");

  // These only when followed by number
#if 0			     
  nonbreaking_prefixes_en_.insert("No #NUMERIC_ONLY#");
  nonbreaking_prefixes_en_.insert("Nos");
  nonbreaking_prefixes_en_.insert("Art #NUMERIC_ONLY#");
  nonbreaking_prefixes_en_.insert("Nr");
  nonbreaking_prefixes_en_.insert("pp #NUMERIC_ONLY#");
#endif
}

std::string
Tokenizer::filterWord(const std::string & input) const {
  string lcit = utf8_lowercase(input);

//  glasses
// 8-) 8) B-) B)
//  sunglasses
// 8-| 8| B-| B|
//  cry
// :'(
//  devil
// 3:) 3:-)
//  angel
// O:) O:-)
//  kiss
// :-* :*
//  confused
// o.O O.o
//  pacman
// :v
//  curly lips
// :3
//  robot
// :|]
//  Chris Putnam
// :putnam:
//  Shark
// (^^^)
//  Penguin
// <(“)
//  42
// :42:
//  Thumbs up

  if (link_mode_ != KEEP) {
    if (lcit.compare(0, 7, "http:…") == 0 ||
	lcit.compare(0, 8, "https:…") == 0 ||
	lcit.compare(0, 6, "http…") == 0 ||
	lcit.compare(0, 7, "https…") == 0 ||
	lcit == "~link" || // Dai-Labor data
	lcit == "http:/" ||
	lcit == "https:/"
	) {
      return "[link]";
    } else if (lcit.compare(0, 4, "www.") == 0) {
      return "[link]";
    } else if (lcit.compare(0, 6, "http:/") == 0 ||
	       lcit.compare(0, 7, "https:/") == 0
	       ) {
      return "[link]";
    }
  }

  if (strip_numbers_ && !input.empty()) {
    if (is_number(input)) {
      return "[number]";
    } else if (input[input.size() - 1] == '.' &&
	       is_number(input.substr(0, input.size() - 1))) {
      return "[ordinal]";
    }
  }

  if (normalize_) {
    if (input == ":)" || input == ":-)" || input == "=)" ||
	input == "(:" || input == "(-:" || input == "(=" ||
	input == ":]" || input == "[:" ||
	input == ":o)" || input == "☺" ||
	input == "😘" ||
	input == "😍" ||
	input == "😊") {
      return "[smile]";
    } else if (input == ":))" || input == ":)))" || input == ":))))" || input == ":)))))" || input == "=))" || input == ":-))))") {
      return "[SMILE]";
    } else if (lcit == ":s") {
      return "[annoyed]";
    } else if (input == ":(" || input == ":-(" || input == "):" || input == ")-:" || input == ":[" || input == "]:" || input == "=(" || input == "☹") {
      return "[frown]";
    } else if (input == ":((" || input == ":(((" || input == ":((((") {
      return "[FROWN]";
    } else if (input == ":´(" || input == ":´´(" || input == ":`(" || input == ":'(") {
      return "[tear]";
    } else if (input == ":3") {
      return "[curly-lips]";
    } else if (input == "D:") {
      return "[frown2]";
    } else if (input == ";(" || input == ";|") {
      return "[frown3]";
    } else if (input == ";((" || input == ";(((") {
      return "[FROWN3]";
      // is_frown = true;
    } else if (lcit == ":-p" || lcit == ":p" || lcit == "=p") {
      return "[tongue]";
    } else if (lcit == ";-p" || lcit == ";p") {
      return "[tongue2]";
      // is_smile = true;
    } else if (lcit == "^_^") {
      return "[kiki]";
    } else if (input == "-_-") {
      return "[squint]";
    } else if (lcit == "(y)" || input == "👍") {
      return "[thumb-up]";
    } else if (lcit == "(n)") {
      return "[thumb-down]";
    } else if (lcit == "x)" || lcit == "xd") {
      return "[x-smile]";
    } else if (lcit == "^" || lcit == "^^" || lcit == "^^^" || lcit == "^^^^") {
      return "[agreement]";
    } else if (lcit == "…" || lcit == ".." || lcit == "...") {
      return "[ellipsis]";
    } else if (input == ":D" || input == ":-D" || input == "=D" || input == ":'D" || input == "😂") {
      return "[grin]";
    } else if (input == ":DD" || input == ":DDD" || input == ":DDDD" || input == ":DDDDD" ||
	       input == ":-DD" || input == ":-DDD" || input == ":-DDDD" || input == ":-DDDDD" ||
	       input == ":'DD" || input == "=DD") {
      return "[GRIN]";
      // strength = 2;
      // is_smile = true;
    } else if (input == ";)" || input == ";-)") {
      return "[wink]";
    } else if (input == ";D" || input == ";-D") {
      return "[wink2]";
    } else if (lcit == ":o" || lcit == ":-o" || input == ":о" || lcit == "=o") {
      return "[gasp]";
    } else if (lcit == ":oo" || lcit == ":ooo") {
      return "[GASP]";
    } else if (lcit == ">:o" || lcit == ">:-o") {
      return "[upset]";
    } else if (input == ">:(" || input == ">:-(") {
      return "[grumpy]";
    } else if (lcit == "*o*") {
      return "[amazement2]";
    } else if (lcit == "o_o" || lcit == "o__o" || lcit == "o.o") {
      return "[confused]";
    } else if (input == ":/" || input == ":-/" || input == ":\\" ||
	       input == ":-\\" || input == "=/") {
      return "[unsure]";
    } else if (input == "-,-") {
      return "[face]";
    } else if (input == "\\o/") {
      return "[cheer]";
    } else if (input == ":>" || input == "<:") {
      return "[smug]";
    } else if (lcit == "%link%") {
      return ""; // ?
    } else if (input == "**" || input == "***" || input == "****" || input == "*****" ||
	       input == "******" || input == "*******") {
      return "[censored]";
    } else if (lcit == "fuck" || lcit == "f**k") {
      return "fuck";
    } else if (lcit == "paska" || lcit == "p**a" || lcit == "p***a") {
      return "paska";
    } else if (lcit == "vittu" || lcit == "v*tu") {
      return "vittu";
    } else if (input == "<3" || input == "♥" || input == "❤" || input == "♡") {
      return "[heart]";
      // is_smile = true;
    } else if (input == "<33" || input == "<333" || input == "<3333") {
      return "[HEART]";
    } else if (input == "t&j" || input == "tj") {
      return "[liked/shared]";
    }
  }

  if (input == "\"" || input == "”" || input == "“") {
    return "\"";
  } else if (input == "–" || input == "—" || input == "-") {
    return "-";
  }

  return input;
}

#define STATE_WHITESPACE	1
#define STATE_WORD		2
#define STATE_SQ_IN_WORD	3
#define STATE_COLON_AFTER_WORD	4
#define STATE_SMILEY_EYES	5
#define STATE_SMILEY_NOSE	6
#define STATE_SMILEY_NOSE_MOUTH	7
#define STATE_SMILEY_MOUTH	8
#define STATE_TAG_START		9
#define STATE_REV_SMILEY_MOUTH	10
#define STATE_BRACKET_START	11
#define STATE_X_SMILEY		12
#define STATE_FIRST_CARET	13
#define STATE_CARET		14
#define STATE_EYEBROWS		15
#define STATE_REV_D_SMILEY_MOUTH 16
#define STATE_MENTION		17 // or hashtag

#define STATE_H	100
#define STATE_HT 101
#define STATE_HTT 102
#define STATE_HTTP 103
#define STATE_HTTPS 104
#define STATE_HTTP_COLON 105
#define STATE_HTTP_BODY 106

#define STATE_ELLIPSIS_START 200
#define STATE_ELLIPSIS 201

#define STATE_NUMBER 300

#define STATE_W 400
#define STATE_WW 401
#define STATE_WWW 402
#define STATE_WWWDOT 403

#define STATE_TAG		500

static inline bool isRedundant(uint32_t c) {
  auto cat = utf8proc_category(c);
  return cat == UTF8PROC_CATEGORY_CC ||
    cat == UTF8PROC_CATEGORY_CF ||
    cat == UTF8PROC_CATEGORY_ZS ||
    cat == UTF8PROC_CATEGORY_ZL ||
    cat == UTF8PROC_CATEGORY_ZP ||
    is_invalid_unicode(c) ||
    c == '|' ||
    c == '\'' ||
    c == 180 || // acute accent
    c == 8217 || // RIGHT SINGLE QUOTATION MARK
    c == 0xfe0f; // ??
}

static inline bool isIntraWordPunctuation(uint32_t c) {
  return c == '+' || c == '-' || c == '*' || c == '_';
}

static inline bool isSingular(uint32_t c) {
  return c == '!' || c == '?' || c == '(' || c == ')' ||
    c == '[' || c == ']' || c == '{' || c == '}' || c == '\"' || c == '\'' ||
    c == '<' || c == '>' ||
    c == ',' || 
    c == 0xAB || // Left-Pointing Double Angle Quotation Mark
    c == 0xBB || // Right-Pointing Double Angle Quotation Mark
    c == 0x203C || // double exclamation mark
    c == 0x2022 || // Bullet
    c == 0xa8 || // Diaresis
    c == 161 || // reversed bang
    c == 8364 || // EURO SIGN
    c == 8212 || // Em Dash
    c == 96 || // Backquote
    c == 145 || // left single quotation mark
    c == 146 || // some quote
    c == 147 || // some quote
    c == 148 || // some quote
    c == 180 || // acute accent
    c == '$' || c == '&' || c == '%' ||
    c == ';' || c == ':' || c == '=' ||
    c == '/' ||
    c == 8230 || // ellipsis
    c == 8220 || c == 8221 || // double quotation marks
    c == 8216 || // single quote (left?)
    c == 8217 || // single right quotation mark
    (c >= 0x1f10b && c <= 0x1F14D) || // circled letters and numbers
    is_complex_symbol_unicode(c);
}

static inline bool isBlank(const std::string & s) {
  for (unsigned int i = 0; i < s.size(); i++) {
    if (!is_space_unicode(s[i])) return false;
  }
  return true;
}

static inline void append_utf8(char32_t c, std::string & s) {
  char tmp[4];
  auto r = utf8proc_encode_char(c, reinterpret_cast<utf8proc_uint8_t*>(&tmp[0]));
  s += std::string(tmp, r);
}
			       
vector<string>
Tokenizer::tokenize(std::string_view lang, std::string_view input) const {
  vector<string> tokens;
  int state = STATE_WHITESPACE;
  string current_token;

  const char * str_it = input.data();
  const char * end = input.data() + input.size();

  uint32_t prev_c = 0;
  while ( str_it < end ) {
    auto prior = str_it;
    // get 32 bit code of a utf-8 symbol
    char32_t c;
    auto r = utf8proc_iterate(reinterpret_cast<const utf8proc_uint8_t*>(str_it), end - str_it, reinterpret_cast<utf8proc_int32_t*>(&c));
    if (r <= 0 || !c) break;
    str_it += r;
    
    switch (state) {
    case STATE_WHITESPACE:
      if (is_control_unicode(c) || is_invalid_unicode(c)) {
	// ignore
      } else if (is_space_unicode(c)) {
	append_utf8(c, current_token);
      } else if (c == '^') {
	if (!current_token.empty()) tokens.push_back(current_token);
	current_token = "^";
	state = STATE_FIRST_CARET;
      } else if (c == '<') {
	if (!current_token.empty()) tokens.push_back(current_token);
	current_token = '<';
	state = STATE_TAG_START;
      } else if (c == '(') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	state = STATE_BRACKET_START;
      } else if (c == ':' || c == ';' || c == '=') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	state = STATE_SMILEY_EYES;
      } else if (c == 'x' || c == 'X') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	state = STATE_X_SMILEY;
      } else if (c == '>') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	state = STATE_EYEBROWS;
      } else if (c == ')' || c == '[' || c == ']') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	state = STATE_REV_SMILEY_MOUTH;
      } else if (c == 'D') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	state = STATE_REV_D_SMILEY_MOUTH;
      } else if (c == '.') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	state = STATE_ELLIPSIS_START;
      } else if (isSingular(c) || isIntraWordPunctuation(c)) {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	tokens.push_back(std::move(current_token));
	current_token.clear();
      } else if (c == 'h' || c == 'H') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	state = STATE_H;
      } else if (c == 'w' || c == 'W') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	state = STATE_W;
      } else if (c == '@' || c == '#') {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	state = STATE_MENTION;
      } else {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	append_utf8(c, current_token);
	state = STATE_WORD;
      }
      break;
    case STATE_MENTION:
      if (is_control_unicode(c) || is_invalid_unicode(c)) {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
      } else if (c == ':') {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_COLON_AFTER_WORD;
      } else if (c == '\'' || c == '|') { // keep endings (such as #puolue'iden)
	append_utf8(c, current_token);
      } else if (isRedundant(c) || isSingular(c) || c == '.' || c == '(' || c == '\\' || c == '#' || c == '-' || c == '@' || c == '~') {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      } else {
	append_utf8(c, current_token);
      }
      break;
    case STATE_WORD:
      if (is_control_unicode(c) || is_invalid_unicode(c)) {
	// skip
      } else if (c == '\'') {
	state = STATE_SQ_IN_WORD;
      } else if (c == ':') {
	state = STATE_COLON_AFTER_WORD;
      } else if (c == '*') {
#if 1
	append_utf8(c, current_token);
#else
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
#endif
      } else if (isSingular(c) || c == '.' || c == '(' || c == '\\' || c == '#' || c == '@' || c == '~') {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      } else if (c == '&') {
	append_utf8(c, current_token);
      } else if (c == '^') {
	tokens.push_back(current_token);
	current_token = "^";
	state = STATE_FIRST_CARET;
      } else if (isRedundant(c)) {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      } else {
	append_utf8(c, current_token);
      }
      break;
    case STATE_SQ_IN_WORD: // Tark'ampuja
      if (is_alpha_unicode(c)) {
	current_token += '\'';
	state = STATE_WORD;
	str_it = prior;
      } else {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	tokens.push_back("\'");
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_COLON_AFTER_WORD:
      if (is_space_unicode(c) || (c >= '0' && c <= '9') || isSingular(c)) {
	if (!current_token.empty()) {
	  tokens.push_back(std::move(current_token));
	  current_token.clear();
	}
	tokens.push_back(":");
	state = STATE_WHITESPACE;
	str_it = prior;
      } else if (c == '-' || c == 96 || c == ')' || c == '|' || c == '(' || c == 'D' || c == 'p' || c == 'P' || c == '>' || c == 'O' || c == 'S' || c == '/' || c == '\\' || c == '[' || c == ']' || c == '3') {
	if (!current_token.empty()) {
	  tokens.push_back(current_token);
	}
	current_token = ':';
	state = STATE_SMILEY_EYES;
	str_it = prior;
      } else if (c == 'h' || c == 'H') {
	current_token += ':';
	tokens.push_back(std::move(current_token));
	current_token.clear();
	append_utf8(c, current_token);
	state = STATE_H;
      } else if (c == 'w' || c == 'W') {
	current_token += ':';
	tokens.push_back(std::move(current_token));
	current_token.clear();
	append_utf8(c, current_token);
	state = STATE_W;
      } else {
	current_token += ':';
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_SMILEY_EYES:
      if (c == '-' ||
	  c == '\'' ||
	  c == 96 || // grave accent (tear)
	  c == 180 // acute accent (tear)
	  ) {
	append_utf8(c, current_token);
	state = STATE_SMILEY_NOSE;
      } else if (c == 'o') {
	append_utf8(c, current_token);
	state = STATE_SMILEY_NOSE_MOUTH;
      } else if (c == ')' || c == '(' || c == '|' || c == 'D' || c == 'p' || c == 'P' || c == '>' || c == 'O' || c == 's' || c == 'S' || c == '/' || c == '\\' || c == '[' || c == ']' || c == '3') {
	append_utf8(c, current_token);
	state = STATE_SMILEY_MOUTH;
      } else {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_SMILEY_NOSE:
      if (c == '-' ||
	  c == '\'' ||
	  c == 96 || // grave accent (tear)
	  c == 180 // acute accent (tear)
	  ) {
	// skip long nose or tear
      } else if (c == ')' || c == '(' || c == '|' || c == 'D' || c == 'p' || c == 'P' || c == '>' || c == 'o' || c == 'O' || c == 's' || c == '/') {
	append_utf8(c, current_token);
	state = STATE_SMILEY_MOUTH;
      } else {
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_SMILEY_NOSE_MOUTH:
      if (c == ')' || c == '(' || c == 'o') {
	append_utf8(c, current_token);
	state = STATE_SMILEY_MOUTH;
      } else {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_SMILEY_MOUTH:
      if (c == prev_c) {
	append_utf8(c, current_token);
      } else {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_TAG_START:
      if (c == '3' || c == ':' || c == '=') {
	append_utf8(c, current_token);
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
      } else if (c == '/' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
	append_utf8(c, current_token);
	state = STATE_TAG;
      } else {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_REV_SMILEY_MOUTH:
      if (c == ':' || c == '=') {
	append_utf8(c, current_token);
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
      } else {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_W:
      if (c == 'w' || c == 'W') {
	append_utf8(c, current_token);
	state = STATE_WW;
      } else {
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_WW:
      if (c == 'w' || c == 'W') {
	append_utf8(c, current_token);
	state = STATE_WWW;
      } else {
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_WWW:
      if (c == '.') {
	append_utf8(c, current_token);
	state = STATE_WWWDOT;
      } else {
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_WWWDOT:
      if (c != '/' && isRedundant(c)) {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      } else if (c != '/' && c != '&' && c != '%' && c != '=' &&
		 (c == '(' || c == '\\' || c == '@' || isSingular(c))) {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      } else {
	append_utf8(c, current_token);
      }
      break;
    case STATE_H:
      if (c == 't' || c == 'T') {
	append_utf8(c, current_token);
	state = STATE_HT;
      } else {
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_HT:
      if (c == 't' || c == 'T') {
	append_utf8(c, current_token);
	state = STATE_HTT;
      } else {
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_HTT:
      if (c == 'p' || c == 'P') {
	append_utf8(c, current_token);
	state = STATE_HTTP;
      } else {
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_HTTP:
      if (c == 's' || c == 'S') {
	append_utf8(c, current_token);
	state = STATE_HTTPS;
	break;
      }
      // pass
    case STATE_HTTPS:
      if (c == ':') {
	append_utf8(c, current_token);
	state = STATE_HTTP_COLON;
      } else if (c == 8230) { // ellipsis
	append_utf8(c, current_token);
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
      } else {
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_HTTP_COLON:
      if (c == '/') {
	append_utf8(c, current_token);
	state = STATE_HTTP_BODY;
      } else if (c == 8230) { // ellipsis
	append_utf8(c, current_token);
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
      } else {
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_HTTP_BODY:
      if (c == ')' || c == ']' || c =='>' || (c != '/' && isRedundant(c))) {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      } else {
	append_utf8(c, current_token);
      }
      break;
    case STATE_ELLIPSIS_START:
      if (c == '.') {
	state = STATE_ELLIPSIS;
      } else {
	tokens.push_back(".");
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_ELLIPSIS:
      if (c == '.') {
	// nothing
      } else {
	tokens.push_back("...");
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_BRACKET_START:
      if (c == ')') {
	tokens.push_back("()");
	current_token.clear();
	state = STATE_WHITESPACE;
      } else if (c == ':' || c == '=') {
	current_token += '(';
	append_utf8(c, current_token);
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
      } else {
	tokens.push_back("(");
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_X_SMILEY:
      if (c == ')') {
	append_utf8(c, current_token);
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	break;
      } else {
	state = STATE_WORD;
	str_it = prior;
      }
      break;
    case STATE_FIRST_CARET:
      append_utf8(c, current_token);
      if (c == '^' || c == '_' || c == '-') {
	state = STATE_CARET;
      } else {
	state = STATE_WORD;
      }
      break;
    case STATE_CARET:
      if (c == '^' || c == '_' || c == '-') {
	append_utf8(c, current_token);
      } else {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_EYEBROWS:
      if (c == ':' || c == ';') {
	append_utf8(c, current_token);
	state = STATE_SMILEY_EYES;
      } else {
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
	str_it = prior;
      }
      break;
    case STATE_REV_D_SMILEY_MOUTH:
      if (c == ':') {
	append_utf8(c, current_token);
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
      } else {
	append_utf8(c, current_token);
	state = STATE_WORD;
      }
      break;
    case STATE_TAG:
      append_utf8(c, current_token);
      if (c == '>' || is_space_unicode(c) || is_control_unicode(c) || is_invalid_unicode(c)) {
	// for now, only support parameterless tags
	tokens.push_back(std::move(current_token));
	current_token.clear();
	state = STATE_WHITESPACE;
      }
      break;
    }

    prev_c = c;
  }
  // cerr << "prev_c was " << prev_c << endl;

  if (state == STATE_SQ_IN_WORD) {
    tokens.push_back(current_token);
    tokens.push_back("'");
  } else if (state == STATE_COLON_AFTER_WORD) {
    tokens.push_back(current_token);
    tokens.push_back(":");
  } else if (state == STATE_ELLIPSIS_START) {
    tokens.push_back(".");
  } else if (state == STATE_ELLIPSIS) {
    tokens.push_back("...");
  } else if (state == STATE_BRACKET_START) {
    tokens.push_back("(");
  } else if (!current_token.empty()) {
    tokens.push_back(current_token);
  }

  vector<string> tokens2;
  for (auto & t : tokens) {
    if (t.empty()) {
      // cerr << "ERROR: empty token\n";
    } else if (isBlank(t)) {
      if (!skip_whitespace_) tokens2.push_back(" ");
    } else if (t == "." && !tokens2.empty() && (nonbreaking_prefixes_common_.count(tokens2.back()) ||
						getNonbreakingPrefixes(lang).count(tokens2.back()) ||
						is_number(tokens2.back()))) {
      tokens2.back() += t;
    } else if (0 &&
	       ((t == "&" && !tokens2.empty()) ||
		(!tokens2.empty() && tokens2.back()[tokens2.back().size() - 1] == '&'))) {
      tokens2.back() += t;
    } else if (!tokens2.empty() && tokens2.back() == ":" && t == ")") {
      tokens2.back() += t;
    } else if (!tokens2.empty() && tokens2.back() == "RT") {
      tokens2.back() += t;
    } else if (t.compare(0, 5, "http:") == 0 || t.compare(0, 6, "https:") == 0) {
      tokens2.push_back(t);
    } else {
      size_t pos = t.find_first_of("|':");
      if (pos != string::npos) {
	if (pos + 4 < t.size()) { // if suffix is long, it's actually another word
	  tokens2.push_back(t.substr(0, pos));
	  tokens2.push_back(t.substr(pos, 1));
	  tokens2.push_back(t.substr(pos + 1));
	} else if (strip_endings_ && (t[0] == '#' || t[0] == '@')) {
	  t.erase(pos);
	  tokens2.push_back(t);
	} else {
	  tokens2.push_back(t);
	}
      } else {
	while (!t.empty() && (t.back() == '*' || t.back() == '_')) {
	  t.erase(t.size() - 1);
	}
	tokens2.push_back(t);
      }
    }
  }

#if 0
  cerr << "tokenized " << input << " =>";
  for (auto & t : tokens2) {
    cerr << " " << t;
  }
  cerr << "\n";
#endif

  vector<string> tokens3;
  for (auto & t : tokens2) {
    auto token = filterWord(t);
    if (!token.empty()) tokens3.push_back(token);
  }

  return tokens3;
}
