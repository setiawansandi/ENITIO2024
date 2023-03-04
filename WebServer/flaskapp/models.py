from flaskapp import db, bcrypt


class Player(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    clan_id = db.Column(db.Integer, nullable=False)
    participant_id = db.Column(db.Integer)  # in place of name, sequential number inside clan
    mac_address = db.Column(db.Text)
    num_kills = db.Column(db.Integer, nullable=False)
    num_level1_treasures_wanderer = db.Column(db.Integer, nullable=False)
    num_level2_treasures_wanderer = db.Column(db.Integer, nullable=False)

    treasure = db.relationship('Level2Treasure', back_populates='collected_by')
    level1_treasures = db.relationship('Level1TreasureCollectors')

    def __init__(self, clan_id, mac_address):
        self.clan_id = clan_id
        self.mac_address = mac_address
        self.participant_id = None
        self.num_kills = 0
        self.num_level1_treasures_wanderer = 0
        self.num_level2_treasures_wanderer = 0


class Level2Treasure(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    treasure_type = db.Column(db.Integer, nullable=False)
    location = db.Column(db.Text)
    collected_by_id = db.Column(db.Integer, db.ForeignKey('player.id'))
    
    collected_by = db.relationship('Player', back_populates='treasure')

    def __init__(self, name, location, treasure_type):
        self.name = name
        self.location = location
        self.treasure_type = treasure_type


class Level1Treasure(db.Model):
    __tablename__ = 'level1_treasure'
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    location = db.Column(db.Text)
    num_invicta_collected = db.Column(db.Integer)
    num_dynari_collected = db.Column(db.Integer)
    num_ephilia_collected = db.Column(db.Integer)
    num_akrona_collected = db.Column(db.Integer)
    num_solaris_collected = db.Column(db.Integer)

    collected_players = db.relationship('Level1TreasureCollectors')

    def __init__(self, name, location):
        self.name = name
        self.location = location
        self.num_invicta_collected = 0
        self.num_dynari_collected = 0
        self.num_ephilia_collected = 0
        self.num_akrona_collected = 0
        self.num_solaris_collected = 0
    
    def reset_stats(self):
        self.num_invicta_collected = 0
        self.num_dynari_collected = 0
        self.num_ephilia_collected = 0
        self.num_akrona_collected = 0
        self.num_solaris_collected = 0
        

class Level1TreasureCollectors(db.Model):
    __tablename__ = 'player_treasure'
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    level1_treasure_id = db.Column(db.Integer, db.ForeignKey('level1_treasure.id', ondelete='CASCADE'))
    player_id = db.Column(db.Integer, db.ForeignKey('player.id', ondelete='CASCADE'))

    treasure = db.relationship(Level1Treasure, overlaps="collected_players")
    player = db.relationship(Player, overlaps="level1_treasures")

    def __init__(self, player, treasure):
        self.treasure = treasure
        self.player = player


class GameStatus(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True, nullable=False)
    name = db.Column(db.Text, nullable=False)
    value = db.Column(db.Integer, nullable=False)

    def __init__(self, name, value):
        self.name = name
        self.value = value


def populate_constants_table():
    with open("instance/constants.csv") as f:
        content = f.readlines()[1:]  # remove header row
        for row in content:
            if not row:
                continue
            row = row.strip()  # remove \n at the end
            name, value, units = row.split(",")
            value = int(value.strip())
            var = GameStatus(name=name, value=value)
            db.session.add(var)
    
    db.session.commit()

def populate_treasures_table():
    with open("instance/treasure_locations.csv") as f:
        content = f.readlines()[1:]  # remove header row
        for row in content:
            if not row:
                continue
            row = row.strip()  # remove \n at the end
            name, level, treasure_type, location = row.split(",")
            level = int(level.strip())
            treasure_type = int(treasure_type.strip())
            if level == 1:
                t = Level1Treasure(name, location)
            elif level == 2:
                t = Level2Treasure(name, location, treasure_type)
            db.session.add(t)
    
    db.session.commit()
