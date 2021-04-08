//Generated from: ..\..\html\main9.html

const char* main_html = "\
<!DOCTYPE html>\n\
<html>\n\
\n\
<head>\n\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1\">\n\
  <style>\n\
    :root {\n\
      --main-bg-color: #c1c2e6;\n\
      --botton-bg-color: #c1c2e6;\n\
      --botton-color: black;\n\
      --text-color: black;\n\
      --dialog-bg-color: #dadbed;\n\
      --form-bg-color: #dadbed;\n\
      --input-bg-color: white;\n\
    }\n\
\n\
    body {\n\
      background-color: var(--main-bg-color);\n\
    }\n\
\n\
    text {\n\
      color: var(--text-color);\n\
      white-space: nowrap;\n\
    }\n\
\n\
    h1 {\n\
      color: var(--text-color);\n\
    }\n\
\n\
    hr.rounded {\n\
      border-top: 4px solid black;\n\
      border-radius: 4px;\n\
    }\n\
\n\
    button {\n\
      color: var(--botton-color);\n\
      background-color: var(--botton-bg-color);\n\
      padding: 0.7em;\n\
      width: 100%;\n\
      border: 2px solid gray;\n\
      border-radius: 4px;\n\
    }\n\
\n\
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
    .dialog-div {\n\
      display: inline-grid;\n\
      grid-template-columns: auto auto;\n\
      position: absolute;\n\
      top: 50%;\n\
      left: 50%;\n\
      color: white;\n\
      transform: translate(-50%, -50%);\n\
      -ms-transform: translate(-50%, -50%);\n\
      background-color: var(--dialog-bg-color);\n\
      border: 8px solid var(--main-bg-color);\n\
      /*margin: 2rem auto 0 auto;*/\n\
      margin: 2px;\n\
      /*min-width: 50%;*/\n\
      padding: 1em;\n\
      border-radius: 4px;\n\
      justify-items: center;\n\
      align-items: center;\n\
    }\n\
\n\
    .button-grid {\n\
      grid-column: 1 / span 2;\n\
      width: 40%;\n\
      padding: 1em 0 0 60%;\n\
    }\n\
\n\
    .dialog-circle {\n\
      width: 30px;\n\
      height: 30px;\n\
      line-height: 28px;\n\
      border-radius: 50%;\n\
      border: 4px solid white;\n\
      color: white;\n\
      font-size: 20px;\n\
      font-family: verdana;\n\
      text-align: center;\n\
    }\n\
\n\
    #x {\n\
      color: white;\n\
      font-size: 20px;\n\
    }\n\
\n\
    .msg-text {\n\
      color: black;\n\
      padding: 0 0 0 10px;\n\
    }\n\
\n\
    #dialog-circle-ok {\n\
      background: #009900;\n\
    }\n\
\n\
    #dialog-circle-error {\n\
      background: #990000;\n\
    }\n\
\n\
    /*****************************/\n\
\n\
    #main-form {\n\
      display: grid;\n\
      padding: 1em 1em 1em 0;\n\
      background-color: var(--form-bg-color);\n\
      border: 2px solid black;\n\
      /*margin: 2rem auto 0 auto;*/\n\
      max-width: 400px;\n\
    }\n\
\n\
    #main-form input {\n\
      background: var(--input-bg-color);\n\
      border: 1px solid #9c9c9c;\n\
      border-radius: 4px;\n\
    }\n\
\n\
    #main-form button:hover {\n\
      border: 2px solid black;\n\
    }\n\
\n\
    label {\n\
      padding: 0.5em 0.5em 0.5em 0;\n\
    }\n\
\n\
    input {\n\
      padding: 0.7em;\n\
      margin-bottom: 0.5rem;\n\
    }\n\
\n\
    input:focus {\n\
      outline: 3px solid black;\n\
    }\n\
\n\
    @media (min-width: 300px) {\n\
      form {\n\
        grid-template-columns: 100px 1fr;\n\
        grid-gap: 16px;\n\
      }\n\
\n\
      label {\n\
        text-align: right;\n\
        grid-column: 1 / 2;\n\
      }\n\
\n\
      input,\n\
      button {\n\
        grid-column: 2 / 3;\n\
      }\n\
    }\n\
\n\
    .wrapper {\n\
      display: grid;\n\
    }\n\
\n\
    /*****************************/\n\
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
    window.onload = function(){\n\
      initApList(g_apList);\n\
      document.getElementById(\"status\").innerHTML = apStatus;\n\
    };\n\
\n\
    function apEntry(ssid, db) \n\
    {\n\
        this.ssid = ssid;\n\
        this.db = db;\n\
    }\n\
\n\
    function initApList(apList) \n\
    {\n\
      var selectList = document.getElementById(\"ap-list\");\n\
\n\
      if (selectList != null)  {\n\
        selectList.innerText = null;\n\
        \n\
        var lines = apList.split(\"\\n\");\n\
        \n\
        var apArray = new Array();\n\
        var maxLen = 0;\n\
        for (var l of lines) \n\
        {\n\
          if (l.length > 0) {\n\
            var fields = l.split(\",\");\n\
            var ssid = fields[1].replace(/\"/g, '');\n\
            if (ssid.length > maxLen) {\n\
               maxLen = ssid.length;\n\
            }\n\
            var db = fields[2];\n\
            apArray.push(new apEntry(ssid, db));  \n\
          }\n\
        }\n\
        apArray = apArray.sort((e1, e2) => e2.db - e1.db);\n\
        for (var e of apArray) \n\
        {\n\
            var val = e.ssid;\n\
            var option = document.createElement(\"option\");\n\
            option.value = val;\n\
            option.text = val;\n\
            selectList.appendChild(option);\n\
        }\n\
      }   \n\
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
    function on_submit() {\n\
      disable_form();\n\
      spinner_on();\n\
      let ssid = document.getElementById(\"ap-list\").value;\n\
      let password = document.getElementById(\"password\").value;\n\
      setAPCfg(ssid, password);\n\
    }\n\
\n\
    function disable_form() {\n\
      document.getElementById(\"submit\").disabled = true;\n\
      document.getElementById(\"ap-list\").disabled = true;\n\
      document.getElementById(\"password\").disabled = true;\n\
    } \n\
\n\
    function enable_form(is_success, msg) {\n\
      spinner_off();\n\
      document.getElementById(\"ap-list\").disabled = false;\n\
      document.getElementById(\"password\").disabled = false;\n\
\n\
      if (is_success) {\n\
        document.getElementById(\"password\").value = \"\";\n\
        document.getElementById(\"submit\").disabled = true;\n\
      }\n\
      else {\n\
        document.getElementById(\"submit\").disabled = false;\n\
      }\n\
\n\
      if (msg.length > 0) \n\
      {\n\
        document.getElementById(\"status\").innerHTML = msg;\n\
      }\n\
\n\
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
            enable_form(false, \"set_ap_config_error\");\n\
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
        enable_form(false, \"set_ap_config error\");\n\
      }\n\
      else {\n\
        window.setTimeout(queryAPCfg, 3000);\n\
      }\n\
    }\n\
\n\
    function queryAPCfg() {\n\
\n\
      var xhttp = new XMLHttpRequest();\n\
      xhttp.timeout = retry_timeout;\n\
      xhttp.onreadystatechange = function () {\n\
        if (this.readyState == 4) {\n\
          if (this.status == 200) {\n\
            let response = this.responseText;\n\
            enable_form(true, response);\n\
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
    function closeModal(id) {\n\
      document.getElementById(id).style.display = \"none\";\n\
    }\n\
\n\
    function on_populate() {\n\
\n\
            disable_form();\n\
            spinner_on();\n\
\n\
            var xhttp = new XMLHttpRequest();\n\
            xhttp.timeout = retry_timeout;\n\
            xhttp.onreadystatechange = function () {\n\
              if (this.readyState == 4) {\n\
                if (this.status == 200) {\n\
                  initApList(this.responseText);\n\
                  enable_form(true, \"\");\n\
                }\n\
                else {\n\
                  enable_form(false, \"get_ap_list error\");\n\
                }\n\
                spinner_off();\n\
              }\n\
            };\n\
      \n\
            xhttp.open(\"GET\", \"/get_ap_list\", true);\n\
            xhttp.send();\n\
    }\n\
\n\
\n\
  </script>\n\
\n\
</head>\n\
\n\
<body>\n\
\n\
  <div class=\"wrapper\">\n\
    <div>\n\
      <div style=\"text-align: center\">AP Configuration</div>\n\
    </div>\n\
    <div>\n\
      <form id=\"main-form\">\n\
        <button type=\"button\" class=\"button\" id=\"populate\" onClick=\"on_populate()\">Populate AP list</button>\n\
        <label for=\"ap-list\">SSID:</label>\n\
        <select name=\"ap-list\" id=\"ap-list\">\n\
        </select>\n\
        <label for=\"password\">Password:</label>\n\
        <input type=\"text\" id=\"password\" name=\"password\" onkeyup=\"check_input()\">\n\
        <button type=\"button\" class=\"button\" id=\"submit\" onClick=\"on_submit()\"  disabled>Submit</button>\n\
      </form>\n\
      Status: <label id=\"status\">???</label>\n\
    </div>\n\
\n\
  </div>\n\
  <div>\n\
    <hr class=\"rounded\">\n\
  </div>\n\
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
  <div class=\"overlay\" id=\"overlay-dialog-ok\">\n\
    <div class=\"dialog-div\">\n\
        <div class='dialog-circle' id=\"dialog-circle-ok\">\n\
          <div>✓</div>\n\
        </div>\n\
        <div><text class=\"msg-text\">AP successfully configured.</text></div>\n\
        <div class=\"button-grid\"><button class=\"dialog-button\" onclick=\"closeModal('overlay-dialog-ok')\">OK</button></div>\n\
    </div>\n\
  </div>\n\
\n\
  <div class=\"overlay\" id=\"overlay-dialog-error\">\n\
    <div class=\"dialog-div\">\n\
        <div class='dialog-circle' id=\"dialog-circle-error\">\n\
          <div id=\"x\">✕</div>\n\
        </div>\n\
        <div><text class=\"msg-text\">Error configuring AP.</text></div>\n\
        <div class=\"button-grid\"><button class=\"dialog-button\" onclick=\"closeModal('overlay-dialog-error')\">OK</button></div>\n\
    </div>\n\
  </div>\n\
\n\
\n\
</body>\n\
\n\
</html>";

