package com.example.homeguard

import android.content.ContentValues
import android.graphics.Color
import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.fragment.app.activityViewModels
import androidx.navigation.fragment.findNavController
import com.example.homeguard.data.FloodData
import com.example.homeguard.databinding.FloodDataFragmentBinding
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.ValueEventListener
import java.math.RoundingMode

/**
 * A simple [Fragment] subclass as the second destination in the navigation.
 */
class FloodDataFragment : Fragment() {

    private var _binding: FloodDataFragmentBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    private val mainViewModel: MainViewModel by activityViewModels()
    private val floodData: FloodData
        get() = mainViewModel.floodData

    private val firebaseDB = FirebaseConnection()
    private val dataListener = object: ValueEventListener {
        override fun onDataChange(dataSnapshot: DataSnapshot) {
            // This method is called once with the initial value and again
            // whenever data at this location is updated.
            val value = dataSnapshot.value

            if (value is Map<*, *>) {
                val waterLevel: Double = (value["waterLevel"].toString().toDouble()).toBigDecimal().setScale(2, RoundingMode.UP).toDouble()
                Log.d(ContentValues.TAG, "Water Level: $waterLevel")

                val waterLevelView: TextView? = view?.findViewById(R.id.water_level_data)
                val statusView: TextView? = view?.findViewById(R.id.status_data)

                if (waterLevelView != null && statusView != null) {

                    waterLevelView.text = "$waterLevel"

                    if (waterLevel.equals(0.0)) {
                        Log.d(ContentValues.TAG, "Inside loop normal")
                        statusView.text = "Normal"
                        statusView.setTextColor(Color.rgb(76, 175, 80));

                    }
                    else {
                        floodData.addData(waterLevel, true)
                        Log.d(ContentValues.TAG, "Inside loop alert")
                        statusView.text = "Alert"
                        statusView.setTextColor(Color.rgb(233, 30, 99));
                    }
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

        _binding = FloodDataFragmentBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.buttonSecond.setOnClickListener {
            findNavController().navigate(R.id.action_FloodData_to_EnvironmentData)
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