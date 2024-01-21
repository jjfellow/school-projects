# meta dict_keys(['name', 'address', 'gmap_id', 'description', 'latitude', 'longitude', 'category', 'avg_rating',
#                 'num_of_reviews', 'price', 'hours', 'MISC', 'state', 'relative_results', 'url'])
# revi dict_keys(['user_id', 'name', 'time', 'rating', 'text', 'pics', 'resp', 'gmap_id'])

import json
import pickle
import csv
import string
import enchant
from collections import Counter
from nltk.corpus import stopwords
import nltk
nltk.download('stopwords')

import pandas as pd


def get_sample(min_count):
    f1 = open("meta-Texas.json")
    metaLines = f1.readlines()
    sample = ""
    num_of_reviews = 0

    for l in metaLines:
        js = json.loads(l)
        if js['num_of_reviews'] > min_count:
            # print(js['num_of_reviews'])
            num_of_reviews = int(js['num_of_reviews'])
            sample = js['gmap_id']
            break

    sample_reviews = []

    with open("review-Texas.json") as f2:
        for line in f2:
            js = json.loads(line)
            if js['gmap_id'] == sample:
                sample_reviews.append(js)
                # print(js['rating'], "::", js['text'])
                if len(sample_reviews) == num_of_reviews:
                    break

    return sample_reviews


def testSave():
    dict = {"name": "CVS", "reviews": [
        {'rating': 5, 'review': "I loved it!"},
        {'rating': 1, 'review': "I hated it!"}
    ]}

    with open('pickles/saved_dictionary.pkl', 'wb') as f:
        pickle.dump(dict, f)


def testLoad():
    with open('pickles/saved_dictionary.pkl', 'rb') as f:
        loaded_dict = pickle.load(f)

        print(loaded_dict["reviews"][0]['rating'])


def countReviews():
    with open('review-Texas.json') as rev_file:
        count = 0
        for _ in rev_file:
            count += 1

    return count


def init():
    categories = []
    businesses = []

    with open('meta-Texas.json') as metaf:
        # get a list of categories
        for line in metaf:
            js = json.loads(line)
            if js['category'] is None:
                continue
            for c in js['category']:
                categories.append(c)

        # list of categories is sorted by frequency
        categories = sorted(Counter(categories))

        # Assign 1 (most popular) category for each business
    with open('meta-Texas.json') as metaf:
        with open('businesses.csv', 'w') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=['name', 'id', 'category'])
            writer.writeheader()
            for line in metaf:
                js = json.loads(line)
                if js['category'] is None:
                    continue
                chosen = ""
                for c in categories:
                    if c in js['category']:
                        chosen = c
                        break
                if chosen not in businesses:
                    businesses.append(chosen)
                rowdict = {'name': js['name'], 'id': js['gmap_id'], 'category': chosen}
                writer.writerow(rowdict)
            print(len(businesses))


def add_to_dataframe(df, five_star, one_star, word):
    word = word.lower()
    if not (df['word'].eq(word)).any():
        df = pd.concat([df, pd.DataFrame({
            'word': [word],
            'one_star': [one_star],
            'five_star': [five_star]})], ignore_index=True)
    else:
        df.loc[df['word'] == word, 'five_star'] += five_star
        df.loc[df['word'] == word, 'one_star'] += one_star

    return df


# sample_reviews = get_sample(250)
five = 0
one = 0
count = 0
percent = 0
dictionary = enchant.Dict("en_US")
df = pd.DataFrame({
    'word': pd.Series(dtype=str),
    'one_star': pd.Series(dtype=int),
    'five_star': pd.Series(dtype=int),
    'post_prob_5': pd.Series(dtype=float)})

with open("review-Texas.json") as f:
    for line in f:
        count += 1
        if count == 665000:
            percent += 1
            print(percent, "% done")
            count = 0
        r = json.loads(line)
        if r['text'] is None:
            continue
        if r['rating'] != 1 and r['rating'] != 5:
            continue
        # remove punctuation
        review = str(r['text']).translate(str.maketrans('', '', string.punctuation))
        review = review.split()
        for word in review:
            if not dictionary.check(word):
                continue
            elif not word.isalpha():
                continue
            elif word in stopwords.words():
                continue
            elif r['rating'] == 5:
                five += 1
                df = add_to_dataframe(df, 1, 0, word)
            elif r['rating'] == 1:
                one += 1
                df = add_to_dataframe(df, 0, 1, word)

total = five+one
for (i, row) in df.iterrows():
    total_word = row.one_star + row.five_star
    df.at[i, 'post_prob_5'] = ((row.five_star/five) * (five/total)) / (total_word/total)
# print(df)
df = df.sort_values(by='post_prob_5')
df.to_pickle('./pickles/naiveClassify.pkl')
print(five)
print(one)

