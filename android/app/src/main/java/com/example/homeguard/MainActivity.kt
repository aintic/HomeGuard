package com.example.homeguard

import android.annotation.SuppressLint
import android.graphics.Color
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.splashscreen.SplashScreen.Companion.installSplashScreen
import androidx.lifecycle.ViewModelProvider
import androidx.navigation.findNavController
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import androidx.navigation.ui.setupActionBarWithNavController
import com.example.homeguard.databinding.ActivityMainBinding
import com.google.android.material.snackbar.Snackbar
import com.jjoe64.graphview.GraphView
import com.jjoe64.graphview.series.DataPoint
import com.jjoe64.graphview.series.LineGraphSeries

class MainActivity : AppCompatActivity() {

    private lateinit var appBarConfiguration: AppBarConfiguration
    private lateinit var binding: ActivityMainBinding
    private lateinit var mainViewModel: MainViewModel

    @SuppressLint("SetTextI18n")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        installSplashScreen()

        binding = ActivityMainBinding.inflate(layoutInflater)

        setContentView(binding.root)

        setSupportActionBar(binding.toolbar)

        mainViewModel = ViewModelProvider(this)[MainViewModel::class.java]

        val navController = findNavController(R.id.nav_host_fragment_content_main)
        appBarConfiguration = AppBarConfiguration(navController.graph)
        setupActionBarWithNavController(navController, appBarConfiguration)

        binding.fab.setOnClickListener { view ->
            val firebaseDB = FirebaseConnection("app")

            mainViewModel.toggleBuzzerState()
            var buzzerState = "on"
            if (mainViewModel.buzzerOff) {
                buzzerState = "off"
            }

            firebaseDB.getDatabaseRef().child("buzzerOff").setValue(mainViewModel.buzzerOff)

            Snackbar.make(view, "Buzzer $buzzerState.", Snackbar.LENGTH_LONG)
                .setAction("Action", null).show()
        }

    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        return when (item.itemId) {
            R.id.temperature_mode -> {
                val tempLabel: TextView? = findViewById(R.id.temperature_label)
                mainViewModel.toggleFahrenheitMode()

                if (mainViewModel.fahrenheitMode && tempLabel != null) {
                    tempLabel.text = "Temperature (F)"
                    item.title = "Celcius"
                    val tempGraph: GraphView? = findViewById(R.id.temperatureGraph)
                    tempGraph?.removeAllSeries()

                    val tempView: TextView? = findViewById(R.id.temperature_data)
                    if (tempView != null) {
                        tempView.text = "${mainViewModel.envData.getTempFDataPoints().last().y}"
                    }

                    val tempSeries: LineGraphSeries<DataPoint> = LineGraphSeries(mainViewModel.envData.getTempFDataPoints())
                    tempSeries.color = Color.rgb(255, 87, 37)
                    tempGraph?.addSeries(tempSeries)
                }
                else if (!mainViewModel.fahrenheitMode && tempLabel != null) {
                    tempLabel.text = "Temperature (C)"
                    item.title = "Fahrenheit"
                    val tempGraph: GraphView? = findViewById(R.id.temperatureGraph)
                    tempGraph?.removeAllSeries()

                    val tempView: TextView? = findViewById(R.id.temperature_data)
                    if (tempView != null) {
                        tempView.text = "${mainViewModel.envData.getTempCDataPoints().last().y}"
                    }

                    val tempSeries: LineGraphSeries<DataPoint> = LineGraphSeries(mainViewModel.envData.getTempCDataPoints())
                    tempSeries.color = Color.rgb(255, 87, 37)
                    tempGraph?.addSeries(tempSeries)
                }
                true
            }

            else -> super.onOptionsItemSelected(item)
        }
    }

    override fun onSupportNavigateUp(): Boolean {
        val navController = findNavController(R.id.nav_host_fragment_content_main)
        return navController.navigateUp(appBarConfiguration)
                || super.onSupportNavigateUp()
    }
}