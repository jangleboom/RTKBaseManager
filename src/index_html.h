#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <meta content="text/html" ; charset="UTF-8" ; http-equiv="content-type">
    <meta name="viewport" content="width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
    <title>RTK base setup</title>
    <style>
        body 
        {
            background-color: #241E4E;
            background-image: radial-gradient(#15112C, #241E4E, #312966);
            font-family: Lato, Helvetica, Roboto, sans-serif;
            color: #DFDFDF;
            text-align: center;
            border: 1em;
        }

        .table 
        {
            margin-left: auto;
            margin-right: auto;
        }

        .button 
        {
            border-radius: 4px;
            border: none;
            padding: 13px 18px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }
.blue 
        {
            background-color: #40798C;
            color: #241E4E;
        }
        .green 
        {
            background-color: #399E5A;
            color: #241E4E;
        }
        .red
        {
            background-color: #CE6C47;
            color: #241E4E;
        }

        .text_field 
        {
            border-radius: 4px;
            border: none;
            color: black;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }

        ::placeholder 
        {
            color: black;
            opacity: 1;
            transition: opacity 1s;
        }

        :focus::placeholder 
        {
            opacity: 0.1;
        }
    </style>
</head>
<script>
    function loadRadioState() 
    {
        if (document.getElementById("radio_state").value == "survey_enabled") 
        {
            (document.getElementById("survey_enabled").checked = true);
        } else 
        {
            (document.getElementById("coords_enabled").checked = true);
        }
    }

    function enableLocationMethod() 
    {
        if (document.getElementById("survey_enabled").checked == false) 
        {
            document.getElementById("survey_accuracy").disabled = true;
            document.getElementById("latitude").disabled = false;
            document.getElementById("longitude").disabled = false;
            document.getElementById("altitude").disabled = false;
            document.getElementById("coord_accuracy").disabled = false;
        } else 
        {
            document.getElementById("survey_accuracy").disabled = false;
            document.getElementById("latitude").disabled = true;
            document.getElementById("longitude").disabled = true;
            document.getElementById("altitude").disabled = true;
            document.getElementById("coord_accuracy").disabled = true;
        }
    }
</script>

<body onload="loadRadioState();enableLocationMethod();">

    <form id="Form1" onsubmit="return confirm('Restart the ESP32 by pressing the Reboot button for your changes to take effect!');" action='actionUpdateData' method='post' target="hidden-form"></form>
    <form id="Form2" onsubmit="return confirm('Are you sure? All saved LittleFS files will be deleted (Wifi and RTK config).');" action='actionWipeData' method='post' target="hidden-form"></form>
    <form id="Form3" onsubmit="return confirm('Connection will be lost during reboot, please refresh this page after reconnecting!');" action='actionRebootESP32' method='post' target="hidden-form"></form>
    <input form="Form1" type="hidden" id="radio_state" value=%location_method%>
    <p>
        <table class="table">
            <tr>
                <td colspan=2>
                    <h2>RTK Base Station</h2>
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
                <td colspan=2> </td>
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
                <td colspan=2>
                    <h3>RTK base setup</h3>
                </td>
                <tr>
                    <td style="text-align:left;">Caster host:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="caster_host" placeholder=%caster_host% style="text-align:center;">
                    </td>
                </tr>
                <tr>
                    <td style="text-align:left;">Caster port:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="caster_port" placeholder=%caster_port% style="text-align:center;">
                    </td>
                </tr>
                <tr>
                    <td style="text-align:left;">Mount point:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="mount_point" placeholder=%mount_point% style="text-align:center;">
                    </td>
                </tr>
                <tr>
                    <td style="text-align:left;">Mount point PW:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="mount_point_pw" placeholder=%mount_point_pw% style="text-align:center;">
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
                    <td style="text-align:right;"> <input form="Form1" type="radio" id="survey_enabled" value="survey_enabled" name="location_method" onclick="enableLocationMethod()"></td>
                    <td style="text-align:left;" title="For an accuracy of 0.06 m this can take up to 24 hours."> <label for="survey_enabled"> Run a survey-in </label></td>
                </tr>
                <tr>
                    <td style="text-align:right;"> <input form="Form1" type="radio" id="coords_enabled" value="coords_enabled" name="location_method" onclick="enableLocationMethod()"></td>
                    <td style="text-align:left;" title="Seven decimal places are required."> <label for="coords_enabled"> Enter high precision coords </label></td>
                </tr>
                <td colspan=2></td>
                <tr>
                    <td style="text-align:left;"> Survey-in accuracy, m: </td>
                    <td><input title="The survey is carried out at minimum for 60 s or until the desired accuracy is achieved." class="text_field" form="Form1" type="text" maxlength="30" id="survey_accuracy" name="survey_accuracy" placeholder=%survey_accuracy%></td>
                </tr>
                <tr>
                    <td style="text-align:left;"> Latitude, deg: </td>
                    <td><input class="text_field" form="Form1" type="text" maxlength="30" id="latitude" name="latitude" placeholder=%latitude%></td>
                </tr>
                <tr>
                    <td style="text-align:left;"> Longitude, deg: </td>
                    <td><input class="text_field" form="Form1" type="text" maxlength="30" id="longitude" name="longitude" placeholder=%longitude%> </td>
                </tr>
                <tr>
                    <td style="text-align:left;"> Altitude, m: </td>
                    <td><input title="Ellipsoid height of the antenna is required (float)." class="text_field" form="Form1" type="text" maxlength="30" id="altitude" name="altitude" placeholder=%altitude%></td>
                </tr>
                <tr>
                    <td style="text-align:left;"> Accuracy, m: </td>
                    <td><input title="Optional: You can save your data quality to remember later." class="text_field" form="Form1" type="text" maxlength="30" id="coord_accuracy" name="coord_accuracy" placeholder=%coord_accuracy%></td>
                </tr>
        </table>
    </p>
    <br>
    <br>
    <div>
        <input type="submit" form="Form1" class="button green" formaction="/actionUpdateData" value="Save" id="save_button" name="save_button" />
        <input type="submit" form="Form3" class="button green" formaction="/actionRebootESP32" value="Reboot" id="reboot_button" name="reboot_button" />
        <input type="reset" form="Form1" class="button blue" value="Cancel" />
        <input type="submit" form="Form2" class="button red" formaction="/actionWipeData" value="Wipe" id="wipe_button" name="wipe_button" />
    </div>
    <br>
    <br>
</body>

</html>
)rawliteral";

#endif /* INDEX_HTML_H */


