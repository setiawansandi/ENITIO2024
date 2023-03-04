from flask import request, jsonify, abort, render_template, redirect, url_for
from flaskapp import app
from flaskapp.models import *


@app.route("/treasure/<int:level>/<name>/<int:clan_id>/<int:participant_id>", methods=["GET"])
def set_treasure_as_collected(level, name, clan_id, participant_id):
    print("Received Treasure Collection Request - {} (Level {}) by CLAN {} ID {}".format(name, level, clan_id, participant_id))
    player = db.session.execute(db.select(Player).filter_by(clan_id=clan_id, participant_id=participant_id)).scalar_one_or_none()
    if player:
        treasure = None
        if level == 2:
            treasure = db.session.execute(db.select(Level2Treasure).filter_by(name=name)).scalar_one_or_none()
            if treasure:
                print(treasure.name, "collected by CLAN", player.clan_id, "ID", player.participant_id, "- GET")
                treasure.collected_by = player
                db.session.commit()
                print("Committed")

        elif level == 1:
            treasure = db.session.execute(db.select(Level1Treasure).filter_by(name=name)).scalar_one_or_none()
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
    player = db.session.execute(db.select(Player).filter_by(clan_id=clan_id, mac_address=mac_addr)).scalar_one_or_none()
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
                largest_participant_id_player = db.session.execute(db.select(Player).filter_by(clan_id=clan_id).order_by(Player.participant_id.desc())).first()
                if largest_participant_id_player and largest_participant_id_player[0].participant_id is not None:
                    player_id = largest_participant_id_player[0].participant_id + 1
                else:
                    player_id = 1  # No other players in OG
                player.participant_id = player_id
                db.session.commit()
            
        result["player_id"] = player_id

        variables = db.session.execute(db.select(GameStatus)).scalars()
        for var in variables:
            result[var.name] = int(var.value)
        return jsonify(result)

    abort(404)


@app.route("/register", methods=["POST"])
def register_player():
    content = request.json
    if "clan" in content and "mac_address" in content:
        clan_id = content["clan"]
        mac_addr = content["mac_address"]
        player = db.session.execute(db.select(Player).filter_by(clan_id=clan_id, mac_address=mac_addr)).scalar_one_or_none()
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


@app.route("/treasure_enquiry_all")
def get_all_treasure_status():
    result = dict()
    level2_treasures = db.session.execute(db.select(Level2Treasure)).scalars()
    for treasure in level2_treasures:
        collected_player = treasure.collected_by
        if treasure.treasure_type == 0:
            result[treasure.name] = "Level2Treasure "
        elif treasure.treasure_type == 1:
            result[treasure.name] = "Level2Virus "
        elif treasure.treasure_type == 2:
            result[treasure.name] = "Level2Poison "
        result[treasure.name] += "({}) ".format(treasure.location)
        if collected_player:
            result[treasure.name] += "Collected by CLAN {} ID {}".format(collected_player.clan_id, collected_player.participant_id)
        else:
            result[treasure.name] += "<available>"

    level1_treasures = db.session.execute(db.select(Level1Treasure)).scalars()
    for treasure in level1_treasures:
        t_id = treasure.id
        collection_logs = db.session.execute(db.select(Level1TreasureCollectors).filter_by(level1_treasure_id=t_id)).all()
        num_collected = len(collection_logs)
        result[treasure.name] = "Level1Treasure ({}) - Collected {} times".format(treasure.location, num_collected)

    return jsonify(result)


@app.route("/unset_all_player_ids")
def unset_all_player_ids():
    players = db.session.execute(db.select(Player)).scalars()
    for player in players:
        player.participant_id = None

    # ensure game status is False
    status = db.session.execute(db.select(GameStatus).filter_by(name="started")).scalar_one()
    status.value = "0"
    db.session.commit()
    return jsonify({"result": "OK"})


@app.route("/delete_all_players")
def delete_all_players():
    # check if game started
    g = db.session.execute(db.select(GameStatus).filter_by(name="started")).scalar_one()
    if g and g.value == "1":
        return {"result": "ignored"}

    treasures = db.session.execute(db.select(Level2Treasure)).scalars()
    for treasure in treasures:
        treasure.collected_by = None

    players = db.session.execute(db.select(Player)).scalars()
    for player in players:
        db.session.delete(player)

    level1_collection_logs = db.session.execute(db.select(Level1TreasureCollectors)).scalars()
    for collection_log in level1_collection_logs:
        db.session.delete(collection_log)

    db.session.commit()
    return jsonify({"result": "OK"})


@app.route("/get_all_players")
def get_all_players():
    players = db.session.execute(db.select(Player)).scalars()
    result = dict()
    result["tally"] = dict()
    for player in players:
        result[player.mac_address] = "CLAN " + str(player.clan_id) + " ID " + str(player.participant_id)
        if player.clan_id not in result["tally"]:
            result["tally"][player.clan_id] = 0
        result["tally"][player.clan_id] += 1

    return jsonify(result)


@app.route("/player_score", methods=["POST"])
def update_player_score():
    content = request.json
    print("UPDATE SCORE:", content)
    if "clan" in content and "id" in content and "num_kills" in content and "level1" in content and "level2" in content:
        clan_id = content["clan"]
        ID = content["id"]
        num_kills = content["num_kills"]
        num_level1_treasures = content["level1"]
        num_level2_treasures = content["level2"]
        player = db.session.execute(db.select(Player).filter_by(clan_id=clan_id, participant_id=ID)).scalar_one_or_none()
        if player:
            player.num_kills = int(num_kills)
            player.num_level1_treasures_wanderer = num_level1_treasures
            player.num_level2_treasures_wanderer = num_level2_treasures
            result = {"clan_id": int(clan_id), "ID": int(ID), "sent_kills": int(num_kills),
                      "level1": num_level1_treasures, "level2": num_level2_treasures,
                      "num_powerups": 0,
                      "num_kills": 0
                      }
            db.session.commit()
            return jsonify(result)

    abort(404)


@app.route("/treasure_score", methods=["POST"])
def update_level1_treasure_score():
    content = request.json
    print("UPDATE SCORE:", content)
    if "name" in content and "invicta" in content and "dynari" in content and "ephilia" in content and "akrona" in content and "solaris" in content:
        name = content["name"].strip()
        invicta = int(content["invicta"].strip())
        dynari = int(content["dynari"].strip())
        ephilia = int(content["ephilia"].strip())
        akrona = int(content["akrona"].strip())
        solaris = int(content["solaris"].strip())

        treasure = db.session.execute(db.select(Level1Treasure).filter_by(name=name)).scalar_one_or_none()
        if treasure:
            treasure.num_invicta_collected = invicta
            treasure.num_dynari_collected = dynari
            treasure.num_ephilia_collected = ephilia
            treasure.num_akrona_collected = akrona
            treasure.num_solaris_collected = solaris
            db.session.commit()
            return jsonify({"name": name, "invicta": invicta, "dynari": dynari, "ephilia": ephilia, "akrona": akrona, "solaris": solaris})

    abort(404)


@app.route("/score")
def calculate_score():
    clan_score = dict()
    tally = dict()
    for i in range(0, 4):
        clan_score[i] = 0
        tally[i] = dict()
        for category in ("kills", "level1_treasure", "level2_treasure"):
            tally[i][category] = 0

    # calculate player kills
    players = db.session.execute(db.select(Player)).scalars()
    for player in players:
        clan_id = player.clan_id
        kills = player.num_kills
        clan_score[clan_id] += (kills * 4)
        tally[clan_id]["kills"] += kills

    level2_treasures = db.session.execute(db.select(Level2Treasure)).scalars()
    for treasure in level2_treasures:
        collected_player = treasure.collected_by
        if collected_player and treasure.is_treasure:
            clan_id = collected_player.clan_id
            clan_score[clan_id] += 50
            tally[clan_id]["level2_treasure"] += 1

    level1_treasures_log = db.session.execute(db.select(Level1TreasureCollectors)).scalars()
    for collection_log in level1_treasures_log:
        player = collection_log.player
        clan_id = player.clan_id
        clan_score[clan_id] += 1
        tally[clan_id]["level1_treasure"] += 1

    return jsonify({"tally": tally, "score": clan_score})


@app.route("/score_player_side")
def calculate_score_from_player_side_stats():
    OG_score = dict()
    tally = dict()
    for i in range(0, 4):
        OG_score[i] = 0
        tally[i] = dict()
        for category in ("kills", "level1_treasure", "level2_treasure"):
            tally[i][category] = 0

    # calculate player kills
    players = Player.query.all()
    for player in players:
        og = player.clan_id
        kills = player.num_kills
        level1_treasure = player.num_level1_treasures_wanderer
        level2_treasure = player.num_level2_treasures_wanderer
        OG_score[og] += (kills * 4)
        tally[og]["kills"] += kills

        OG_score[og] += (level1_treasure * 1)
        tally[og]["level1_treasure"] += level1_treasure

        OG_score[og] += (level2_treasure * 50)
        tally[og]["level2_treasure"] += level2_treasure

    return jsonify({"tally": tally, "score": OG_score})


@app.route("/player_kill/<int:og>/<int:participant_id>")
def upload_failed_player_kill(og, participant_id):
    print("Received Failed Player Kill by OG {} ID {}".format(og, participant_id))
    player = Player.query.filter_by(OG=og, participant_id=participant_id).first()
    if player:
        player.num_temp_failed_kills += 1
        db.session.commit()
        return jsonify({"result": "OK"})
    abort(404)


@app.route("/treasurefeedback/<name>/<int:og>/<int:participant_id>")
def upload_failed_treasure_feedback(name, og, participant_id):
    print("Received Failed Level1Treasure Collection ({}) by OG {} ID {}".format(name, og, participant_id))
    player = Player.query.filter_by(clan_id=og, participant_id=participant_id).first()
    treasure = Level1Treasure.query.filter_by(name=name).first()
    if treasure and player:
        collection_log = Level1TreasureCollectors(player, treasure)
        db.session.add(collection_log)
        player.num_temp_failed_treasure1_feedback += 1
        db.session.commit()
        return jsonify({"result": "OK"})
    abort(404)


@app.route("/treasure_feedback/<name>", methods=["POST"])
def upload_failed_treasure_feedback_post(name):
    content = request.json
    print("UPDATE SCORE:", content)
    if "OG" in content and "ID" in content:
        og = content["OG"]
        participant_id = content["ID"]
        print("Received Failed Level1Treasure Collection ({}) by OG {} ID {}".format(name, og, participant_id))
        player = Player.query.filter_by(OG=og, participant_id=participant_id).first()
        treasure = Level1Treasure.query.filter_by(name=name).first()
        if treasure and player:
            collection_log = Level1TreasureCollectors(player, treasure)
            db.session.add(collection_log)
            player.num_temp_failed_treasure1_feedback += 1
            db.session.commit()
            return jsonify({"result": "OK"})
    abort(404)


@app.route("/get_failed_feedback_count")
def get_failed_feedback_count():
    players = Player.query.all()
    result = dict()
    for player in players:
        og = player.clan_id
        participant_id = player.participant_id
        key_string = "OG {} ID {}".format(og, participant_id)
        result[key_string] = "KILLS {} LEVEL1TREASURE {}".format(player.num_temp_failed_kills,
                                                                 player.num_temp_failed_treasure1_feedback)
    return jsonify(result)