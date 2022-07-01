#ifndef HTML_H
#define HTML_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <meta content="text/html" ; charset="UTF-8" ; http-equiv="content-type">
    <meta name="viewport" content="width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
    <title>RTK base setup</title>
    <style>
        body {
            background-color: #4180C8;
            font-family: Arial, Helvetica, sans-serif;
            Color: #ffffff;
            text-align: center;
            border: 1em;
        }

        .center {
            margin-left: auto;
            margin-right: auto;
        }

        .button {
            background-color: #F0A03C;
            border: none;
            color: white;
            padding: 13px 18px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }

        .text_field {
            border: none;
            color: black;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }

        ::placeholder {
            color: navy;
            opacity: 1;
            transition: opacity 1s;
        }

        :focus::placeholder {
            opacity: 0
        }
    </style>
</head>
<script>
    function loadRadioState() {
        if (document.getElementById("radio_state").value == "survey_enabled") {
            (document.getElementById("survey_enabled").checked = true);
        } else {
            (document.getElementById("coords_enabled").checked = true);
        }
        enableChoosenMethod();
    }

    function enableChoosenMethod() {
        if (document.getElementById("survey_enabled").checked == false) {
            document.getElementById("survey_accuracy").disabled = true;
            document.getElementById("latitude").disabled = false;
            document.getElementById("longitude").disabled = false;
            document.getElementById("height").disabled = false;
        } else {
            document.getElementById("survey_accuracy").disabled = false;
            document.getElementById("latitude").disabled = true;
            document.getElementById("longitude").disabled = true;
            document.getElementById("height").disabled = true;
        }
    }
</script>

<body onload="loadRadioState()">
    <form id="Form1" onsubmit="return confirm('Please restart the ESP32 board by pressing the Reboot button after saving your data!');" action='actionUpdateData' method='post' target="hidden-form"></form>
    <form id="Form2" onsubmit="return confirm('Are you sure? All saved SPIFFS files will be deleted (Wifi and RTK config). The device will restart in AP mode.');" action='actionWipeData' method='post' target="hidden-form"></form>
    <form id="Form3" onsubmit="return confirm('Connection will be lost during reboot, please refresh this page after reconnecting!');" action='actionReboot' method='post' target="hidden-form"></form>
    <p>
        <table class=center>
            <tr>
                <td colspan=2>
                    <h2>RTK base station</h2>
                </td>
                <td colspan=2></td>
            </tr>
            <tr>
                <td colspan=2>
                    <h3>WiFi credentials</h3>
                </td>
                <td colspan=2></td>
            </tr>
            <tr>
                <td style="text-align:left;">SSID:</td>
                <td>
                    <input class="text_field" form="Form1" type="text" maxlength="30" name="ssid" placeholder=%ssid% style="text-align:center;">
                </td>
            </tr>
            <tr>
                <td style="text-align:left;">Password:</td>
                <td>
                    <input class="text_field" form="Form1" type="text" maxlength="30" name="password" placeholder=%password% style="text-align:center;">
                </td>
            </tr>
            <tr>
                <td></td>
                <td style="text-align:right;"> </td>
            </tr>
            <tr>
                <td colspan=2>
                    <h3>RTK configuration</h3>
                </td>
                <td colspan=2></td>
            </tr>
            <tr>
                <td colspan=2>
                    Choose location method:
                </td>
                <td colspan=2></td>
            </tr>
            <tr>
                <td colspan=2> <input form="Form1" type="hidden" id="radio_state" value=%location_method% </td>
            </tr>
            <tr>
                <td style="text-align:right;"> <input form="Form1" type="radio" id="survey_enabled" value="survey_enabled" name="location_method" onclick="enableChoosenMethod()"></td>
                <td style="text-align:left;" title="For an accuracy of 0.06 m this can take up to 24 hours."> <label for="survey_enabled"> Run a long survey </label></td>
            </tr>
            <tr>
                <td style="text-align:right;"> <input form="Form1" type="radio" id="coords_enabled" value="coords_enabled" name="location_method" onclick="enableChoosenMethod()"></td>
                <td style="text-align:left;" title="Seven decimal places are required."> <label for="coords_enabled"> High precision coordinates </label></td>
            </tr>
            <td colspan=2></td>
            <tr>
                <td style="text-align:left;"> Accuracy in m: </td>
                <td><input title="The survey is carried out until the desired accuracy is achieved. After that, the location coordinates are stored in SPIFFS. 0.06 m is a useful value." class="text_field" form="Form1" type="text" maxlength="30" id="survey_accuracy" name="survey_accuracy" placeholder=%survey_accuracy%></td>
            </tr>
            <tr>
                <td style="text-align:left;"> Latitude: </td>
                <td><input class="text_field" form="Form1" type="text" maxlength="30" id="latitude" name="latitude" placeholder=%latitude%></td>
            </tr>
            <tr>
                <td style="text-align:left;"> Longitude: </td>
                <td><input class="text_field" form="Form1" type="text" maxlength="30" id="longitude" name="longitude" placeholder=%longitude%></td>
            </tr>
            <tr>
                <td style="text-align:left;"> Height in m: </td>
                <td><input title="Height over sea-level of the antenna is required (float)." class="text_field" form="Form1" type="text" maxlength="30" id="height" name="height" placeholder=%height%></td>
            </tr>
        </table>
    </p>
    <br>
    <div>
        <input type="submit" form="Form1" formaction="/actionUpdateData" class="button" value="Save" id="save_button" name="save_button" />
        <input type="submit" form="Form3" value="Reboot" class="button" />
        <input type="reset" form="Form1" value="Cancel" class="button" />
        <input type="submit" form="Form2" formaction="/actionWipeData" class="button" value="Wipe" id="wipe_button" name="wipe_button" />
    </div>
</body>

</html>
)rawliteral";
#endif /* HTML_H */