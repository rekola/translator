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

