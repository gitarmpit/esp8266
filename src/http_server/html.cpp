const char main14[] = "\
<!DOCTYPE html>\n\
<html>\n\
\n\
<head>\n\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1\">\n\
  \n\
  <style>\n\
  #title {\n\
    max-width: 300px; \n\
    /*min-height: 200px;*/ \n\
    border: 1px solid #335b71;\n\
    background-color: #88a2af;\n\
    border-radius: 10px 10px 0 0;\n\
    padding: 0.5em 1em ;\n\
    color: white;\n\
    font-family: Helvetica, Arial, sans-serif;\n\
  }\n\
\n\
  #form {\n\
    display: grid;\n\
    grid-template-columns: auto;\n\
    grid-auto-rows: 1fr;\n\
    max-width: 300px; \n\
    row-gap: 1em;\n\
    padding: 1em;\n\
\n\
    border: 1px solid #335b71;\n\
    border-top: none;\n\
    background-color: white;\n\
    border-radius: 0 0 10px 10px;\n\
    text-align: center;\n\
  }\n\
\n\
\n\
  button {\n\
      color: white;\n\
      text-decoration: none;\n\
      font-family: Helvetica, Arial, sans-serif;\n\
      font-size: 14px;\n\
      text-align: center;\n\
      padding: 5px;\n\
      line-height: 30px;\n\
      display: inline-block;\n\
      position: relative;\n\
      border-radius: 20px;\n\
      background-image: linear-gradient(#335b71 45%, #03324c 55%);\n\
      box-shadow: 0 2px 2px #888888;\n\
      transition: color 0.3s, background-image 0.5s, ease-in-out;\n\
      outline: none;\n\
  }\n\
\n\
  select { \n\
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
      background: transparent; \n\
      height: 20px;\n\
  }\n\
\n\
\n\
  input:focus{\n\
    outline: none;\n\
  }\n\
\n\
  </style>\n\
\n\
</head>\n\
\n\
<body>\n\
<div id=\"title\">\n\
  <div style=\"text-align: center\">AP Configuration</div>\n\
</div>\n\
\n\
<div id=\"form\">\n\
    <button type=\"button\" class=\"form-button\" id=\"populate\" onClick=\"populateApList()\">Populate AP list</button>\n\
    <select name=\"ssid\" id=\"ssid\">\n\
      <option value=\"ssid1\">ssid1</option>\n\
      <option value=\"ssid2\">ssid1</option>\n\
      <option value=\"ssid3\">ssid1</option>\n\
    </select>\n\
    <input type=\"password\" id=\"password\" name=\"password\">\n\
    <button type=\"button\" class=\"form-button\" id=\"submit\" onClick=\"populateApList()\">Submit</button>\n\
</div>\n\
\n\
</body>\n\
\n\
</html>\n\
";

