#include "MarianTranslator.h"

#include <iostream>

using namespace std;
using namespace marian;    

MarianTranslator::MarianTranslator(std::string source_lang, std::string target_lang)
  : Translator(source_lang, target_lang) { }

void
MarianTranslator::initialize() {
  if (is_initialized_) return;
  is_initialized_ = true;
  
  std::string path_base = "assets/";
  auto model_dir = path_base + getSourceLang() + "-" + getTargetLang();
  
  std::string source_model = model_dir + "/source.spm";
  // std::string target_model = model_dir + "/target.spm";
  std::string vocab, model;

  if (getSourceLang() == "en" && getTargetLang() == "fi") {
    vocab = model_dir + "/opus+bt-2020-02-26_news.vocab.yml";
    model = model_dir + "/opus+bt-2020-02-26_news.transformer.model.npz.best-perplexity.npz";
  } else if (getSourceLang() == "fi" && getTargetLang() == "en") {
    vocab = model_dir + "/opus-2020-02-13_news.vocab.yml";
    model = model_dir + "/opus-2020-02-13_news.transformer.model.npz.best-perplexity.npz";
  } else if (false && (getSourceLang() == "fi" || getSourceLang() == "sv") && getTargetLang() == "en") {
    
  } else {
    // (sv,et,ru,de)-en
    vocab = model_dir + "/opus.spm32k-spm32k.vocab.yml";
    model = model_dir + "/opus.spm32k-spm32k.transformer-align.model1.npz.best-perplexity.npz";
  }

  cerr << "loading source spm\n";
  source_spm_ = make_unique<sentencepiece::SentencePieceProcessor>();
  source_spm_->Load(source_model);

#if 0
  cerr << "loading target spm\n";
  target_spm_ = make_unique<sentencepiece::SentencePieceProcessor>();
  target_spm_->Load(target_model);
#endif

#if 0
  std::vector<const char *> args;
  args.push_back("./m22");
  args.push_back("-m");
  args.push_back(model.c_str());
  args.push_back("-v");
  args.push_back(vocab.c_str());
  args.push_back(vocab.c_str());
  args.push_back("--cpu-threads");
  args.push_back("1");

  cerr << "loading options\n";

  ConfigParser cp(cli::mode::server);
  auto options = cp.parseOptions(args.size(), const_cast<char **>(args.data()), true);

#else
  auto options = New<Options>();
  // mandatory parameters
  options->set<std::vector<std::string>>("vocabs", { vocab, vocab });
  // options->set<std::string>("model", model);
  options->set<std::vector<std::string>>("models", { model });
  options->set<std::vector<int>>("dim-vocabs", { 65000, 65000 }); // 0 0 => use all
  // options->set<std::vector<int>>("dim-vocabs", { 0, 0 }); // use all
  options->set<std::vector<int>>("output-approx-knn", { 100, 1024 });
  options->set<std::vector<std::string>>("devices", { "0" });
  options->set<size_t>("cpu-threads", 1);
  options->set<bool>("optimize", false);
  options->set<std::string>("gemm-type", "float32");
  options->set<float>("quantize-range", 0.f);
  options->set<int>("workspace", 512);
  options->set<bool>("ignore-model-config", false);
  options->set<bool>("skip-cost", false);
  options->set<size_t>("max-length", 1000);
  options->set<bool>("max-length-crop", false);
  options->set<bool>("right-left", false);
  options->set<bool>("model-mmap", false);
  options->set<bool>("word-scores", false);
  options->set<int>("mini-batch", 1);
  options->set<int>("maxi-batch", 1);
#if 0
  options->set<size_t>("beam-size", 6);
#else
  options->set<size_t>("beam-size", 12);
#endif
  options->set<float>("word-penalty", 0.0f);
  options->set<float>("normalize", 0.0f); // implicit = 1.0f, default = 0.0f
  options->set<std::string>("factors-combine", "sum");
  options->set<int>("factors-dim-emb", 0);
  options->set<bool>("n-best", false);
  options->set<float>("max-length-factor", 3.0f);

  // not mandatory
  options->set<bool>("output-omit-bias", false);
  options->set<bool>("allow-unk", false);
  options->set<bool>("allow-special", false);
  options->set<std::string>("alignment", ""); // implicit = 1
  options->set<std::vector<std::string>>("precision", {"float32"});
  options->set<std::vector<std::string>>("output-sampling", { }); // implicit = full
  options->set<std::vector<int>>("output-approx-knn", { }); // implicit = "100 1024"
  options->set<std::string>("dump-config", "full");
  options->set<std::string>("type", "transformer"); // was amun
  options->set<int>("dim-emb", 512);
  options->set<int>("lemma-dim-emb", 0);
  options->set<int>("dim-rnn", 1024);
  options->set<std::string>("enc-type", "bidirectional");
  options->set<std::string>("enc-cell", "gru");
  options->set<int>("enc-cell-depth", 1);
  options->set<int>("enc-depth", 6); // was 1
  options->set<std::string>("dec-cell", "gru");
  options->set<int>("dec-cell-base-depth", 2);
  options->set<int>("dec-cell-high-depth", 1);
  options->set<int>("dec-depth", 6); // was 1

  options->set<int>("transformer-heads", 8);
  options->set<int>("transformer-dim-ffn", 2048);  
  options->set<int>("transformer-decoder-dim-ffn", 0);
  options->set<int>("transformer-ffn-depth", 2);
  options->set<int>("transformer-decoder-ffn-depth", 0);
  options->set<std::string>("transformer-ffn-activation", "swish");
  options->set<int>("transformer-dim-aan", 2048);
  options->set<int>("transformer-aan-depth", 2);
  options->set<std::string>("transformer-aan-activation", "swish");
  options->set<std::string>("transformer-decoder-autoreg", "self-attention");
  options->set<std::string>("transformer-guided-alignment-layer", "last");
  options->set<std::string>("transformer-postprocess-emb", "d");
  options->set<std::string>("transformer-postprocess", "dan");
  options->set<std::string>("transformer-postprocess-top", "");
  
  options->set<std::string>("bert-mask-symbol", "[MASK]");
  options->set<std::string>("bert-sep-symbol", "[SEP]");
  options->set<std::string>("bert-class-symbol", "[CLS]");
  options->set<float>("bert-masking-fraction", 0.15f);
  options->set<bool>("bert-train-type-embeddings", true);
  options->set<int>("bert-type-vocab-size", 2);

#if 0
  options->set<std::string>("maxi-batch-sort", "src");
#else
  options->set<std::string>("maxi-batch-sort", "none");
#endif
  options->set<size_t>("data-threads", 8); // 1 for deterministic
  
  // options->set<int>("char-stride",
  //     "Width of max-pooling layer after convolution layer in char-s2s model",
  //     5);
  // options->set<int>("char-highway",
  //     "Number of highway network layers after max-pooling in char-s2s model",
  //     4);
  // options->set<std::vector<int>>("char-conv-filters-num",
  //     "Numbers of convolution filters of corresponding width in char-s2s model",
  //     {200, 200, 250, 250, 300, 300, 300, 300});
  // options->set<std::vector<int>>("char-conv-filters-widths",
  //     "Convolution window widths in char-s2s model",
  //     {1, 2, 3, 4, 5, 6, 7, 8});

#endif
  cerr << "loading model: " << model << "\n";
  task_ = New<TranslateService<BeamSearch>>(options);
}

static std::vector<std::string> split_string(const std::string & line, const char * delimiters, bool keep_delimiters = false) {
  std::vector<std::string> r;
  size_t pos0 = 0;
  while (pos0 < line.size()) {
    auto pos1 = line.find_first_of(delimiters, pos0);
    if (pos1 == string::npos) pos1 = line.size();
    if (pos0 < pos1) r.push_back(line.substr(pos0, pos1 - pos0 + (keep_delimiters ? 1 : 0)));
    pos0 = pos1 + 1;
  }
  return r;
}

static void inline replace(std::string & data, const std::string from, const std::string & to) {
  std::string::size_type pos = 0;
  while ( 1 ) {
    pos = data.find(from, pos);
    if (pos == std::string::npos) break;
    data.replace(pos, from.size(), to);
    pos += to.size();
  }
}

static void preprocess(std::string & input) {
  replace(input, "，", ",");
  replace(input, "。", ".");
  replace(input, "．", ".");
  replace(input, "、", ",");
  replace(input, "”", "\"");
  replace(input, "“", "\"");
  replace(input, "∶", ":");
  replace(input, "：", ":"); 
  replace(input, "？", "?");
  replace(input, "《", "\"");
  replace(input, "》", "\"");
  replace(input, "）", ")");
  replace(input, "！", "!");
  replace(input, "（", "(");
  replace(input, "；", ";");
  replace(input, "１", "\"");
  replace(input, "」", "\"");
  replace(input, "「", "\"");
  replace(input, "０", "0");
  replace(input, "３", "3");
  replace(input, "２", "2");
  replace(input, "５", "5");
  replace(input, "６", "6");
  replace(input, "９", "9");
  replace(input, "７", "7");
  replace(input, "８", "8");
  replace(input, "４", "4");
  replace(input, "～", "~");
  replace(input, "’", "'");
  replace(input, "…", "...");
  replace(input, "━", "-");
  replace(input, "〈", "<");
  replace(input, "〉", ">");
  replace(input, "【", "[");
  replace(input, "】", "]");
  replace(input, "％", "%");

  // remove control characters
  for (auto & c : input) {
    if (c >= 0 && c < 32) c = ' ';
  }
  
  // concatenate spaces    
}

std::string
MarianTranslator::encodeSentencePiece(const std::string & input) {
  std::vector<std::string> sps;
  source_spm_->Encode(input, &sps);

  std::string encoded_input;
  auto sps_it = sps.begin();
  if (sps_it != sps.end()) {
    encoded_input = *sps_it++;
    for ( ; sps_it != sps.end(); sps_it++) {
      encoded_input += " " + *sps_it;
    }
  }

  return encoded_input;
}

std::string
MarianTranslator::decodeSentencePiece(const std::string & input0) {
#if 1
  auto input = input0;
  replace(input, " ", "");
  replace(input, "▁", " ");
  return input;
#else
  std::string detok;
  auto pieces = split_string(input, " ");
  target_spm_->Decode(pieces, &detok);  
  return detok;
#endif
}

std::string
MarianTranslator::translate(const std::string & input) {
  initialize();
  
  auto sentences = split_string(input, "!?.", true);
  std::string output;
  for (auto & sentence : sentences) {
    preprocess(sentence);
    
    auto sp = encodeSentencePiece(sentence);
    cerr << "translating sentence: " << sentence << ": " << sp << "\n";
    auto r = decodeSentencePiece(task_->run(sp));
    if (!output.empty()) output += " ";
    output += r;
  }
  return output;
}