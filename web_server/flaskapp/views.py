from flask import request, jsonify, abort, render_template, redirect, url_for
from flaskapp import app
from flaskapp.models import *


def retrieve_OG_participant_id_from_identifier(identifier):
    # Identifier has 3 hexadecimal digits
    # First digit is OG
    # Next 2 digits is ID
    OG = int(identifier / (16 ** 2))
    participant_id = identifier % 16
    return dict(OG=OG, participant_id=participant_id)


@app.route("/treasure/<int:level>/<name>", methods=["POST"])
def set_treasure_as_collected(level, name):
    content = request.json
    if "player_identifier" in content:
        player_identifier = content["player_identifier"]
        if level == 2:
            treasure = Level2Treasure.query.filter_by(name=name).first()
            if treasure:
                player_details = retrieve_OG_participant_id_from_identifier(player_identifier)
                player = Player.query.filter_by(OG=player_details["OG"], participant_id=player_details["participant_id"]).first()
                print(treasure.name, "collected by OG", player.OG, "ID", player.participant_id)
                if player:
                    treasure.collected_by = player
                    db.session.commit()
                    print("Committed")

                    # return MAC address to treasure to send confirmation message
                    OG_hex_str = "{0:02x}".format(player_details["OG"])
                    participant_id_hex_str = "{0:02x}".format(player_details["participant_id"])
                    result = {"mac_address_part5": "04",
                              "mac_address_part4": "08",
                              "mac_address_part3": "01",
                              "mac_address_part2": OG_hex_str,
                              "mac_address_part1": participant_id_hex_str,
                              "mac_address_part0": "01",
                              "mac_address": "04:08:01:{}:{}:01".format(OG_hex_str, participant_id_hex_str)}

                    return jsonify(result)
        elif level == 1:
            treasure = Level1Treasure.query.filter_by(name=name).first()
            player_details = retrieve_OG_participant_id_from_identifier(player_identifier)
            player = Player.query.filter_by(OG=player_details["OG"],
                                            participant_id=player_details["participant_id"]).first()
            if player and treasure:
                print("Uploading Treasure Level 1 Collection ({}) by Player OG {} ID {}".format(treasure.name, player.OG, player.participant_id))
                treasure.collected_players.append(player)
            db.session.commit()
            print("Committed")

            # return MAC address to treasure to send confirmation message
            OG_hex_str = "{0:02x}".format(player_details["OG"])
            participant_id_hex_str = "{0:02x}".format(player_details["participant_id"])
            result = {"mac_address_part5": "04",
                      "mac_address_part4": "08",
                      "mac_address_part3": "01",
                      "mac_address_part2": OG_hex_str,
                      "mac_address_part1": participant_id_hex_str,
                      "mac_address_part0": "01",
                      "mac_address": "04:08:01:{}:{}:01".format(OG_hex_str, participant_id_hex_str)}

            return jsonify(result)

    abort(404)


@app.route("/treasure/<int:level>/<name>/<int:OG>/<int:participant_id>", methods=["GET"])
def set_treasure_as_collected(level, name, og, participant_id):
    player = Player.query.filter_by(OG=og, participant_id=participant_id).first()
    if player:
        if level == 2:
            treasure = Level2Treasure.query.filter_by(name=name).first()
            if treasure:
                print(treasure.name, "collected by OG", player.OG, "ID", player.participant_id)
                treasure.collected_by = player
                db.session.commit()
                print("Committed")

        elif level == 1:
            treasure = Level1Treasure.query.filter_by(name=name).first()
            if treasure:
                print("Uploading Treasure Level 1 Collection ({}) by Player OG {} ID {}".format(treasure.name, player.OG, player.participant_id))
                treasure.collected_players.append(player)
            db.session.commit()
            print("Committed")

        # return MAC address to treasure to send confirmation message
        OG_hex_str = "{0:02x}".format(og)
        participant_id_hex_str = "{0:02x}".format(participant_id)
        result = {"mac_address_part5": "04",
                  "mac_address_part4": "08",
                  "mac_address_part3": "01",
                  "mac_address_part2": OG_hex_str,
                  "mac_address_part1": participant_id_hex_str,
                  "mac_address_part0": "01",
                  "mac_address": "04:08:01:{}:{}:01".format(OG_hex_str, participant_id_hex_str)}
        return jsonify(result)

    abort(404)


@app.route("/player/<int:identifier>")
def get_player_mac_address(identifier):
    player_details = retrieve_OG_participant_id_from_identifier(identifier)
    player = Player.query.filter_by(OG=player_details["OG"], participant_id=player_details["participant_id"]).first()
    if player and player.participant_id:
        OG_hex_str = "{0:x}".format(player.OG)
        participant_id_hex_str = "{0:x}".format(player.participant_id)
        result = {"mac_address_part5": "04",
                  "mac_address_part4": "08",
                  "mac_address_part3": "01",
                  "mac_address_part2": OG_hex_str,
                  "mac_address_part1": participant_id_hex_str,
                  "mac_address_part0": "01",
                  "mac_address": "04:08:01:{}:{}:01".format(OG_hex_str, participant_id_hex_str)}

        return jsonify(result)
    else:
        abort(404)


@app.route("/player_id/<int:OG>/<mac_addr>")
def get_player_id(OG, mac_addr):
    result = dict()
    player = Player.query.filter_by(OG=OG, mac_address=mac_addr).first()
    if player:
        result["player_id"] = player.participant_id
        return jsonify(result)

    abort(404)


@app.route("/player_registration", methods=["POST"])
def register_player():
    content = request.json
    if "OG" in content and "mac_address" in content:
        OG = content["OG"]
        mac_addr = content["mac_address"]
        player = Player.query.filter_by(OG=OG, mac_address=mac_addr).first()
        if player:
            return {}
        else:
            player = Player(OG=OG, mac_address=mac_addr)
            db.session.add(player)
            db.session.commit()
            return {"result": "OK"}

    abort(404)


@app.route("/game_status")
def has_game_started():
    status = GameStatus.query.filter_by(name="started").first()
    return jsonify({"has_game_started": int(status.value)})


@app.route("/start_game")
def start_game():
    # assign all players their IDs
    players = Player.query.all()
    og_counter = dict()
    result = dict()
    for player in players:
        if player.OG not in og_counter:
            og_counter[player.OG] = 0
        player.participant_id = og_counter[player.OG] + 1
        og_counter[player.OG] += 1
        result[player.mac_address] = f"OG {player.OG} ID {player.participant_id}"

    status = GameStatus.query.filter_by(name="started").first()
    status.value = "1"
    db.session.commit()

    return jsonify(result)


@app.route("/stop_game")
def stop_game():
    status = GameStatus.query.filter_by(name="started").first()
    status.value = "0"
    db.session.commit()

    return {"has_game_started": "0"}


# for testing purposes
@app.route("/treasure_enquiry_all")
def get_all_treasure_status():
    treasures = Level2Treasure.query.all()
    result = dict()
    for treasure in treasures:
        collected_player = treasure.collected_by
        result[treasure.name] = "TREASURE " if treasure.is_treasure else "VIRUS "
        if collected_player:
            result[treasure.name] += f"Collected by OG {collected_player.OG} ID {collected_player.participant_id}"
        else:
            result[treasure.name] += "<available>"

    return jsonify(result)


@app.route("/unset_all_player_ids")
def unset_all_player_ids():
    players = Player.query.all()
    for player in players:
        player.participant_id = None

    # ensure game status is False
    status = GameStatus.query.filter_by(name="started").first()
    status.value = "0"
    db.session.commit()
    return {"result": "OK"}


@app.route("/delete_all_players")
def delete_all_players():
    treasures = Level2Treasure.query.all()
    for treasure in treasures:
        treasure.collected_by = None

    players = Player.query.all()
    for player in players:
        db.session.delete(player)

    db.session.commit()
    return {"result": "OK"}


@app.route("/get_all_players")
def get_all_players():
    players = Player.query.all()
    result = dict()
    for player in players:
        result[player.mac_address] = "OG " + str(player.OG) + " ID " + str(player.participant_id)

    return jsonify(result)


@app.route("/get_all_game_variables")
def get_all_game_variables():
    variables = GameStatus.query.all()
    result = dict()
    for var in variables:
        result[var.name] = int(var.value)

    return jsonify(result)


@app.route("/num_kills", methods=["POST"])
def update_num_kills():
    content = request.json
    if "OG" in content and "ID" in content and "num_kills" in content:
        OG = content["OG"]
        ID = content["ID"]
        num_kills = content["num_kills"]
        player = Player.query.filter_by(OG=OG, participant_id=ID).first()
        if player:
            player.num_kills = int(num_kills)
            db.session.commit()
            return jsonify({"OG": int(OG), "ID": int(ID), "num_kills": int(num_kills)})

    abort(404)


@app.route("/score")
def calculate_score():
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
        og = player.OG
        kills = player.num_kills
        OG_score[og] += (kills * 4)
        tally[og]["kills"] += kills

    level2_treasures = Level2Treasure.query.all()
    for treasure in level2_treasures:
        collected_player = treasure.collected_by
        if collected_player:
            og = collected_player.OG
            OG_score[og] += 50
            tally[og]["level2_treasure"] += 1

    level1_treasures = Level1Treasure.query.all()
    for treasure in level1_treasures:
        for player in treasure.collected_players:
            og = player.OG
            OG_score[og] += 4
            tally[og]["level1_treasure"] += 1

    return jsonify({"tally": tally, "score": OG_score})
