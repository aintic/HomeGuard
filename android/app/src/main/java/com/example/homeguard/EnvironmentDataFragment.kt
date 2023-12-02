package com.example.homeguard

import android.content.ContentValues
import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.navigation.fragment.findNavController
import com.example.homeguard.databinding.EnvironmentDataFragmentBinding
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.ValueEventListener

/**
 * A simple [Fragment] subclass as the default destination in the navigation.
 */
class EnvironmentDataFragment : Fragment() {

    private var _binding: EnvironmentDataFragmentBinding? = null
    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    private val firebaseDB = FirebaseConnection()
    private val dataListener = object: ValueEventListener {
        override fun onDataChange(dataSnapshot: DataSnapshot) {
            // This method is called once with the initial value and again
            // whenever data at this location is updated.
            val value = dataSnapshot.value

            if (value is Map<*, *>) {
                val temp = value["temperature"]
                val humidity = value["humidity"]
                Log.d(ContentValues.TAG, "Temperature: $temp, Humidity: $humidity")
                val tempView: TextView? = view?.findViewById(R.id.temperature_data)
                val humidView: TextView? = view?.findViewById(R.id.humidity_data)
                if (tempView != null && humidView != null) {
                    tempView.text = "$temp"
                    humidView.text = "$humidity"
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