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
        :root {
            --RUSSIAN_VIOLETT: #241E4E;
            --GREEN_COLOR: #399E5A;
            --RED_COLOR: #CE6C47;
            --GAINSBORO: #DFDFDF;
            --TEAL_BLUE: #40798C;
        }

        body {
            background-color: var(--RUSSIAN_VIOLETT);
            color: var(--GAINSBORO);
            font-family: Lato, Helvetica, Roboto, sans-serif;
            text-align: center;
            border: 1em;
        }

        .table {
            margin-left: auto;
            margin-right: auto;
        }

        .button {
            border-radius: 4px;
            border: none;
            padding: 15px 30px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
        }

        .blue {
            background-color: var(--TEAL_BLUE);
            color: var(--RUSSIAN_VIOLETT);
        }

        .green {
            background-color: var(--GREEN_COLOR);
            color: var(--RUSSIAN_VIOLETT);
        }

        .red {
            background-color: var(--RED_COLOR);
            color: var(--RUSSIAN_VIOLETT);
        }

        .text_field {
            border-radius: 4px;
            border: none; // color: black;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 13px;
            width: 200px;
        }

        /* set the opacity of placeholder */

        ::placeholder {
            opacity: 1;
        }

        :focus::placeholder {
            opacity: 0.1;
        }
    </style>
</head>
<script>
function checkVariables()
{
    // check if variable exists
    let ssid            = "%ssid%";
    let password        = "%password%";
    let device_name       = "%device_name%";
    let caster_host     = "%caster_host%";
    let caster_port     = "%caster_port%";
    let mount_point     = "%mount_point%";
    let mount_point_pw  = "%mount_point_pw%";
    let survey_accuracy = "%survey_accuracy%";
    let latitude        = "%latitude%";
    let longitude       = "%longitude%";
    let altitude        = "%altitude%";
    let coord_accuracy  = "%coord_accuracy%";

    if (ssid) 
    {
        document.getElementsByName("ssid")[0].placeholder = ssid;
        document.getElementsByName("ssid")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("ssid")[0].placeholder = "Enter WiFi SSID";
        document.getElementsByName("ssid")[0].style.color = "grey";
    }

    if (password) 
    {
        document.getElementsByName("password")[0].placeholder = password;
        document.getElementsByName("password")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("password")[0].placeholder = "Enter WiFi password";
        document.getElementsByName("password")[0].style.color = "grey";
    }

    if (device_name) 
    {
        document.getElementsByName("device_name")[0].placeholder = device_name;
        document.getElementsByName("device_name")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("device_name")[0].placeholder = "Enter a custom host name";
        document.getElementsByName("device_name")[0].style.color = "grey";
    }

    if (caster_host) 
    {
        document.getElementsByName("caster_host")[0].placeholder = caster_host;
        document.getElementsByName("caster_host")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("caster_host")[0].placeholder = "Enter caster host";
        document.getElementsByName("caster_host")[0].style.color = "grey";
    }

    if (caster_port) 
    {
        document.getElementsByName("caster_port")[0].placeholder = caster_port;
        document.getElementsByName("caster_port")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("caster_port")[0].placeholder = "Enter caster port";
        document.getElementsByName("caster_port")[0].style.color = "grey";
    }

    if (mount_point) 
    {
        document.getElementsByName("mount_point")[0].placeholder = mount_point;
        document.getElementsByName("mount_point")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("mount_point")[0].placeholder = "Enter caster mountpoint";
        document.getElementsByName("mount_point")[0].style.color = "grey";
    }

    if (mount_point_pw) 
    {
        document.getElementsByName("mount_point_pw")[0].placeholder = mount_point_pw;
        document.getElementsByName("mount_point_pw")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("mount_point_pw")[0].placeholder = "Enter caster mountpoint PW";
        document.getElementsByName("mount_point_pw")[0].style.color = "grey";
    }

    if (survey_accuracy) 
    {
        document.getElementsByName("survey_accuracy")[0].placeholder = survey_accuracy;
        document.getElementsByName("survey_accuracy")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("survey_accuracy")[0].placeholder = "Enter desired survey in m";
        document.getElementsByName("survey_accuracy")[0].style.color = "grey";
    }

    if (latitude) 
    {
        document.getElementsByName("latitude")[0].placeholder = latitude;
        document.getElementsByName("latitude")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("latitude")[0].placeholder = "Enter high precision latitude";
        document.getElementsByName("latitude")[0].style.color = "grey";
    }

    if (longitude) 
    {
        document.getElementsByName("longitude")[0].placeholder = longitude;
        document.getElementsByName("longitude")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("longitude")[0].placeholder = "Enter high precision longitude";
        document.getElementsByName("longitude")[0].style.color = "grey";
    }

    if (altitude) 
    {
        document.getElementsByName("altitude")[0].placeholder = altitude;
        document.getElementsByName("altitude")[0].style.color = "black";
    } 
    else 
    {
        document.getElementsByName("altitude")[0].placeholder = "Enter high precision altitude";
        document.getElementsByName("altitude")[0].style.color = "grey";
    }

    console.log(ssid);
    console.log(password);
    console.log(device_name);
    console.log(mount_point);
    console.log(mount_point_pw);
    console.log(survey_accuracy);
    console.log(latitude);
    console.log(longitude);
    console.log(altitude);
    console.log(coord_accuracy);
}


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
    } 
    else 
    {
        document.getElementById("survey_accuracy").disabled = false;
        document.getElementById("latitude").disabled = true;
        document.getElementById("longitude").disabled = true;
        document.getElementById("altitude").disabled = true;
    }
}
</script>

<body onload="checkVariables();loadRadioState();enableLocationMethod();">

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
                    <input class="text_field" form="Form1" type="text" maxlength="30" name="ssid" placeholder={{ssid}}>
                </td>
            </tr>
            <tr>
                <td style="text-align:left;">Password:</td>
                <td>
                    <input class="text_field" form="Form1" type="text" maxlength="30" name="password" placeholder={{password}}>
                </td>
            </tr>
            <tr>
                <td colspan=2>
                    <h3>RTK base setup</h3>
                </td>
                <tr>
                    <td style="text-align:left;">Device name:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" name="device_name" placeholder={{device_name}}>
                    </td>
                </tr>
                <tr>
                    <td style="text-align:left;">Caster host:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="caster_host" placeholder={{caster_host}}>
                    </td>
                </tr>
                <tr>
                    <td style="text-align:left;">Caster port:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" name="caster_port" placeholder={{caster_port}}>
                    </td>
                </tr>
                <tr>
                    <td style="text-align:left;">Mount point:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="mount_point" placeholder={{mount_point}}>
                    </td>
                </tr>
                <tr>
                    <td style="text-align:left;">Mount point PW:</td>
                    <td>
                        <input class="text_field" form="Form1" type="text" maxlength="30" name="mount_point_pw" placeholder={{mount_point_pw}}>
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
                    <td><input title="The survey is carried out at minimum for 60 s or until the desired accuracy is achieved." class="text_field" form="Form1" type="text" maxlength="30" id="survey_accuracy" name="survey_accuracy" placeholder={{survey_accuracy}}></td>
                </tr>
                <tr>
                    <td style="text-align:left;"> Latitude, deg: </td>
                    <td><input class="text_field" form="Form1" type="text" maxlength="30" id="latitude" name="latitude" placeholder={{latitude}}></td>
                </tr>
                <tr>
                    <td style="text-align:left;"> Longitude, deg: </td>
                    <td><input class="text_field" form="Form1" type="text" maxlength="30" id="longitude" name="longitude" placeholder={{longitude}}> </td>
                </tr>
                <tr>
                    <td style="text-align:left;"> Altitude, m: </td>
                    <td><input title="Ellipsoid height of the antenna is required (float)." class="text_field" form="Form1" type="text" maxlength="30" id="altitude" name="altitude" placeholder={{altitude}}></td>
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


