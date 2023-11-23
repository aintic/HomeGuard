package com.example.homeguard

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.navigation.fragment.findNavController
import com.example.homeguard.databinding.DataDisplayFragmentBinding

/**
 * A simple [Fragment] subclass as the default destination in the navigation.
 */
class DataDisplayFragment : Fragment() {

    private var _binding: DataDisplayFragmentBinding? = null
//    private val db = Firebase.firestore
//    private var tempData = ""
//    private var humidityData = ""
//    private var statusData = "off"

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {

        _binding = DataDisplayFragmentBinding.inflate(inflater, container, false)
        return binding.root

    }



    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.buttonFirst.setOnClickListener {
            findNavController().navigate(R.id.action_DataDisplay_to_DeviceControl)
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}