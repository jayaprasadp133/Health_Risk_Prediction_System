# Cloud-Based Intelligent Health Risk Prediction System 🌍💓

This project aims to predict health risks using wearable health band data and real-time environmental data, providing preventive lifestyle recommendations. It combines **IoT**, **Machine Learning**, and **Cloud APIs** for a full-stack health monitoring system.

---

## 🔧 Components

- **NodeMCU ESP32/ESP8266**
  - Reads data from MAX30100 (Heart Rate, SpO2)
  - Calculates HRV, Respiration, Stress, Blood Pressure
  - Sends data to ThingSpeak via HTTP
- **Google Colab (Python)**
  - Applies ML models for health risk prediction
  - Generates lifestyle recommendations

---

## 📁 Folder Structure

HealthRiskPredictionSystem/
├── NodeMCU_Code/
│ └── health_monitoring_nodemcu.ino
├── GoogleColab_Notebook/
│ └── health_risk_prediction.ipynb
├── README.me


---

## 🚀 Features

- Real-time health data collection via MAX30100
- Heart rate, SpO2, Stress, BP estimation
- Respiration rate and HRV-based stress levels
- Machine learning model for predicting health risk levels
- Lifestyle medicine recommendations
- Cloud dashboard via ThingSpeak

---

## 📡 Technologies Used

- C++ (ESP32 Arduino)
- Python (Google Colab)
- ThingSpeak API
- GSM/Location Tracking
- MAX30100 Sensor
- WiFi/HTTP for IoT communication

---

## 🔗 Live Notebook

[📘 Google Colab Notebook](https://colab.research.google.com/drive/1-ITufjy4a4afelHyJp9xl5jF-FBefqx0?usp=sharing)

---

## 📌 Author

**Jaya Prasad P**  
LinkedIn: [in/jayaprasadp](https://www.linkedin.com/in/jayaprasadp)  
ORCID: [0009-0007-3720-855X](https://orcid.org/0009-0007-3720-855X)

---

## 📄 License

This project is open-source under the MIT License.
