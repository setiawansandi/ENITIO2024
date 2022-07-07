from flaskapp import db, bcrypt


class Player(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    OG = db.Column(db.Integer, nullable=False)
    participant_id = db.Column(db.Integer)  # in place of name, sequential number inside OG
    mac_address = db.Column(db.Text)

    treasure = db.relationship('Treasure', back_populates='collected_by')

    def __init__(self, OG, mac_address):
        self.OG = OG
        self.mac_address = mac_address
        self.participant_id = None


class Treasure(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    level = db.Column(db.Integer, primary_key=True)
    location = db.Column(db.Text)
    collected_by_id = db.Column(db.Integer, db.ForeignKey('player.id'))

    is_treasure = db.Column(db.Boolean, nullable=False, default=True)
    is_virus = db.Column(db.Boolean, nullable=False, default=False)

    collected_by = db.relationship('Player', back_populates='treasure')

    def __init__(self, name, level, location, is_treasure, is_virus):
        self.name = name
        self.level = level
        self.location = location
        self.is_treasure = is_treasure
        self.is_virus = is_virus


class GameStatus(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    value = db.Column(db.Text, nullable=False)

    def __init__(self, name, value):
        self.name = name
        self.value = value
