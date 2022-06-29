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
                mac_address = player.mac_address
                return jsonify({"mac_address": mac_address})

    abort(404)


@app.route("/player/<int:identifier>")
def get_player_mac_address(identifier):
    player_details = retrieve_OG_participant_id_from_identifier(identifier)
    player = Player.query.filter_by(OG=player_details["OG"], participant_id=player_details["participant_id"]).first()
    if player:
        result = dict()
        mac_address_str = player.mac_address
        result["mac_address"] = mac_address_str
        for i, mac_address_part in enumerate(mac_address_str.split(":")):
            result["mac_address_part" + str(5 - i)] = str(int(mac_address_part, 16))
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
        player = Player(OG=content["OG"], mac_address=content["mac_address"])
        db.session.add(player)
        db.session.commit()
        return {"result": "OK"}

    abort(404)


@app.route("/game_status")
def has_game_started():
    status = GameStatus.query.filter_by(name="started").first()
    return jsonify({"has_game_started": status.value})  # for testing purposes


@app.route("/start_game")
def start_game():
    # assign all players their IDs
    players = Player.query.all()
    og_counter = dict()
    result = dict()
    for player in players:
        if player.OG not in og_counter:
            og_counter[player.OG] = 0
        player.participant_id = og_counter[player.OG]
        og_counter[player.OG] += 1
        result[player.mac_address] = (player.OG, player.participant_id)

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
        if collected_player:
            result[treasure.name] = collected_player.mac_address
        else:
            result[treasure.name] = "<empty>"

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
