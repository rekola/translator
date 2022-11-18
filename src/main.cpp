#define CPPHTTPLIB_FORM_URL_ENCODED_PAYLOAD_MAX_LENGTH (512*1024*1024)
// #define CPPHTTPLIB_OPENSSL_SUPPORT

#include "TranslationContext.h"

#include <language/LanguageDetector.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <httplib.h>
#include <nlohmann/json.hpp>

using namespace std;
using namespace nlohmann;

int main(int argc, char ** argv) {
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
  httplib::SSLServer svr;  
#else
  httplib::Server svr;
#endif

  svr.set_payload_max_length(1024 * 1024 * 512); // 512MB

  auto translate_handler = [](const httplib::Request & req, httplib::Response & res) {
    string q, source, target, format;
    if (req.has_param("source")) source = req.get_param_value("source");
    if (req.has_param("target")) target = req.get_param_value("target");
    if (req.has_param("format")) format = req.get_param_value("format");

    auto & detector = LanguageDetector::getInstance();

    auto num_input = req.get_param_value_count("q");
    std::unordered_map<std::string, std::vector<std::pair<std::string, size_t>>> input;
    for (size_t i = 0; i < num_input; i++) {
      auto text = req.get_param_value("q", i);
      auto lang = !source.empty() ? source : detector.detectLanguage(text);
      input[lang].push_back(pair(text, i));
    }

    std::cerr << "input size: " << num_input << "\n";

    std::vector<pair<string, string>> output;
    output.resize(num_input);
				 
    auto & translator = TranslationContext::getInstance();
    for (auto & [ lang, data ] : input) {
      std::vector<std::string> data2;
      for (auto & [s, pos] : data) data2.push_back(s);
      auto data3 = translator.translate(lang, target, data2);
      for (size_t i = 0; i < data.size(); i++) {
	auto & [s, pos] = data[i];
	std::string s2;
	if (i < data3.size()) {
	  s2 = std::move(data3[i]);
	}
	cerr << "translated (" << lang << " => " << target << "): \"" << s << "\" => \"" << s2 << "\"\n";
	output[pos] = pair(s2, lang);
      }
    }
		   
    json translations;
		   
    for (auto & [s, lang] : output) {
      json translation = json::object();
      // if (!orig.empty()) translation["originalText"] = orig;
      if (!s.empty()) translation["translatedText"] = s;
      if (source.empty() && !lang.empty()) translation["detectedSourceLanguage"] = lang;
		     
      translations.push_back(translation);
    }
		   
    json payload;
    payload["data"]["translations"] = translations;
		   
    res.set_content(payload.dump(), "application/json");
  };
  
  auto search_handler = [](const httplib::Request & req, httplib::Response & res) {
    res.set_content("Not implemented", "text/plain");
  };

  auto status_handler = [](const httplib::Request & req, httplib::Response & res) {
    res.set_content("Not implemented", "text/plain");    
  };

  svr.Get("/translate", translate_handler);
  svr.Post("/translate", translate_handler);
  svr.Get("/search", search_handler);
  svr.Get("/status", status_handler);

  svr.Get("/index.html", [](const httplib::Request & req, httplib::Response & res) {
    res.set_content("Nothing here.", "text/plain");
  });

  svr.listen("0.0.0.0", 8080);

  return 0;
}
