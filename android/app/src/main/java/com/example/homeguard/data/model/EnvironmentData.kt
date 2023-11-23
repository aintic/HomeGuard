package com.example.homeguard.data.model

class EnvironmentData {
    var temperature: Float
    var humidity = 0f

    constructor() {
        temperature = 0f
        humidity = 0f
    }

    constructor(temperature: Float, humidity: Float) {
        this.temperature = temperature
        this.humidity = humidity
    }
}