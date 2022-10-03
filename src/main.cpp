// #define CPPHTTPLIB_OPENSSL_SUPPORT

#include "TranslationContext.h"

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

  auto handler = [](const httplib::Request & req, httplib::Response & res) {
		   string q, source, target, format;
		   if (req.has_param("source")) source = req.get_param_value("source");
		   if (req.has_param("target")) target = req.get_param_value("target");
		   if (req.has_param("format")) format = req.get_param_value("format");
		   
		   auto num_input = req.get_param_value_count("q");
		   std::vector<std::string> input;
		   input.reserve(num_input);
		   for (size_t i = 0; i < num_input; i++) input.push_back(req.get_param_value("q", i));
		   
		   auto & translator = TranslationContext::getInstance();
		   json translations;
		   
		   for (auto & s : input) {
		     auto output = translator.translate(source, target, s);
		     cerr << "translated (" << source << " => " << target << "): \"" << s << "\" => \"" << output << "\"\n";
		     
		     json translation;
		     translation["translatedText"] = output;
		     translations.push_back(translation);
		   }
		   
		   json payload;
		   payload["data"]["translations"] = translations;
		   
		   res.set_content(payload.dump(), "application/json");
		 };
  
  svr.Get("/translate", handler);
  svr.Post("/translate", handler);

  svr.Get("/index.html", [](const httplib::Request & req, httplib::Response & res) {
    res.set_content("Nothing here.", "text/plain");
  });

  svr.listen("0.0.0.0", 8080);

  return 0;
}
