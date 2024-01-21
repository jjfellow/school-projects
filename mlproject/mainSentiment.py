import sqlite3
import os
import sys
import yaml
from nltk.tokenize import sent_tokenize
import pandas as pd
import spacy
nlp = spacy.load("en_core_web_sm")


nbfile = "QT/build-naiveBayes-Qt_5_15_3_qt5-Debug/naiveBayes.csv"


def takeInput():
    # Checking for the correct number of arguments
    if len(sys.argv) != 2:
        print("Wrong number of arguments. Provide an input file name as \'filename.yml\'.")
        return None
    # Check if provided filename exists, then open it
    if not os.path.isfile(str(sys.argv[1])):
        print("Cannot find specified file.")
        print(str(sys.argv[1]))
        return None
    inFile = open(sys.argv[1],'rt')
    request = yaml.safe_load(inFile.read(-1))
    inFile.close()
    # Do things
    return request


def parse_pos_neg():
    positive = []
    negative = []
    df = pd.read_csv(nbfile, delimiter=';', header=0)
    for idx, row in df.iterrows():
        if row["NUM_1_REVIEWS"]+row["NUM_5_REVIEWS"] > 2:
            if row['POST_PROB_5'] > 0.55:
                positive.append(row["WORD"])
            elif row['POST_PROB_5'] < 0.45:
                negative.append(row["WORD"])

    return positive, negative


# create a list of globally defined positive and negative words to identify sentiment
# sentiment score based on the laxicon neg, pos words
def feature_sentiment(sentence, pos, neg):
    '''
    input: dictionary and sentence
    function: appends dictionary with new features if the feature
              did not exist previously,then updates sentiment to
              each of the new or existing features
    output: updated dictionary
    '''
    sent_dict = dict()
    sentence = nlp(sentence)
    opinion_words = neg + pos
    debug = 0
    for token in sentence:
        # check if the word is an opinion word, then assign sentiment
        if token.text in opinion_words:
            sentiment = 1 if token.text in pos else -1
            # if target is an adverb modifier (i.e. pretty, highly, etc.)
            # but happens to be an opinion word, ignore and pass
            if (token.dep_ == "advmod"):
                continue
            elif (token.dep_ == "amod"):
                sent_dict[token.head.text] = sentiment
            # for opinion words that are adjectives, adverbs, verbs...
            else:
                for child in token.children:
                    # if there's a adj modifier (i.e. very, pretty, etc.) add more weight to sentiment
                    # This could be better updated for modifiers that either positively or negatively emphasize
                    if ((child.dep_ == "amod") or (child.dep_ == "advmod")) and (child.text in opinion_words):
                        sentiment *= 1.5
                    # check for negation words and flip the sign of sentiment
                    if child.dep_ == "neg":
                        sentiment *= -1
                for child in token.children:
                    # if verb, check if there's a direct object
                    if (token.pos_ == "VERB") & (child.dep_ == "dobj"):
                        sent_dict[child.text] = sentiment
                        # check for conjugates (a AND b), then add both to dictionary
                        subchildren = []
                        conj = 0
                        for subchild in child.children:
                            if subchild.text == "and":
                                conj = 1
                            if (conj == 1) and (subchild.text != "and"):
                                subchildren.append(subchild.text)
                                conj = 0
                        for subchild in subchildren:
                            sent_dict[subchild] = sentiment

                # check for negation
                for child in token.head.children:
                    noun = ""
                    if ((child.dep_ == "amod") or (child.dep_ == "advmod")) and (child.text in opinion_words):
                        sentiment *= 1.5
                    # check for negation words and flip the sign of sentiment
                    if (child.dep_ == "neg"):
                        sentiment *= -1

                # check for nouns
                for child in token.head.children:
                    noun = ""
                    if (child.pos_ == "NOUN") and (child.text not in sent_dict):
                        noun = child.text
                        # Check for compound nouns
                        for subchild in child.children:
                            if subchild.dep_ == "compound":
                                noun = subchild.text + " " + noun
                        sent_dict[noun] = sentiment
                    debug += 1
    return sent_dict


yml = takeInput()
if yml is None:
    sys.exit(-1)

pos, neg = parse_pos_neg()
category = yml['Category']
look = yml["LookFor"]
conn = sqlite3.connect("companyReviews.db")
cursor = conn.cursor()
query = "SELECT REVIEW.RevText, COMPANY.Name, COMPANY.GmapID " \
        "FROM REVIEW JOIN COMPANY ON REVIEW.GmapID = COMPANY.GmapID " \
        "WHERE COMPANY.Category LIKE '%"+category+"%' ORDER BY COMPANY.GmapID;"
res = cursor.execute(query)
resLine = res.fetchone()

while resLine is not None:
    sentences = sent_tokenize(resLine[0])
    print(sentences)
    for s in sentences:
        sentiment = feature_sentiment(s, pos, neg)
        print(sentiment)
    resLine = res.fetchone()