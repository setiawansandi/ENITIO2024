from flask import request, jsonify, abort, render_template, redirect, url_for
from flaskapp import app
from flaskapp.models import *


@app.route("/treasure/<int:level>/<name>/<int:clan_id>/<int:participant_id>", methods=["GET"])
def set_treasure_as_collected(level, name, clan_id, participant_id):
    print("Received Treasure Collection Request - {} (Level {}) by CLAN {} ID {}".format(name, level, clan_id, participant_id))
    player = db.session.execute(db.select(Player).filter_by(clan_id=clan_id, participant_id=participant_id)).scalar_one()
    if player:
        treasure = None
        if level == 2:
            treasure = db.session.execute(db.select(Level2Treasure).filter_by(name=name)).scalar_one()
            if treasure:
                print(treasure.name, "collected by CLAN", player.clan_id, "ID", player.participant_id, "- GET")
                treasure.collected_by = player
                db.session.commit()
                print("Committed")

        elif level == 1:
            treasure = db.session.execute(db.select(Level1Treasure).filter_by(name=name)).scalar_one()
            if treasure:
                print("Uploading Treasure Level 1 Collection ({}) by Player CLAN {} ID {} - GET".format(treasure.name, player.clan_id, player.participant_id))
                collection_row = Level1TreasureCollectors(player, treasure)
                db.session.add(collection_row)
                db.session.commit()
                print("Committed")

        # return MAC address to treasure to send confirmation message
        if treasure is not None:
            clan_hex_str = "{0:02x}".format(clan_id)
            participant_id_hex_str = "{0:02x}".format(participant_id)
            mac_address_str = "04:08:01:{}:{}:01".format(clan_hex_str, participant_id_hex_str)
            result = {"mac_address_part5": "04",
                      "mac_address_part4": "08",
                      "mac_address_part3": "01",
                      "mac_address_part2": clan_hex_str,
                      "mac_address_part1": participant_id_hex_str,
                      "mac_address_part0": "01",
                      "mac_address": mac_address_str}
            print("Returning Request to", mac_address_str)
            return jsonify(result)

    print("Aborting Request...")
    abort(404)


@app.route("/player_id/<int:clan_id>/<mac_addr>")
def get_player_id(clan_id, mac_addr):
    result = dict()
    player = db.session.execute(db.select(Player).filter_by(clan_id=clan_id, mac_address=mac_addr)).scalar_one()
    if player:
        player_id = player.participant_id
        if player_id is None:
            # Check if server allow for assignment after game starts
            statuses = db.session.execute(db.select(GameStatus).filter(GameStatus.name.in_(("started", "ASSIGN_PARTICIPANT_ID_AFTER_GAME_START")))).scalars()
            both_true = True
            for row in statuses:
                if row.value != 1:
                    both_true = False
            
            if both_true:
                # Assign ID
                largest_participant_id_player = db.session.execute(db.select(Player).filter_by(clan_id=clan_id).order_by(Player.participant_id.desc())).scalar_one()
                if largest_participant_id_player and largest_participant_id_player.participant_id is not None:
                    player_id = largest_participant_id_player.participant_id + 1
                else:
                    player_id = 1  # No other players in OG
                player.participant_id = player_id
                db.session.commit()
            
        result["player_id"] = player_id
        return jsonify(result)

    abort(404)


@app.route("/player_registration", methods=["POST"])
def register_player():
    content = request.json
    if "clan" in content and "mac_address" in content:
        clan_id = content["clan"]
        mac_addr = content["mac_address"]
        player = db.session.execute(db.select(Player).filter_by(clan_id=clan_id, mac_address=mac_addr)).scalar_one()
        if player:
            return jsonify({"result": "already registered"})
        else:
            player = Player(clan_id=clan_id, mac_address=mac_addr)
            db.session.add(player)
            db.session.commit()
            return jsonify({"result": "OK"})

    abort(404)


@app.route("/game_status")
def has_game_started():
    status = db.session.execute(db.select(GameStatus).filter_by(name="started")).scalar_one()
    return jsonify({"has_game_started": int(status.value)})


@app.route("/get_game_constants")
def get_all_game_variables():
    variables = db.session.execute(db.select(GameStatus)).scalars()
    result = dict()
    for var in variables:
        result[var.name] = int(var.value)

    return jsonify(result)


@app.route("/reset_score")
def reset_score():
    g = db.session.execute(db.select(GameStatus).filter_by(name="started")).scalar_one()
    if g and g.value == 1:
        return jsonify({"result": "ignored"})

    players = db.session.execute(db.select(Player)).scalars()
    for player in players:
        player.num_kills = 0
        player.num_level1_treasures_wanderer = 0
        player.num_level2_treasures_wanderer = 0

    # RESET EVERYTHING
    level1_treasure_collection_logs = db.session.execute(db.select(Level1TreasureCollectors)).scalars()
    for collection_log in level1_treasure_collection_logs:
        db.session.delete(collection_log)

    level2_treasures = db.session.execute(db.select(Level2Treasure)).scalars()
    for treasure in level2_treasures:
        treasure.collected_by = None

    level1_treasures = db.session.execute(db.select(Level1Treasure)).scalars()
    for treasure in level1_treasures:
        treasure.reset_stats()

    db.session.commit()
    return jsonify({"result": "OK"})


@app.route("/start_game")
def start_game():
    # assign all players their IDs
    players = db.session.execute(db.select(Player)).scalars()
    clan_counter = dict()
    result = dict()
    for player in players:
        if player.clan_id not in clan_counter:
            clan_counter[player.clan_id] = 0
        player.participant_id = clan_counter[player.clan_id] + 1
        clan_counter[player.clan_id] += 1
        result[player.mac_address] = "CLAN {} ID {}".format(player.clan_id, player.participant_id)

    status = db.session.execute(db.select(GameStatus).filter_by(name="started")).scalar_one()
    status.value = 1
    db.session.commit()

    return jsonify(result)


@app.route("/stop_game")
def stop_game():
    status = db.session.execute(db.select(GameStatus).filter_by(name="started")).scalar_one()
    status.value = 0
    db.session.commit()

    return jsonify({"has_game_started": "0"})

