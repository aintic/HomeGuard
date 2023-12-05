package com.example.homeguard.data

import android.content.ContentValues
import android.util.Log
import com.jjoe64.graphview.series.DataPoint

class EnvironmentData() {
    private val maxDataPoints: Int = 9
    private var temperatureArr: ArrayList<DataPoint> = ArrayList(maxDataPoints)
    private var humidityArr: ArrayList<DataPoint> = ArrayList(maxDataPoints)

    fun addData(temperature: Double, humidity: Double): ArrayList<DataPoint> {
        var dataRemoved: ArrayList<DataPoint> = ArrayList(2)

        if (temperatureArr.size == maxDataPoints) {
            dataRemoved.add(temperatureArr[0])
            dataRemoved.add(humidityArr[0])

            temperatureArr.removeAt(0)
            humidityArr.removeAt(0)

            for (index in temperatureArr.indices) {
                temperatureArr[index] = DataPoint(index.toDouble(), temperatureArr[index].y)
                humidityArr[index] = DataPoint(index.toDouble(), humidityArr[index].y)
            }
        }
        temperatureArr.add(DataPoint((temperatureArr.size).toDouble(), temperature))
        humidityArr.add(DataPoint((humidityArr.size).toDouble(), humidity))

        return dataRemoved
    }

    fun resetData() {
        temperatureArr = ArrayList(maxDataPoints)
        humidityArr = ArrayList(maxDataPoints)
    }

    fun getTempDataPoints(): Array<DataPoint> {
        return this.temperatureArr.toTypedArray()
    }

    fun getHumidityDataPoints(): Array<DataPoint> {
        return this.humidityArr.toTypedArray()
    }
}