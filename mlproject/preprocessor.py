# Planning time:
# Check if the database exists, if not initialize it 
# Read a line in, tokenize the string, save text, rating, and gmap_id
# Using nltk, simplify the text portion with synonyms
# Save this result to the database
# Next, the meta data section gives context to the gmap_id s and includes the category. Keep all meta data
# Will have to make a decision regarding Categories

import os
import sqlite3
import nltk
import json


# Function for adding a review into the REVIEW table, requires a connection object to a database as well as the 4 fields
def addReview(conn, index, rating, id, text):
    cur = conn.cursor()
    cur.execute(
    """
        INSERT INTO REVIEW VALUES (:ind, :rate, :GmapID, :revText)
    """,
    {
        'ind': index,
        'rate' : rating,
        'GmapID' : id,
        'revText' : text
    })

# Function for adding a company to the COMPANY table, requires a connection object to a database as well as the 6 categories
def addCompany(conn, id, name, address, desc, category, avgRating):
    cur = conn.cursor()
    cur.execute(
        """
            INSERT INTO COMPANY VALUES (:id, :name, :addr, :desc, :cat, :avgR);
        """,
        {
            'id' : id,
            'name' : name,
            'addr' : address,
            'desc' : desc,
            'cat' : str(category),
            'avgR' : avgRating
        }
    )
def init():
    # Check if init has been called before
    
    if not os.path.isfile("initstat"):
        statusFile = open("initstat", 'wt')
        statusFile.write("0")
        statusFile.close()
    statusFile = open("initstat", 'rt')
    if statusFile.read() == "1":
        statusFile.close()
        print("Initialization has already been performed.")
        return
    

    # Necessary file Paths, change these as needed
    reviewPath = "/home/kate/school/mlearning/Project/review-Texas.json"
    companyPath = "/home/kate/school/mlearning/Project/meta-Texas.json"

    # sqlite3 will create this db if it hasn't been created already
    procDB = sqlite3.connect("companyReviews.db")

    # Create a cursor, and initialize the db. SQL handles the check for existing table
    cursor = procDB.cursor()
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS COMPANY (
    	GmapID		VARCHAR(50)		NOT NULL,
    	Name		VARCHAR(4096)	NOT NULL,
    	Address		VARCHAR(4096),
    	Description	VARCHAR(4096),
    	Category	TEXT,
    	AVGRating	DECIMAL(2,1),

    	PRIMARY KEY(GmapID)
        );
        """
    )

    cursor.execute("""
    CREATE TABLE IF NOT EXISTS REVIEW (
    	Ind         INT,
        Rating      INT,
    	GmapID      VARCHAR(50)     NOT NULL,
    	RevText     VARCHAR(4096)   NOT NULL,
        PRIMARY KEY (Ind),
    	FOREIGN KEY(GmapID) REFERENCES COMPANY(GmapID)
    );
        """
    )
    # Changes must be committed or else they are not saved
    procDB.commit()

    # Open the review json file and load the database with the fields we care about
    reviews = open(reviewPath, 'rt')
    x = 0
    for line in reviews:
        js = json.loads(line)
        if js['text'] != None:
            addReview(procDB, x, js['rating'], js['gmap_id'], js['text'])
            x += 1
    procDB.commit()

    reviews.close()
    companies = open(companyPath, 'rt')

    for line in companies:
        js = json.loads(line)
        try:
            addCompany(procDB, js['gmap_id'], js['name'], js['address'], js['description'], js['category'], js['avg_rating'])
        except sqlite3.IntegrityError:
            print("Found a non-unique gmap id of " + js['gmap_id'] + " when adding a company.")


    procDB.commit()

    procDB.close()
    companies.close()
    statusFile = open("initstat", 'wt')
    statusFile.write("1")
    statusFile.close()

if __name__ == "__main__":
    init()
