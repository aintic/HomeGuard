package com.example.homeguard

import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener

class FirebaseConnection {
    private val database: DatabaseReference = FirebaseDatabase.getInstance().getReference("sensor")

    fun addDataListener(listener: ValueEventListener) {
        database.addValueEventListener(listener)
    }

    fun removeDataListener(listener: ValueEventListener) {
        database.removeEventListener(listener)
    }
}