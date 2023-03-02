import os
basedir = os.path.abspath(os.path.dirname(__file__))


class Config:
    DEBUG = False
    SECRET_KEY = ""
    # SQLALCHEMY_DATABASE_URI = 'sqlite:///' + os.path.join(basedir, 'flaskapp.db')

    SQLALCHEMY_DATABASE_URI = "postgresql://{username}:{password}@{hostname}/{databasename}".format(
        username="",
        password="",
        hostname="localhost",
        databasename="enitio2023",
    )

    SQLALCHEMY_POOL_RECYCLE = 299
    SQLALCHEMY_TRACK_MODIFICATIONS = False
