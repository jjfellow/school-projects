import sys
import enchant
from nltk.corpus import stopwords

dictionary = enchant.Dict("en_US")
word = sys.argv[1]
if not dictionary.check(word):
    sys.exit(0)
elif not word.isalpha():
    sys.exit(0)
elif word in stopwords.words():
    sys.exit(0)

sys.exit(1)
