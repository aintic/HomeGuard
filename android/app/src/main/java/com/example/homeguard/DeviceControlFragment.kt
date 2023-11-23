package com.example.homeguard

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.navigation.fragment.findNavController
import com.example.homeguard.databinding.DeviceControlFragmentBinding

/**
 * A simple [Fragment] subclass as the second destination in the navigation.
 */
class DeviceControlFragment : Fragment() {

    private var _binding: DeviceControlFragmentBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {

        _binding = DeviceControlFragmentBinding.inflate(inflater, container, false)
        return binding.root

    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.buttonSecond.setOnClickListener {
            findNavController().navigate(R.id.action_DeviceControl_to_DataDisplay)
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}