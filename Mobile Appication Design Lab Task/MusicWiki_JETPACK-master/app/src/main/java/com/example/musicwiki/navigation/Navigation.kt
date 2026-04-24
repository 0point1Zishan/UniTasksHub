package com.example.musicwiki.navigation

import androidx.compose.runtime.Composable
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import com.example.musicwiki.screens.GenreListScreen
import com.example.musicwiki.screens.GenreDetailScreen
import com.example.musicwiki.viewmodel.MainViewModel

@Composable
fun Navigation(viewModel: MainViewModel) {

    val navController = rememberNavController()

    NavHost(
        navController = navController,
        startDestination = "genre_list"
    ) {
        composable("genre_list") {
            GenreListScreen(viewModel = viewModel, navController = navController)
        }
        
        composable("genre_detail/{genreName}") { backStackEntry ->
            val genreName = backStackEntry.arguments?.getString("genreName") ?: ""
            GenreDetailScreen(genreName = genreName, viewModel = viewModel, navController = navController)
        }
    }
}