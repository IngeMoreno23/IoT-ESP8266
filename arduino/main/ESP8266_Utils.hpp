void connectWifi(){
    Serial.println("Conectando a WiFi...");
    WiFi.begin(ssid, password);

    // Esperar a que se conecte
    Serial.print("Conectando");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println();

        Serial.print("Conectado, dirección IP: ");
        Serial.println(WiFi.localIP());
}

void checkWifiConnection(){
    if (WiFi.status() != WL_CONNECTED){
        Serial.println("Conexión perdida, intentando reconectar...");
        connectWifi();
    }
}