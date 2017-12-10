import pymongo
from pymongo import MongoClient
import datetime


client = MongoClient('mongodb://localhost:27017/')

db = client.test_database

collection = db.test_collection

post = {
    "author": "Wass",
    "text":"This is the content to show",
    "tags":["mongod","python","nodejs","react"],
    "date":datetime.datetime.utcnow()
}

posts = db.posts
post_id = posts.insert_one(post).inserted_id

print post_id
