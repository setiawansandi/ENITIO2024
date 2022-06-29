import os
basedir = os.path.abspath(os.path.dirname(__file__))


class Config:
    DEBUG = False
    SECRET_KEY = "5407fb3612286a2c8e8333b930042c85"
    FLASK_ENV = "production"
    SQLALCHEMY_DATABASE_URI = 'sqlite:///' + os.path.join(basedir, 'flaskapp.db')
    SQLALCHEMY_TRACK_MODIFICATIONS = False
