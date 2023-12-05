package com.example.homeguard.data
import java.time.LocalDateTime

data class eventData(
    var waterLevel: Double,
    var timeOfOccurrence: LocalDateTime,
    var alertSent: Boolean,
    )

class FloodData {
    private val maxDataPoints: Int = 9
    private var eventLog: ArrayList<eventData> = ArrayList(maxDataPoints)

    fun addData(waterLevel: Double, alertSent: Boolean) {
        if (eventLog.size == maxDataPoints) {
            eventLog.removeAt(0)
        }

        var isFlooding: Boolean = waterLevel.equals(0)

        eventLog.add(eventData(waterLevel, LocalDateTime.now(), isFlooding))
    }

    fun resetData() {
        eventLog = ArrayList(maxDataPoints)
    }

    fun getTempDataPoints(): Array<eventData> {
        return this.eventLog.toTypedArray()
    }
}