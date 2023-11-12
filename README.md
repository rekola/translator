# translator

Machine Translation Service based on MarianNMT. The project is a
microservice that contains a web server that provides a REST based
HTTP API for machine translation. This is a work in progress.

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



## Installation (Ubuntu)

### Install dependencies

```
sudo apt install libutf8proc-dev libopenblas-dev fmt-dev libfasttext-dev
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

- Duplicate parameters are ignored, which means that you cannot translate the same text multiple times.

## BLEU Score Interpretation

| BLEU Score | Interpretation                                            |
|------------|-----------------------------------------------------------|
| < 10       | Almost useless                                            |
| 10 - 19    | Hard to get the gist                                      |
| 20 - 29    | The gist is clear, but has significant grammatical errors |
| 30 - 40    | Understandable to good translations                       |
| 40 - 50    | High quality translations                                 |
| 50 - 60    | Very high quality, adequate, and fluent translations      |
| > 60       | Quality often better than human                           |

