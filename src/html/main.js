function showInfoModal(text) {
    const dialog = document.getElementById("infoModal");
    let infoModalText = document.getElementById("infoModalText");
    infoModalText.innerText = text;

    dialog.showModal();
}

async function send(messageType, value) {
    let message = {
        "type": messageType,
        "value": value
    };

    try {
        let results = await fetch("/set", {
            method: "POST",
            headers: { "Content-type": "application/json" },
            body: JSON.stringify(message)
        });

        if (results.status != 200) {
            showInfoModal("Communication error");
        }
    }
    catch (error) {
        showInfoModal("Communication error");
    }
}

function sendMessage(group, messageType, value) {
    send(messageType, value);
    event.target.disabled = true;
}

function timerChanged(selectedTime) {
    let valuesMapping = {
        "Off": 0,
        "1H": 1,
        "2H": 2,
        "3H": 3,
        "4H": 4,
        "5H": 5,
        "6H": 6,
        "7H": 7,
        "8H": 8,
        "9H": 9,
        "10H": 10,
        "11H": 11,
        "12H": 12
    };

    let selectedValue = selectedTime.value;
    send("timer", valuesMapping[selectedValue])
}

function closeModal(modalName) {
    const dialog = document.getElementById(modalName);
    dialog.close();
}

function getButton(id) {
    return document.getElementById(id);
}

function resetControls(controls) {
    for (let cntrl in controls) {
        if (controls.hasOwnProperty(cntrl) === false) continue;

        let buttons = controls[cntrl]
        for (let id in buttons) {
            let button = buttons[id];
            button.disabled = false;
        }
    }
}

function setButtonsState(configuration, controls) {
    for (var cntrl in configuration) {
        if (configuration.hasOwnProperty(cntrl) === false) continue;
        if (controls.hasOwnProperty(cntrl) === false) continue;

        let value = configuration[cntrl];
        if (controls[cntrl].hasOwnProperty(value) === false) continue;

        controls[cntrl][value].disabled = true;
    }
}

window.addEventListener('load', function () {
    let controls = {
        "power": {
            0: getButton("btn_off"),
            1: getButton("btn_on")
        },
        "humidification_power": {
            0: getButton("hpow_0"),
            1: getButton("hpow_1"),
            2: getButton("hpow_2"),
            3: getButton("hpow_3"),
        },
        "humidification_level": {
            1: getButton("hlvl_1"),
            2: getButton("hlvl_2"),
            3: getButton("hlvl_3"),
            4: getButton("hlvl_4"),
            5: getButton("hlvl_5"),
            6: getButton("hlvl_6"),
            7: getButton("hlvl_7"),
            8: getButton("hlvl_8")
        },
        "night_mode": {
            0: getButton("night_0"),
            1: getButton("night_1"),
        },
        "auto_mode": {
            0: getButton("auto_0"),
            1: getButton("auto_1"),
        },
        "light": {
            0: getButton("light_0"),
            1: getButton("light_1"),
            2: getButton("light_2"),
            3: getButton("light_3"),
        }
    }

    let configuration = {
        "power": 1,
    };

    resetControls(controls);
    setButtonsState(configuration, controls);

    console.log(controls)

    if (!!window.EventSource) {
        var source = new EventSource('/events');

        source.addEventListener('open', function (e) {
            console.log("Connected");
        }, false);

        source.addEventListener('error', function (e) {
            if (e.target.readyState != EventSource.OPEN) {
                console.log("Disconnected");
            }
        }, false);

        source.addEventListener('humidifierState', function (e) {
            const change = JSON.parse(e.data);

            if (change.hasOwnProperty("humidity_lvl")) {
                const humidity = document.getElementById("humidity");
                humidity.value = change["humidity_lvl"] + "%";
            }

            if (change.hasOwnProperty("water_lvl")) {
                const water_container = document.getElementById("water_container");
                if (change["water_lvl"] == 1) {
                    water_container.value = "full";
                }
                else {
                    water_container.value = "empty";
                }
            }

            if (change.hasOwnProperty("timer")) {
                const timer = document.getElementById("timer_select");
                const toSelect = change["timer"];
                timer.options[toSelect].selected = true;
            }

            for (var key in change) {
                configuration[key] = change[key];
            }

            resetControls(controls);
            setButtonsState(configuration, controls);
        }, false);
    }
})
