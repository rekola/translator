# translator

[![CI](https://github.com/rekola/translator/workflows/Ubuntu-CI/badge.svg)]()
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)

Machine Translation Service based on MarianNMT. The project is a
microservice that contains a web server that provides a REST based
HTTP API for machine translation. FastText is used for input language
detection. This is a work in progress. The software is not yet
multi-threaded, so translation tasks block the server for their
duration.

## Dependencies

- MarianNMT
- OpenBLAS
- {fmt}
- fastText
- cpp-httplib
- utf8proc
- nlohmann::json

## Assets

Language assets are hardcoded in the program. You must ensure that the needed assets are present in the assets subdirectory.

### fastText model for language detection

Place lid.176.ftz in assets directory (download link: https://fasttext.cc/docs/en/language-identification.html)

### Hardcoded language models

Place each language model in the assets directory in $SOURCELANG/$TARGETLANG subdirectory. New models can be downloaded from https://github.com/Helsinki-NLP/Opus-MT-train/tree/master/models and must be added to MarianTranslator.cpp and TranslationContext.cpp

| Source | Target | Model | Bleu (Tatoeba) |
| - | - | - | - |
| en | fi | [opus+bt-2020-02-26.zip](https://object.pouta.csc.fi/OPUS-MT-models/en-fi/opus+bt-2020-02-26.zip) | 41.4 |
| fi | en | [opus-2020-02-13.zip](https://object.pouta.csc.fi/OPUS-MT-models/fi-en/opus-2020-02-13.zip) | 57.4 |
| fi | ru | [opus-2020-04-12.zip](https://object.pouta.csc.fi/OPUS-MT-models/fi-ru/opus-2020-04-12.zip) | 46.3 |
| sv | en | [opus-2020-02-26.zip](https://object.pouta.csc.fi/OPUS-MT-models/sv-en/opus-2020-02-26.zip) | 64.5 |
| et | en | [opus-2019-12-18.zip](https://object.pouta.csc.fi/OPUS-MT-models/et-en/opus-2019-12-18.zip) | 59.9 |
| ru | en | [opus-2020-02-26.zip](https://object.pouta.csc.fi/OPUS-MT-models/ru-en/opus-2020-02-26.zip) | 61.1 |
| de | en | [opus-2020-02-26.zip](https://object.pouta.csc.fi/OPUS-MT-models/de-en/opus-2020-02-26.zip) | 55.4 |
| uk | en | [opus-2020-01-16.zip](https://object.pouta.csc.fi/OPUS-MT-models/uk-en/opus-2020-01-16.zip) | 64.1 |

#### BLEU Score Interpretation

| BLEU Score | Interpretation                                            |
|------------|-----------------------------------------------------------|
| < 10       | Almost useless                                            |
| 10 - 19    | Hard to get the gist                                      |
| 20 - 29    | The gist is clear, but has significant grammatical errors |
| 30 - 40    | Understandable to good translations                       |
| 40 - 50    | High quality translations                                 |
| 50 - 60    | Very high quality, adequate, and fluent translations      |
| > 60       | Quality often better than human                           |


## Installation (Ubuntu)

### Install dependencies

```
sudo apt install libutf8proc-dev libopenblas-dev libfmt-dev libfasttext-dev
```

### Compilation (CPU)

```
mkdir build
cd build
cmake ../ -DCOMPILE_CPU=on
```

### Compilation (GPU)

```
mkdir build
cd build
cmake ../ -DCOMPILE_CUDA=on
```

## Running

```
./translator
```

Access the url ```http://localhost:8080/translate?q=Hello%20world&target=fi``` to test.

## API

### Endpoints

| Path | Description |
| - | - |
| /translate | Translates input from source to target language |

### /translate

| Parameter | Required | Description |
| - | - | - |
| source | No | Source language. If missing, the language is autodetected |
| target | Yes | Target language (e.g. en) |
| q | Yes | Input text. Can be used multiple times. |
| format | No | Output format. Not used. |

#### Response

```json
{"data":{"translations":[{"detectedSourceLanguage":"en","translatedText":" Hei maailma"}]}}
```

## Known issues

- Translation is done one sentence at a time, which leads to suboptimal translations
- Duplicate parameters are ignored, which means that you cannot translate the same text multiple times.
