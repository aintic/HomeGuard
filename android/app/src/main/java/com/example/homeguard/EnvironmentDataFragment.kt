package com.example.homeguard

import android.content.ContentValues
import android.graphics.Color
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.navigation.fragment.findNavController
import com.example.homeguard.data.EnvironmentData
import com.example.homeguard.databinding.EnvironmentDataFragmentBinding
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.ValueEventListener
import com.jjoe64.graphview.GraphView
import com.jjoe64.graphview.series.DataPoint
import com.jjoe64.graphview.series.LineGraphSeries
import java.math.RoundingMode
import java.util.*

/**
 * A simple [Fragment] subclass as the default destination in the navigation.
 */
class EnvironmentDataFragment : Fragment() {

    private var _binding: EnvironmentDataFragmentBinding? = null
    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    private val mainViewModel: MainViewModel by activityViewModels()
    private val envData: EnvironmentData
        get() = mainViewModel.envData

    private val firebaseDB = FirebaseConnection("sensor")
    private val dataListener = object: ValueEventListener {
        override fun onDataChange(dataSnapshot: DataSnapshot) {
            // This method is called once with the initial value and again
            // whenever data at this location is updated.
            val value = dataSnapshot.value

            if (value is Map<*, *>) {
                val temperature: Double = (value["temperatureC"].toString().toDouble()).toBigDecimal().setScale(2, RoundingMode.UP).toDouble()
                val humidity: Double = (value["humidity"].toString().toDouble()).toBigDecimal().setScale(2, RoundingMode.UP).toDouble()

                Log.d(ContentValues.TAG, "Temperature: $temperature, Humidity: $humidity")

                val tempView: TextView? = view?.findViewById(R.id.temperature_data)
                val humidView: TextView? = view?.findViewById(R.id.humidity_data)
                val tempGraph: GraphView? = view?.findViewById(R.id.temperatureGraph)
                val humidGraph: GraphView? = view?.findViewById(R.id.humidityGraph)

                if (tempView != null && humidView != null) {
                    tempView.text = "$temperature"
                    humidView.text = "$humidity"

                    envData.addData(temperature, humidity)

                    tempGraph?.removeAllSeries()
                    humidGraph?.removeAllSeries()

                    val tempSeries: LineGraphSeries<DataPoint> = LineGraphSeries(envData.getTempDataPoints())
//                    tempSeries.setAnimated(true)
                    tempSeries.color = Color.rgb(255, 87, 37)
                    tempGraph?.addSeries(tempSeries)

                    val humidSeries: LineGraphSeries<DataPoint> = LineGraphSeries(envData.getHumidityDataPoints())
//                    humidSeries.setAnimated(true)
                    humidSeries.color = Color.rgb(57, 181, 236)
                    humidGraph?.addSeries(humidSeries)
                }
            }
        }

        override fun onCancelled(error: DatabaseError) {
            // Failed to read value
            Log.w(ContentValues.TAG, "Failed to read value.", error.toException())
        }
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {

        _binding = EnvironmentDataFragmentBinding.inflate(inflater, container, false)

        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.buttonFirst.setOnClickListener {
            findNavController().navigate(R.id.action_EnvironmentData_to_FloodData)
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    override fun onStart() {
        super.onStart()
        // Add the ValueEventListener when the fragment starts
        firebaseDB.addDataListener(dataListener)
    }

    override fun onStop() {
        super.onStop()
        // Remove the ValueEventListener when the fragment stops
        firebaseDB.removeDataListener(dataListener)
    }
}