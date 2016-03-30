void WebListenForClients() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    String currentLine = "";
    String requestedUrl = "";
    String requestVerb = "";
    int contentLength = -1;
    bool inContent = false;
    String content = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank && contentLength == -1) {

          Serial.println("REQUESTED URL: [" + requestedUrl + "]" + " (" + requestVerb + ")");

          process_request(requestedUrl, requestVerb, content);
          
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          
          client.println(Html_Config());

          requestedUrl = "";
          requestVerb = "";
          break;
        }
        if (c == '\n' && currentLineIsBlank && contentLength >= 0) {
          inContent = true;
        }
        if(inContent == true){
          content = (content + c);
          if(content.length() > contentLength) {
            process_request(requestedUrl, requestVerb, content);
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            
            client.println(Html_Config());
  
            requestedUrl = "";
            requestVerb = "";
            break;
          }
        }
        if (c == '\n' && inContent == false) {
          // you're starting a new line
          Serial.println("LINE: " + currentLine);

          if (currentLine.startsWith("GET ")) {
            requestVerb = "GET";
            requestedUrl = currentLine.substring(4, (currentLine.indexOf(" ", 4)));
          }

          if (currentLine.startsWith("POST ")) {
            requestVerb = "POST";
            requestedUrl = currentLine.substring(5, (currentLine.indexOf(" ", 5)));
          }

          if (currentLine.startsWith("Content-Length: ")) {
            contentLength = currentLine.substring(16, (currentLine.indexOf(" ", 16))).toInt();
          }

          // reset line-tracking for next line
          currentLine = "";
          currentLineIsBlank = true;
        }
        else if (c != '\r' && inContent == false) {
          // you've gotten a character on the current line
          currentLine = (currentLine + c);
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

void process_request(String requestedUrl, String verb, String content){
  Serial.println("processing URL: (" + verb + ") " + requestedUrl);
  Serial.println("Content: " + content);
  
  if(verb == "POST" && content.length() > 0){
    distance = GetQueryStringValue(content, "distance").toFloat();
    distance_mm_multiplier = GetQueryStringValue(content, "distance_mm_multiplier").toFloat();
    duration = GetQueryStringValue(content, "duration").toFloat();
    duration_sec_multiplier = GetQueryStringValue(content, "duration_sec_multiplier").toFloat();

    pull_distance_over_time(distance, distance_mm_multiplier, duration, duration_sec_multiplier);
  }
  
}

String GetQueryStringValue(String query, String param) {
  String value = "";
  int paramLoc = query.indexOf(param);
  if(paramLoc > -1){
    // value exists, go get it
    int valueStartLoc = query.indexOf("=",paramLoc) + 1;
    int valueEndLoc = (query + "&").indexOf("&",valueStartLoc);
    value = query.substring(valueStartLoc, valueEndLoc);
  }
  Serial.println("Extracted value: " + param + " = " + value);
  return value;
}
