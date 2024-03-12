function showInfoModal(text) {
    const dialog = document.getElementById("infoModal");
    let infoModalText = document.getElementById("infoModalText");
    infoModalText.innerText = text;

    dialog.showModal();
}

function switchButton(clickedButton, groupName) {
    let container = document.getElementById(groupName);
    let allButtons = container.querySelectorAll('button');
    allButtons.forEach(function (button) {
        button.disabled = false;
    });

    clickedButton.disabled = true;
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

    switchButton(event.target, group);
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