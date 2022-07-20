from flaskapp import db, bcrypt


player_treasure = db.Table('player_treasure',
                           db.Column('player_id', db.Integer, db.ForeignKey('player.id', ondelete='CASCADE')),
                           db.Column('level1_treasure_id', db.Integer, db.ForeignKey('level1_treasure.id', ondelete='CASCADE'))
                           )


class Player(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    OG = db.Column(db.Integer, nullable=False)
    participant_id = db.Column(db.Integer)  # in place of name, sequential number inside OG
    mac_address = db.Column(db.Text)
    num_kills = db.Column(db.Integer)

    treasure = db.relationship('Level2Treasure', back_populates='collected_by')

    level1_treasures = db.relationship('Level1Treasure', secondary=player_treasure, back_populates='collected_players')

    def __init__(self, OG, mac_address):
        self.OG = OG
        self.mac_address = mac_address
        self.participant_id = None
        self.num_kills = 0


class Level2Treasure(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    location = db.Column(db.Text)
    collected_by_id = db.Column(db.Integer, db.ForeignKey('player.id'))

    is_treasure = db.Column(db.Boolean, nullable=False, default=True)
    is_virus = db.Column(db.Boolean, nullable=False, default=False)

    collected_by = db.relationship('Player', back_populates='treasure')

    def __init__(self, name, location, is_treasure, is_virus):
        self.name = name
        self.location = location
        self.is_treasure = is_treasure
        self.is_virus = is_virus


class Level1Treasure(db.Model):
    __tablename__ = 'level1_treasure'
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    location = db.Column(db.Text)

    collected_players = db.relationship('Player', secondary=player_treasure, back_populates="level1_treasures")

    def __init__(self, name, location):
        self.name = name
        self.location = location


class GameStatus(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    value = db.Column(db.Text, nullable=False)

    def __init__(self, name, value):
        self.name = name
        self.value = value


def populate_statuses():
    statuses = {
        "started": 1,
        "EN_RECOVER_DURATION": 10000,
        "MAX_EN_DECAY_DURATION": 10000,
        "VIRUS_DECAY_DURATION": 10000,
        "VIRUS_IMMUNITY_DURATION": 10000,
        "PARTICIPANT_MaxHP": 12,
        "GL_MaxHP": 20,
        "PARTICIPANT_MaxEn": 5,
        "GL_MaxEn": 10,
        "INITIAL_MANA": 1,
        "INITIAL_ATTACK_MANA": 1,
        "INITIAL_COLLECT_MANA": 1,
        "MAX_COLLECT_MANA": 10,
        "MAX_ATTACK_MANA": 3,
        "HEAL_MANA": 10,
        "VIRUS_INFECTION_PROBABILITY": 40,
        "BOMB_HP_DEDUCTION": 5
    }
    for status in statuses:
        s = GameStatus(status, str(statuses[status]))
        db.session.add(s)

    db.session.commit()


def populate_treasure():
    for i in range(1, 6):
        t = Level1Treasure("TREASURE" + str(i), "-")
        db.session.add(t)
    for i in range(6, 9):
        t = Level2Treasure("TREASURE" + str(i), "-", True, False)
        db.session.add(t)
    for i in range(9, 11):
        t = Level2Treasure("TREASURE" + str(i), "-", False, True)
        db.session.add(t)
    db.session.commit()
