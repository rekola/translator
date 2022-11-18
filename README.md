# translator

Machine Translation Service

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

