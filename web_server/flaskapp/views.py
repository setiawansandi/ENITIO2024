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


@app.route("/treasure/<name>", methods=["POST"])
def set_treasure_as_collected(name):
    content = request.json
    if "player_identifier" in content:
        player_identifier = content["player_identifier"]
        treasure = Treasure.query.filter_by(name=name).first()
        if treasure:
            player_details = retrieve_OG_participant_id_from_identifier(player_identifier)
            player = Player.query.filter_by(OG=player_details["OG"], participant_id=player_details["participant_id"]).first()
            if player:
                treasure.collected_by = player
                db.session.commit()

                # return MAC address to treasure to send confirmation message
                OG_hex_str = "{0:02x}".format(player_details["OG"])
                participant_id_hex_str = "{0:02x}".format(player_details["participant_id"])
                result = {"mac_address_part5": "04",
                          "mac_address_part4": "08",
                          "mac_address_part3": "22",
                          "mac_address_part2": "01",
                          "mac_address_part1": OG_hex_str,
                          "mac_address_part0": participant_id_hex_str,
                          "mac_address": "04:08:22:01:{}:{}".format(OG_hex_str, participant_id_hex_str)}

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
                  "mac_address_part3": "22",
                  "mac_address_part2": "01",
                  "mac_address_part1": f"{OG_hex_str: 0>2}",
                  "mac_address_part0": f"{participant_id_hex_str: 0>2}",
                  "mac_address": f"04:08:22:01:{OG_hex_str: 0>2}:{participant_id_hex_str: 0>2}"}

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
@app.route("/unset_treasure/<name>")
def debug_uncollect_treasure(name):
    treasure = Treasure.query.filter_by(name=name).first()
    if treasure:
        treasure.collected_by = None
        db.session.commit()
        return jsonify({"mac_address": "<empty>"})


@app.route("/treasure_enquiry/<name>")
def get_player_who_collected_treasure(name):
    treasure = Treasure.query.filter_by(name=name).first()
    if treasure:
        collected_player = treasure.collected_by
        if collected_player:
            return jsonify({"mac_address": collected_player.mac_address})
        else:
            return jsonify({"mac_address": "<empty>"})


@app.route("/treasure_enquiry_all")
def get_all_treasure_status():
    treasures = Treasure.query.all()
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
    treasures = Treasure.query.all()
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
