package com.example.homeguard.data

import com.jjoe64.graphview.series.DataPoint

class EnvironmentData() {
    private val maxDataPoints: Int = 9
    private var temperatureCArr: ArrayList<DataPoint> = ArrayList(maxDataPoints)
    private var temperatureFArr: ArrayList<DataPoint> = ArrayList(maxDataPoints)
    private var humidityArr: ArrayList<DataPoint> = ArrayList(maxDataPoints)

    fun addData(temperatureC: Double, temperatureF: Double, humidity: Double): ArrayList<DataPoint> {
        var dataRemoved: ArrayList<DataPoint> = ArrayList(2)

        if (temperatureCArr.size == maxDataPoints) {
            dataRemoved.add(temperatureCArr[0])
            dataRemoved.add(temperatureFArr[0])
            dataRemoved.add(humidityArr[0])

            temperatureCArr.removeAt(0)
            temperatureFArr.removeAt(0)
            humidityArr.removeAt(0)

            for (index in temperatureCArr.indices) {
                temperatureCArr[index] = DataPoint(index.toDouble(), temperatureCArr[index].y)
                temperatureFArr[index] = DataPoint(index.toDouble(), temperatureFArr[index].y)
                humidityArr[index] = DataPoint(index.toDouble(), humidityArr[index].y)
            }
        }
        temperatureCArr.add(DataPoint((temperatureCArr.size).toDouble(), temperatureC))
        temperatureFArr.add(DataPoint((temperatureFArr.size).toDouble(), temperatureF))
        humidityArr.add(DataPoint((humidityArr.size).toDouble(), humidity))

        return dataRemoved
    }

    fun resetData() {
        temperatureCArr = ArrayList(maxDataPoints)
        temperatureFArr = ArrayList(maxDataPoints)
        humidityArr = ArrayList(maxDataPoints)
    }

    fun getTempCDataPoints(): Array<DataPoint> {
        return this.temperatureCArr.toTypedArray()
    }
    fun getTempFDataPoints(): Array<DataPoint> {
        return this.temperatureFArr.toTypedArray()
    }

    fun getHumidityDataPoints(): Array<DataPoint> {
        return this.humidityArr.toTypedArray()
    }
}