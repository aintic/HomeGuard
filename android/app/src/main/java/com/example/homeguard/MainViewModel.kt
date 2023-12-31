package com.example.homeguard

import androidx.lifecycle.ViewModel
import com.example.homeguard.data.EnvironmentData
import com.example.homeguard.data.FloodData

class MainViewModel : ViewModel() {
    var envData: EnvironmentData = EnvironmentData()
    var floodData: FloodData = FloodData()
    var buzzerOff: Boolean = false
    var fahrenheitMode = false;

    fun toggleBuzzerState() {
        buzzerOff = !buzzerOff
    }
    fun toggleFahrenheitMode() {
        fahrenheitMode = !fahrenheitMode
    }
}