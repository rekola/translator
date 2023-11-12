#ifndef _MARIANTRANSLATOR_H_
#define _MARIANTRANSLATOR_H_

#include "Translator.h"

#include "marian.h"                                                                           
#include "translator/beam_search.h"                                                           
#include "translator/translator.h"
#include "sentencepiece/src/sentencepiece_processor.h"

#include "Tokenizer.h"

#include <memory>

class MarianTranslator : public Translator {
 public:
  MarianTranslator(std::string source_lang, std::string target_lang);

  std::string translate(const std::string & input) override;
  std::vector<std::string> translate(const std::vector<std::string> & input);

protected:
  void initialize();

  std::vector<std::string> translateBatch(const std::vector<std::string> & sentences);

  std::string encodeSentencePiece(const std::string & input);
  std::string decodeSentencePiece(const std::string & input);
  
private:
  marian::Ptr<marian::TranslateService<marian::BeamSearch>> task_;
  std::unique_ptr<sentencepiece::SentencePieceProcessor> source_spm_;
  // std::unique_ptr<sentencepiece::SentencePieceProcessor> target_spm_;

  bool is_initialized_ = false;
  Tokenizer tokenizer_;
};

#endif
