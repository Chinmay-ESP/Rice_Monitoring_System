# Rice Monitoring System

## Overview
The **Rice Monitoring System** is an IoT-based project designed to collect real-time agricultural data using sensors and transmit it wirelessly to Google Sheets. The system facilitates data-driven decision-making by training a machine learning model on historical data to provide predictive insights for better rice crop management.

## Features
- **Sensor Data Collection:**
  - **pH Sensor**: Measures soil acidity.
  - **Soil Temperature Sensor**: Monitors soil temperature.
  - **Soil Moisture Sensor**: Determines soil moisture levels.
- **Wireless Data Transmission:**
  - Data is sent to **Google Sheets** using WiFi.
- **Google Sheets Integration:**
  - Data is stored and managed using Google Sheets.
  - An App Script function processes incoming data.
- **Machine Learning Integration:**
  - Data from Google Sheets is used to train an ML model.
  - Predictions are generated based on real-time sensor inputs.

## System Workflow
1. Sensors collect real-time data from the field.
2. Data is transmitted to Google Sheets via WiFi.
3. Google Apps Script processes and stores the data.
4. Machine Learning model trains on historical data.
5. The system predicts optimal conditions for rice growth based on real-time inputs.

## Google Apps Script (code.gs)
The following script handles data reception and storage in Google Sheets:

```javascript
function doPost(e) {
    var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
    var data = JSON.parse(e.postData.contents); // Parse incoming JSON
  
    // Append new data to the Google Sheet
    if (data.method === "append") {
      sheet.appendRow([new Date(),data.timestamp, data.temperature, data.fahrenheit, data.moisture, data.humidity, data.ph, data.n, data.p, data.k, data.buttonState]);
    } else if (data.method === "replace") {
      sheet.clear(); // Clear the sheet and replace the data
      sheet.appendRow(["Sheet Time","ESP32 Time", "Temperature", "Fahrenheit", "Moisture", "Humidity", "Ph", "N", "P", "K","Button State"]);
      sheet.appendRow([new Date(),data.timestamp, data.temperature, data.fahrenheit, data.moisture, data.humidity, data.ph, data.n, data.p, data.k, data.buttonState]);
    }
  
    return ContentService.createTextOutput("Data received successfully");
}
```

## Hardware Requirements
- **ESP32 Microcontroller**
- **pH Sensor**
- **Soil Temperature Sensor**
- **Soil Moisture Sensor**
- **WiFi Module (Built into ESP32)**

## Software Requirements
- **Arduino IDE / PlatformIO** (For ESP32 programming)
- **Google Sheets & Google Apps Script**
- **Python (for ML Model Training)**

## Installation & Setup
1. **Hardware Setup:** Connect sensors to the ESP32.
2. **Firmware Development:** Flash the ESP32 with the appropriate firmware using Arduino IDE or PlatformIO.
3. **Google Sheets Configuration:**
   - Create a new Google Sheet.
   - Open **Extensions > Apps Script** and paste the provided `code.gs`.
   - Deploy the script as a **web app** to receive data.
4. **Machine Learning Model Training:**
   - Export stored data from Google Sheets.
   - Train an ML model using Python libraries like **scikit-learn** or **TensorFlow**.
   - Deploy the trained model for real-time predictions.

## Future Enhancements
- **Cloud Integration:** Use Firebase or AWS for real-time data storage.
- **Automated Irrigation System:** Control irrigation based on predictions.
- **Mobile App:** Develop an Android/iOS app for remote monitoring.

## Contributors
- **Chinmay Chavan** (Developer & Researcher)

## License
This project is licensed under the MIT License.

