//Generated from: ..\..\html\main15.html

const char* main_html = "\
<!DOCTYPE html>\n\
<html>\n\
\n\
<head>\n\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1\">\n\
\n\
  <style>\n\
    #title {\n\
      max-width: 300px;\n\
      border: none;\n\
      background-color: white;\n\
      border-radius: 10px 10px 0 0;\n\
      padding: 0.5em 1em;\n\
      color: #335b71;\n\
      font-family: Helvetica, Arial, sans-serif;\n\
    }\n\
\n\
    #form {\n\
      display: grid;\n\
      grid-template-columns: auto;\n\
      grid-auto-rows: 1fr;\n\
      max-width: 300px;\n\
      row-gap: 10px;\n\
      padding: 10px;\n\
\n\
      border: none;\n\
      background-color: white;\n\
      text-align: center;\n\
    }\n\
\n\
    #status-div {\n\
      display: grid;\n\
      grid-template-columns: auto;\n\
      grid-auto-rows: 1fr;\n\
      max-width: 300px;\n\
      row-gap: 10px;\n\
      padding: 10px;\n\
      border: none;\n\
      text-align: left;\n\
    }\n\
\n\
    #connected-div {\n\
      display: grid;\n\
      grid-template-columns: auto;\n\
      grid-auto-rows: 1fr;\n\
      max-width: 300px;\n\
      row-gap: 0em;\n\
      padding: 0em;\n\
      border: none;\n\
      text-align: left;\n\
    }\n\
\n\
    #brightness-div {\n\
      display: grid;\n\
      grid-template-columns: auto;\n\
      grid-auto-rows: 1fr;\n\
      max-width: 300px;\n\
      row-gap: 0em;\n\
      padding: 0em;\n\
      border: none;\n\
      padding: 0px 0px 0px 10px;\n\
      text-align: left;\n\
    }\n\
\n\
    button {\n\
      background: linear-gradient(to bottom, #7892c2 5%, #476e9e 100%);\n\
      background-color: #7892c2;\n\
      border-radius: 22px;\n\
      border: 1px solid #4e6096;\n\
      display: inline-block;\n\
      cursor: pointer;\n\
      color: #ffffff;\n\
      font-family: Arial;\n\
      font-size: 17px;\n\
      padding: 16px 41px;\n\
      text-decoration: none;\n\
      text-shadow: 0px 1px 0px #283966;\n\
    }\n\
\n\
    select {\n\
      color: #335b71;\n\
      text-decoration: none;\n\
      font-family: Helvetica, Arial, sans-serif;\n\
      font-size: 14px;\n\
      text-align: center;\n\
      padding: 5px;\n\
      line-height: 30px;\n\
      border-radius: 20px;\n\
      box-shadow: 0 2px 2px #888888;\n\
      outline: none;\n\
    }\n\
\n\
    input {\n\
      color: #335b71;\n\
      text-decoration: none;\n\
      font-family: Helvetica, Arial, sans-serif;\n\
      font-size: 14px;\n\
      padding: 0px;\n\
      border: none;\n\
      border-bottom: 1px solid #335b71;\n\
      background: transparent;\n\
      height: 40px;\n\
      border-radius: 0;\n\
    }\n\
\n\
    input:focus {\n\
      outline: none;\n\
    }\n\
\n\
    /*****************************/\n\
    .overlay {\n\
      position: fixed;\n\
      display: none;\n\
      width: 100%;\n\
      height: 100%;\n\
      top: 0;\n\
      left: 0;\n\
      right: 0;\n\
      bottom: 0;\n\
      background-color: rgba(0, 0, 0, 0.6);\n\
      z-index: 2;\n\
      cursor: pointer;\n\
    }\n\
\n\
    .spinner {\n\
      position: absolute;\n\
      top: 50%;\n\
      left: 50%;\n\
      font-size: 80px;\n\
      /*position: relative;*/\n\
      display: inline-block;\n\
      width: 1em;\n\
      height: 1em;\n\
    }\n\
\n\
    .spinner div {\n\
      position: absolute;\n\
      left: 0.4629em;\n\
      bottom: 0;\n\
      width: 0.074em;\n\
      height: 0.2777em;\n\
      border-radius: 0.5em;\n\
      background-color: transparent;\n\
      -webkit-transform-origin: center -0.2222em;\n\
      -ms-transform-origin: center -0.2222em;\n\
      transform-origin: center -0.2222em;\n\
      -webkit-animation: spinner-fade 1s infinite linear;\n\
      animation: spinner-fade 1s infinite linear;\n\
    }\n\
\n\
    .spinner div:nth-child(1) {\n\
      -webkit-animation-delay: 0s;\n\
      animation-delay: 0s;\n\
      -webkit-transform: rotate(0deg);\n\
      -ms-transform: rotate(0deg);\n\
      transform: rotate(0deg);\n\
    }\n\
\n\
    .spinner div:nth-child(2) {\n\
      -webkit-animation-delay: 0.083s;\n\
      animation-delay: 0.083s;\n\
      -webkit-transform: rotate(30deg);\n\
      -ms-transform: rotate(30deg);\n\
      transform: rotate(30deg);\n\
    }\n\
\n\
    .spinner div:nth-child(3) {\n\
      -webkit-animation-delay: 0.166s;\n\
      animation-delay: 0.166s;\n\
      -webkit-transform: rotate(60deg);\n\
      -ms-transform: rotate(60deg);\n\
      transform: rotate(60deg);\n\
    }\n\
\n\
    .spinner div:nth-child(4) {\n\
      -webkit-animation-delay: 0.249s;\n\
      animation-delay: 0.249s;\n\
      -webkit-transform: rotate(90deg);\n\
      -ms-transform: rotate(90deg);\n\
      transform: rotate(90deg);\n\
    }\n\
\n\
    .spinner div:nth-child(5) {\n\
      -webkit-animation-delay: 0.332s;\n\
      animation-delay: 0.332s;\n\
      -webkit-transform: rotate(120deg);\n\
      -ms-transform: rotate(120deg);\n\
      transform: rotate(120deg);\n\
    }\n\
\n\
    .spinner div:nth-child(6) {\n\
      -webkit-animation-delay: 0.415s;\n\
      animation-delay: 0.415s;\n\
      -webkit-transform: rotate(150deg);\n\
      -ms-transform: rotate(150deg);\n\
      transform: rotate(150deg);\n\
    }\n\
\n\
    .spinner div:nth-child(7) {\n\
      -webkit-animation-delay: 0.498s;\n\
      animation-delay: 0.498s;\n\
      -webkit-transform: rotate(180deg);\n\
      -ms-transform: rotate(180deg);\n\
      transform: rotate(180deg);\n\
    }\n\
\n\
    .spinner div:nth-child(8) {\n\
      -webkit-animation-delay: 0.581s;\n\
      animation-delay: 0.581s;\n\
      -webkit-transform: rotate(210deg);\n\
      -ms-transform: rotate(210deg);\n\
      transform: rotate(210deg);\n\
    }\n\
\n\
    .spinner div:nth-child(9) {\n\
      -webkit-animation-delay: 0.664s;\n\
      animation-delay: 0.664s;\n\
      -webkit-transform: rotate(240deg);\n\
      -ms-transform: rotate(240deg);\n\
      transform: rotate(240deg);\n\
    }\n\
\n\
    .spinner div:nth-child(10) {\n\
      -webkit-animation-delay: 0.747s;\n\
      animation-delay: 0.747s;\n\
      -webkit-transform: rotate(270deg);\n\
      -ms-transform: rotate(270deg);\n\
      transform: rotate(270deg);\n\
    }\n\
\n\
    .spinner div:nth-child(11) {\n\
      -webkit-animation-delay: 0.83s;\n\
      animation-delay: 0.83s;\n\
      -webkit-transform: rotate(300deg);\n\
      -ms-transform: rotate(300deg);\n\
      transform: rotate(300deg);\n\
    }\n\
\n\
    .spinner div:nth-child(12) {\n\
      -webkit-animation-delay: 0.913s;\n\
      animation-delay: 0.913s;\n\
      -webkit-transform: rotate(330deg);\n\
      -ms-transform: rotate(330deg);\n\
      transform: rotate(330deg);\n\
    }\n\
\n\
    @-webkit-keyframes spinner-fade {\n\
      0% {\n\
        /*background-color: #69717d;*/\n\
        background-color: white;\n\
      }\n\
\n\
      100% {\n\
        background-color: transparent;\n\
      }\n\
    }\n\
\n\
    @keyframes spinner-fade {\n\
      0% {\n\
        /*background-color: #69717d;*/\n\
        background-color: white;\n\
      }\n\
\n\
      100% {\n\
        background-color: transparent;\n\
      }\n\
    }\n\
  </style>\n\
\n\
  <script>\n\
    var retries;\n\
    var max_retries = 20;\n\
    var retry_timeout = 1000;\n\
    const g_apList = `$AP_LIST$`;\n\
\n\
    var apStatus = \"$AP_STATUS$\";\n\
\n\
    window.onload = function () {\n\
      initApList(g_apList);\n\
      setStatus(apStatus);\n\
      spinner_off();\n\
    };\n\
\n\
    function apEntry(ssid, db) {\n\
      this.ssid = ssid;\n\
      this.db = db;\n\
    }\n\
\n\
    function initApList(apList) {\n\
      var selectList = document.getElementById(\"ap-list\");\n\
\n\
      if (selectList != null) {\n\
        selectList.innerText = null;\n\
\n\
        var lines = apList.split(\"\\n\");\n\
\n\
        var apArray = new Array();\n\
        var maxLen = 0;\n\
        for (var l of lines) {\n\
          if (l.length > 0) {\n\
            var fields = l.split(\",\");\n\
            var ssid = fields[1].replace(/\"/g, '');\n\
            if (ssid.length > maxLen) {\n\
              maxLen = ssid.length;\n\
            }\n\
            var db = fields[2];\n\
            apArray.push(new apEntry(ssid, db));\n\
          }\n\
        }\n\
        apArray = apArray.sort((e1, e2) => e2.db - e1.db);\n\
        for (var e of apArray) {\n\
          var val = e.ssid;\n\
          var option = document.createElement(\"option\");\n\
          option.value = val;\n\
          option.text = val;\n\
          selectList.appendChild(option);\n\
        }\n\
      }\n\
    }\n\
\n\
    function check_input() {\n\
      if (document.getElementById(\"password\").value === \"\") {\n\
        document.getElementById(\"submit\").disabled = true;\n\
      }\n\
      else {\n\
        document.getElementById(\"submit\").disabled = false;\n\
      }\n\
    }\n\
\n\
    function spinner_on() {\n\
      document.getElementById(\"overlay-spinner\").style.display = \"block\";\n\
    }\n\
\n\
    function spinner_off() {\n\
      document.getElementById(\"overlay-spinner\").style.display = \"none\";\n\
    }\n\
\n\
    function disable_form() {\n\
      document.getElementById(\"submit\").disabled = true;\n\
      document.getElementById(\"ap-list\").disabled = true;\n\
      document.getElementById(\"password\").disabled = true;\n\
    }\n\
\n\
    function enable_form(is_success, error_msg) {\n\
      spinner_off();\n\
      document.getElementById(\"ap-list\").disabled = false;\n\
      document.getElementById(\"password\").disabled = false;\n\
\n\
      if (is_success) {\n\
        document.getElementById(\"password\").value = \"\";\n\
        document.getElementById(\"submit\").disabled = true;\n\
        document.getElementById(\"error\").innerHTML = \"\";\n\
      }\n\
      else {\n\
        document.getElementById(\"submit\").disabled = false;\n\
        document.getElementById(\"error\").innerHTML = error_msg;\n\
      }\n\
\n\
    }\n\
\n\
    function on_populate() {\n\
\n\
      disable_form();\n\
      spinner_on();\n\
\n\
      var xhttp = new XMLHttpRequest();\n\
      xhttp.timeout = 20000;\n\
      xhttp.onreadystatechange = function () {\n\
        if (this.readyState == 4) {\n\
          if (this.status == 200) {\n\
            initApList(this.responseText);\n\
            enable_form(true);\n\
          }\n\
          else {\n\
            enable_form(false, \"*** Populate AP: HTTP Error code: \" + this.status);\n\
          }\n\
        }\n\
      };\n\
\n\
      xhttp.open(\"GET\", \"/get_ap_list\", true);\n\
      xhttp.send();\n\
    }\n\
\n\
    function on_submit() {\n\
      disable_form();\n\
      spinner_on();\n\
      let ssid = document.getElementById(\"ap-list\").value;\n\
      let password = document.getElementById(\"password\").value;\n\
      setAPCfg(ssid, password);\n\
    }\n\
\n\
    function setAPCfg(ssid, password) {\n\
\n\
      retries = 0;\n\
      var xhttp = new XMLHttpRequest();\n\
      xhttp.timeout = 5000;\n\
\n\
      xhttp.onreadystatechange = function () {\n\
        if (this.readyState == 4) {\n\
          if (this.status == 200) {\n\
            restartQuery();\n\
          }\n\
          else {\n\
            enable_form(false, \"*** HTTP Error ***\");\n\
          }\n\
        }\n\
      };\n\
\n\
      xhttp.open(\"POST\", \"/set_ap_config\", true);\n\
      xhttp.send(ssid + \";\" + password);\n\
    }\n\
\n\
    function restartQuery() {\n\
      if (retries++ > max_retries) {\n\
        enable_form(false, \"*** HTTP Error ***\");\n\
      }\n\
      else {\n\
        window.setTimeout(queryAPCfg, 3000);\n\
      }\n\
    }\n\
\n\
    function setStatus(ap) {\n\
      var status = ap;\n\
      if (ap === \"\") {\n\
        status = \"*** Not connected ***\";\n\
      }\n\
      document.getElementById(\"status\").innerHTML = status;\n\
    }\n\
\n\
    function queryAPCfg() {\n\
\n\
      var xhttp = new XMLHttpRequest();\n\
      xhttp.timeout = retry_timeout;\n\
      xhttp.onreadystatechange = function () {\n\
        if (this.readyState == 4) {\n\
          if (this.status == 200) {\n\
            setStatus(this.responseText);\n\
            if (this.responseText != \"\") {\n\
              enable_form(true);\n\
            }\n\
            else {\n\
              enable_form(false, \"*** Authentication error ***\");\n\
            }\n\
          }\n\
          else {\n\
            restartQuery();\n\
          }\n\
        }\n\
      };\n\
\n\
      xhttp.open(\"GET\", \"/query_ap_config\", true);\n\
      xhttp.send();\n\
    }\n\
\n\
    function setBrightness(val) {\n\
\n\
      retries = 0;\n\
      var xhttp = new XMLHttpRequest();\n\
      xhttp.timeout = 5000;\n\
\n\
      xhttp.onreadystatechange = function () {\n\
        if (this.readyState == 4) {\n\
          if (this.status == 200) {\n\
          }\n\
        }\n\
        // document.getElementById(\"brightness\").disabled = false;\n\
      };\n\
\n\
      // document.getElementById(\"brightness\").disabled = true;\n\
      xhttp.open(\"POST\", \"/set_brightness\", true);\n\
      xhttp.send(val);\n\
    }\n\
\n\
    function closeModal(id) {\n\
      document.getElementById(id).style.display = \"none\";\n\
    }\n\
\n\
  </script>\n\
\n\
\n\
</head>\n\
\n\
<body>\n\
\n\
  <div id=\"title\">\n\
    <div style=\"text-align: center\">AP Configuration</div>\n\
  </div>\n\
\n\
  <div id=\"form\">\n\
    <button type=\"button\" class=\"form-button\" id=\"populate\" onClick=\"on_populate()\">Populate AP list</button>\n\
    <select name=\"ap-list\" id=\"ap-list\">\n\
    </select>\n\
    <input type=\"password\" id=\"password\" name=\"password\" placeholder=\"Password:\" onkeyup=\"check_input()\">\n\
    <button type=\"button\" class=\"form-button\" id=\"submit\" onClick=\"on_submit()\">Submit</button>\n\
  </div>\n\
  <div id=\"status-div\">\n\
    <div id=\"connected-div\">\n\
      Connected to:\n\
      <label id=\"status\" style=\"font-weight: bold;\">Not connected</label>\n\
    </div>\n\
    <label id=\"error\" style=\"color:red;\"></label>\n\
  </div>\n\
\n\
  <hr style=\"padding: 0; margin: 0 0 0 10px;\">\n\
\n\
  <div id=\"brightness-div\">\n\
    <label>Brightness:</label>\n\
    <input type=\"range\" id=\"brightness\" name=\"brightness\" min=\"0\" max=\"10\" step=\"1\" style=\"height: 20px;\"\n\
      oninput=\"setBrightness(this.value)\">\n\
  </div>\n\
\n\
\n\
  <div class=\"overlay\" id=\"overlay-spinner\">\n\
    <div class='spinner' id=\"spinner\">\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
      <div></div>\n\
    </div>\n\
  </div>\n\
\n\
</body>\n\
\n\
</html>";

