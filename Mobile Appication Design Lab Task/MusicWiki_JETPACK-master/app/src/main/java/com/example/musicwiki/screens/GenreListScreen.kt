package com.example.musicwiki.screens

import android.util.Log
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.navigation.NavController
import com.example.musicwiki.model.Tag
import com.example.musicwiki.viewmodel.MainViewModel

@Composable
fun GenreListScreen(
    viewModel: MainViewModel,
    navController: NavController
) {

    val genres by viewModel.genres.collectAsState()
    var isLoading by remember { mutableStateOf(true) }

    LaunchedEffect(Unit) {
        viewModel.getAllGenres()
    }

    LaunchedEffect(genres) {
        if (genres.isNotEmpty()) {
            isLoading = false
        }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp)
    ) {
        Text(
            text = "MusicWiki",
            style = MaterialTheme.typography.headlineLarge
        )
        
        Spacer(modifier = Modifier.height(16.dp))

        if (isLoading) {
            CircularProgressIndicator()
        } else {
            LazyColumn {
                items(genres) { genre: Tag ->
                    Card(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(vertical = 6.dp)
                            .clickable {
                                Log.d("GenreClick", genre.name)
                                navController.navigate("genre_detail/${genre.name}")
                            }
                    ) {
                        Text(
                            text = genre.name,
                            modifier = Modifier.padding(16.dp),
                            style = MaterialTheme.typography.titleMedium
                        )
                    }
                }
            }
        }
    }
}