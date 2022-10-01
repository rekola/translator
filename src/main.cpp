// #define CPPHTTPLIB_OPENSSL_SUPPORT

#include "TranslationContext.h"

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
    
  svr.Get("/translate", [](const httplib::Request & req, httplib::Response & res) {
    string q, source, target, format;
    if (req.has_param("q")) q = req.get_param_value("q");
    if (req.has_param("source")) source = req.get_param_value("source");
    if (req.has_param("target")) target = req.get_param_value("target");
    if (req.has_param("format")) format = req.get_param_value("format");
    
    auto & translator = TranslationContext::getInstance();
    auto output = translator.translate(source, target, q);
    
    cerr << "translated (" << source << " => " << target << "): \"" << q << "\" => \"" << output << "\"\n";
    
    json translation;
    translation["translatedText"] = output;
    
    json translations;
    translations.push_back(translation);
    
    json payload;
    payload["data"]["translations"] = translations;
    
    res.set_content(payload.dump(), "application/json");
  });

  svr.Get("/index.html", [](const httplib::Request & req, httplib::Response & res) {
    res.set_content("Nothing here.", "text/plain");
  });

  svr.listen("localhost", 8080);

  return 0;
}
